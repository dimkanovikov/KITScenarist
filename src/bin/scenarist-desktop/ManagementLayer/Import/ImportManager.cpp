#include "ImportManager.h"

#include <Domain/Research.h>
#include <Domain/Scenario.h>

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/Import/CeltxImporter.h>
#include <BusinessLayer/Import/KitScenaristImporter.h>
#include <BusinessLayer/Import/DocumentImporter.h>
#include <BusinessLayer/Import/FdxImporter.h>
#include <BusinessLayer/Import/TrelbyImporter.h>
#include <BusinessLayer/Import/FountainImporter.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/DataStorageLayer/ResearchStorage.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <UserInterfaceLayer/Import/ImportDialog.h>

#include <3rd_party/Helpers/ImageHelper.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QApplication>
#include <QFile>
#include <QSet>

using ManagementLayer::ImportManager;
using UserInterface::ImportDialog;

namespace {
    /**
     * @brief Старый вордовский формат не поддерживается
     */
    const QString kMsDocExtension = ".doc";

    /**
     * @brief Форматы файлов импорт которых поддерживается
     */
    /** @{ */
    const QString kKitScenaristExtension = ".kitsp";
    const QString kFinalDraftExtension = ".fdx";
    const QString kFinalDraftTemplateExtension = ".fdxt";
    const QString kTrelbyExtension = ".trelby";
    const QString kFountainExtension = ".fountain";
    const QString kCeltxExtension = ".celtx";
    /** @} */

    /**
     * @brief Сохранить импортированный документ разработки со вложенными документами
     */
    static void storeResearchDocument(const QVariantMap& _documentData, Domain::Research* _parent) {
        //
        // Т.к. разработка может быть довольно большой, то нужно давать выполняться гуи-потоку,
        // чтобы не было ощущения зависания
        //
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        //
        // Загружаем базовые поля
        //
        const int type = _documentData["type"].toInt();
        const int sortOrder = _documentData["sort_order"].toInt();
        const QString name = _documentData["name"].toString();
        auto* document = DataStorageLayer::StorageFacade::researchStorage()->storeResearch(_parent, type, sortOrder, name);
        //
        // Установим описание
        //
        document->setDescription(_documentData["description"].toString());
        //
        // Установим ссылку, если есть
        //
        if (_documentData.contains("url")) {
            document->setUrl(_documentData["url"].toString());
        }
        //
        // Установим картинку, если есть
        //
        if (_documentData.contains("image")) {
            document->setImage(ImageHelper::imageFromBytes(_documentData["image"].toByteArray()));
        }
        //
        // Загрузим дочерние элементы, если есть
        //
        if (_documentData.contains("childs")) {
            for (const QVariant& childDocumentData : _documentData["childs"].toList()) {
                storeResearchDocument(childDocumentData.toMap(), document);
            }
        }
    }

    /**
     * @brief Сохранить импортированного персонажа
     */
    static void storeCharacter(const QVariantMap& _characterData) {
        const QString name = _characterData["name"].toString();
        auto* character = DataStorageLayer::StorageFacade::researchStorage()->character(name);
        if (character == nullptr) {
            character = DataStorageLayer::StorageFacade::researchStorage()->storeCharacter(name);
        }
        character->addDescription(_characterData["description"].toString());

        for (const QVariant& childDocumentData : _characterData["childs"].toList()) {
            storeResearchDocument(childDocumentData.toMap(), character);
        }
    }

    /**
     * @brief Сохранить импортированную локацию
     */
    static void storeLocation(const QVariantMap& _locationData) {
        const QString name = _locationData["name"].toString();
        auto* location = DataStorageLayer::StorageFacade::researchStorage()->location(name);
        if (location == nullptr) {
            location = DataStorageLayer::StorageFacade::researchStorage()->storeLocation(name);
        }
        location->addDescription(_locationData["description"].toString());

        for (const QVariant& childDocumentData : _locationData["childs"].toList()) {
            storeResearchDocument(childDocumentData.toMap(), location);
        }
    }
}


ImportManager::ImportManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_importDialog(new ImportDialog(_parentWidget))
{
    initView();
    initConnections();
}

bool ImportManager::importScenario(BusinessLogic::ScenarioDocument* _scenario, int _cursorPosition,
    const BusinessLogic::ImportParameters& _importParameters)
{
    //
    // Получим xml-представление импортируемого сценария
    //
    QScopedPointer<BusinessLogic::AbstractImporter> importer;
    if (_importParameters.filePath.toLower().endsWith(kKitScenaristExtension)) {
        importer.reset(new BusinessLogic::KitScenaristImporter);
    } else if (_importParameters.filePath.toLower().endsWith(kFinalDraftExtension)
               || _importParameters.filePath.toLower().endsWith(kFinalDraftTemplateExtension)) {
        importer.reset(new BusinessLogic::FdxImporter);
    } else if (_importParameters.filePath.toLower().endsWith(kTrelbyExtension)) {
        importer.reset(new BusinessLogic::TrelbyImporter);
    } else if (_importParameters.filePath.toLower().endsWith(kFountainExtension)) {
        importer.reset(new BusinessLogic::FountainImporter);
    } else if (_importParameters.filePath.toLower().endsWith(kCeltxExtension)) {
        importer.reset(new BusinessLogic::CeltxImporter);
    } else {
        importer.reset(new BusinessLogic::DocumentImporter);
    }
    const QString importScenarioXml = importer->importScript(_importParameters);

    //
    // Если нету текста, прерываем выполнение
    //
    if (importScenarioXml.isEmpty()) {
        return false;
    }

    //
    // Загрузим импортируемый текст в сценарий
    //
    // ... определим позицию вставки
    //
    int insertPosition = 0;
    switch (_importParameters.insertionMode) {
        case BusinessLogic::ImportParameters::ReplaceDocument: {
            _scenario->clear();
            insertPosition = 0;
            break;
        }

        case BusinessLogic::ImportParameters::ToCursorPosition: {
            insertPosition = _cursorPosition;
            break;
        }

        default:
        case BusinessLogic::ImportParameters::ToDocumentEnd: {
            insertPosition = _scenario->document()->characterCount() - 1;
            break;
        }
    }
    //
    // ... загрузим текст
    //
    _scenario->document()->insertFromMime(insertPosition, importScenarioXml);

    //
    // ... в случае необходимости определяем локации и персонажей
    //
    if (_importParameters.findCharactersAndLocations) {
        //
        // Персонажи
        //
        {
            const QSet<QString> characters = QSet<QString>::fromList(_scenario->findCharacters());

            //
            // Определить персонажи, которых нет в тексте
            //
            QSet<QString> charactersToDelete;
            foreach (DomainObject* domainObject,
                     DataStorageLayer::StorageFacade::researchStorage()->characters()->toList()) {
                Research* character = dynamic_cast<Research*>(domainObject);
                if (!characters.contains(character->name())) {
                    charactersToDelete.insert(character->name());
                }
            }

            //
            // Удалить тех, кого нет
            //
            DatabaseLayer::Database::transaction();
            foreach (const QString& character, charactersToDelete) {
                DataStorageLayer::StorageFacade::researchStorage()->removeCharacter(character);
            }
            DatabaseLayer::Database::commit();

            //
            // Добавить новых
            //
            DatabaseLayer::Database::transaction();
            foreach (const QString& character, characters) {
                if (!DataStorageLayer::StorageFacade::researchStorage()->hasCharacter(character)) {
                    DataStorageLayer::StorageFacade::researchStorage()->storeCharacter(character);
                }
            }
            DatabaseLayer::Database::commit();
        }

        //
        // Локации
        //
        {
            const QSet<QString> locations = QSet<QString>::fromList(_scenario->findLocations());

            //
            // Определить локации, которых нет в тексте
            //
            QSet<QString> locationsToDelete;
            foreach (DomainObject* domainObject,
                     DataStorageLayer::StorageFacade::researchStorage()->locations()->toList()) {
                Research* location = dynamic_cast<Research*>(domainObject);
                if (!locations.contains(location->name())) {
                    locationsToDelete.insert(location->name());
                }
            }

            //
            // Удалить те, которых нет
            //
            DatabaseLayer::Database::transaction();
            foreach (const QString& location, locationsToDelete) {
                DataStorageLayer::StorageFacade::researchStorage()->removeLocation(location);
            }
            DatabaseLayer::Database::commit();

            //
            // Добавить новых
            //
            DatabaseLayer::Database::transaction();
            foreach (const QString& location, locations) {
                if (!DataStorageLayer::StorageFacade::researchStorage()->hasLocation(location)) {
                    DataStorageLayer::StorageFacade::researchStorage()->storeLocation(location);
                }
            }
            DatabaseLayer::Database::commit();
        }
    }


    //
    // Загрузим данные разработки
    //
    const QVariantMap research = importer->importResearch(_importParameters);
    if (!research.isEmpty()) {
        //
        // Данные сценария
        //
        {
            const QVariantMap script = research["script"].toMap();
            DataStorageLayer::StorageFacade::scenarioDataStorage()->setName(script["name"].toString());
            DataStorageLayer::StorageFacade::scenarioDataStorage()->setLogline(script["logline"].toString());
            DataStorageLayer::StorageFacade::scenarioDataStorage()->setAdditionalInfo(script["additional_info"].toString());
            DataStorageLayer::StorageFacade::scenarioDataStorage()->setGenre(script["genre"].toString());
            DataStorageLayer::StorageFacade::scenarioDataStorage()->setAuthor(script["author"].toString());
            DataStorageLayer::StorageFacade::scenarioDataStorage()->setContacts(script["contacts"].toString());
            DataStorageLayer::StorageFacade::scenarioDataStorage()->setYear(script["year"].toString());
            DataStorageLayer::StorageFacade::scenarioDataStorage()->setSynopsis(script["synopsis"].toString());
        }

        //
        // Персонажи
        //
        {
            QLightBoxProgress::setProgressText(tr("Characters import"), QString());
            const QVariantList characters = research["characters"].toList();
            for (const QVariant& character : characters) {
                ::storeCharacter(character.toMap());
            }
        }

        //
        // Локации
        //
        {
            QLightBoxProgress::setProgressText(tr("Locations import"), QString());
            const QVariantList locations = research["locations"].toList();
            for (const QVariant& location : locations) {
                ::storeLocation(location.toMap());
            }
        }

        //
        // Документы
        //
        {
            QLightBoxProgress::setProgressText(tr("Documents import"), QString());
            const QVariantList documents = research["documents"].toList();
            for (const QVariant& document : documents) {
                ::storeResearchDocument(document.toMap(), nullptr);
            }
        }
    }

    return true;
}

void ImportManager::importScenario(BusinessLogic::ScenarioDocument* _scenario, const QString& _importFilePath)
{
    BusinessLogic::ImportParameters importParameters;
    importParameters.filePath = _importFilePath;
    importScenario(_scenario, 0, importParameters);
}

void ImportManager::importScenario(BusinessLogic::ScenarioDocument* _scenario, int _cursorPosition)
{
    if (m_importDialog->exec() == QLightBoxDialog::Accepted) {
        BusinessLogic::ImportParameters importParameters = m_importDialog->importParameters();

        //
        // Формат MS DOC не поддерживается, он отображается только для того, чтобы пользователи
        // не теряли свои файлы
        //
        if (importParameters.filePath.toLower().endsWith(kMsDocExtension)) {
            QLightBoxMessage::critical(m_importDialog, tr("File format not supported"),
                tr("Microsoft <b>DOC</b> files are not supported. You need save it to <b>DOCX</b> file and reimport."));
            return;
        }

        //
        // Если файла не существует, уведомим об этом
        //
        if (!QFile::exists(importParameters.filePath)) {
            QLightBoxMessage::critical(m_importDialog, tr("File doesn't exists"),
                tr("Please choose existing file and retry import."));
            return;
        }

        //
        // Покажем уведомление пользователю
        //
        QLightBoxProgress progress(m_importDialog->parentWidget());
        progress.showProgress(tr("Import"), tr("Please wait. Import can take few minutes."));

        //
        // Импортируем
        //
        const bool isImportSucceed = importScenario(_scenario, _cursorPosition, importParameters);

        //
        // Если импорт не удался, уведомим об этом пользователя
        //
        if (!isImportSucceed) {
            QLightBoxMessage::critical(m_importDialog->parentWidget(), tr("Import aborted"),
                tr("File to import is empty. Please check that you select correct file and retry import."));
        }

        //
        // Закроем уведомление
        //
        progress.finish();
    }
}

void ImportManager::initView()
{

}

void ImportManager::initConnections()
{

}

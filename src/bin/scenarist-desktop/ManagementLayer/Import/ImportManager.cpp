#include "ImportManager.h"

#include <Domain/Research.h>
#include <Domain/Scenario.h>

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/Import/KitScenaristImporter.h>
#include <BusinessLayer/Import/DocumentImporter.h>
#include <BusinessLayer/Import/FdxImporter.h>
#include <BusinessLayer/Import/TrelbyImporter.h>
#include <BusinessLayer/Import/FountainImporter.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/ResearchStorage.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <UserInterfaceLayer/Import/ImportDialog.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QApplication>
#include <QSet>

using ManagementLayer::ImportManager;
using UserInterface::ImportDialog;

namespace {
    /**
     * @brief Старый вордовский формат не поддерживается
     */
    const QString MS_DOC_EXTENSION = ".doc";

    /**
     * @brief Формат файлов КИТ Сценарист
     */
    const QString KIT_SCENARIST_EXTENSION = ".kitsp";

    /**
     * @brief Формат файлов Final Draft
     */
    const QString FINAL_DRAFT_EXTENSION = ".fdx";
    const QString FINAL_DRAFT_TEMPLATE_EXTENSION = ".fdxt";

    /**
     * @brief Формат файлов Trelby
     */
    const QString TRELBY_EXTENSION = ".trelby";

    /**
     * @brief Формат файлов Fountain
     */
    const QString FOUNTAIN_EXTENSION = ".fountain";
}


ImportManager::ImportManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_importDialog(new ImportDialog(_parentWidget))
{
    initView();
    initConnections();
}

void ImportManager::importScenario(BusinessLogic::ScenarioDocument* _scenario, int _cursorPosition,
    const BusinessLogic::ImportParameters& _importParameters)
{
    //
    // Получим xml-представление импортируемого сценария
    //
    QScopedPointer<BusinessLogic::AbstractImporter> importer;
    if (_importParameters.filePath.toLower().endsWith(KIT_SCENARIST_EXTENSION)) {
        importer.reset(new BusinessLogic::KitScenaristImporter);
    } else if (_importParameters.filePath.toLower().endsWith(FINAL_DRAFT_EXTENSION)
               || _importParameters.filePath.toLower().endsWith(FINAL_DRAFT_TEMPLATE_EXTENSION)) {
        importer.reset(new BusinessLogic::FdxImporter);
    } else if (_importParameters.filePath.toLower().endsWith(TRELBY_EXTENSION)) {
        importer.reset(new BusinessLogic::TrelbyImporter);
    } else if (_importParameters.filePath.toLower().endsWith(FOUNTAIN_EXTENSION)) {
        importer.reset(new BusinessLogic::FountainImporter);
    } else {
        importer.reset(new BusinessLogic::DocumentImporter);
    }
    const QString importScenarioXml = importer->importScenario(_importParameters);

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
            QSet<QString> characters = QSet<QString>::fromList(_scenario->findCharacters());

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
            QSet<QString> locations = QSet<QString>::fromList(_scenario->findLocations());

            //
            // Определить локации, которых нет в тексте
            //
            QSet<QString> locationsToDelete;
            foreach (DomainObject* domainObject,
                     DataStorageLayer::StorageFacade::researchStorage()->characters()->toList()) {
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
        // TODO:
        //
    }
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
        if (!importParameters.filePath.toLower().endsWith(MS_DOC_EXTENSION)) {
            //
            // Покажем уведомление пользователю
            //
            QLightBoxProgress progress(m_importDialog->parentWidget());
            progress.showProgress(tr("Import"), tr("Please wait. Import can take few minutes."));

            //
            // Импортируем
            //
            importScenario(_scenario, _cursorPosition, importParameters);

            //
            // Закроем уведомление
            //
            progress.finish();
        }
        //
        // Формат MS DOC не поддерживается, он отображается только для того, чтобы пользователи
        // не теряли свои файлы
        //
        else {
            QLightBoxMessage::information(m_importDialog, tr("File format not supported"),
                tr("Microsoft <b>DOC</b> files are not supported. You need save it to <b>DOCX</b> file and reimport."));
        }
    }
}

void ImportManager::initView()
{

}

void ImportManager::initConnections()
{

}

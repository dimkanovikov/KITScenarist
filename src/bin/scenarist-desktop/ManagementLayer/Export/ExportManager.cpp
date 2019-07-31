#include "ExportManager.h"

#include <ManagementLayer/Project/ProjectsManager.h>

#include <BusinessLayer/Research/ResearchModel.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/Export/DocxExporter.h>
#include <BusinessLayer/Export/PdfExporter.h>
#include <BusinessLayer/Export/FdxExporter.h>
#include <BusinessLayer/Export/FountainExporter.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/ResearchStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <Domain/Research.h>
#include <Domain/Scenario.h>
#include <Domain/ScenarioData.h>

#include <UserInterfaceLayer/Export/ExportDialog.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QTimer>

using ManagementLayer::ExportManager;
using ManagementLayer::ExportType;
using ManagementLayer::ProjectsManager;
using DataStorageLayer::StorageFacade;
using UserInterface::ExportDialog;

namespace {
    /**
     * @brief Истинное значение для умолчальных значений, загружаемых из настроек
     */
    const QString TRUE_VALUE = "1";
    const QString FALSE_VALUE = "0";
}


ExportManager::ExportManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_currentScenario(0),
    m_exportDialog(new ExportDialog(_parentWidget)),
    m_researchModelProxy(new BusinessLogic::ResearchModelCheckableProxy(this))
{
    initView();
    initConnections();
}

void ExportManager::setResearchModel(QAbstractItemModel* _model)
{
    m_researchModelProxy->setSourceModel(_model);
}

void ExportManager::exportScenario(BusinessLogic::ScenarioDocument* _scenario,
    const QMap<QString, QString>& _scenarioData)
{
    m_currentScenario = _scenario;
    m_scenarioData = _scenarioData;
    initExportDialog();

    if (m_exportDialog->exec() == QLightBoxDialog::Accepted) {
        //
        // Покажем уведомление пользователю
        //
        QLightBoxProgress progress(m_exportDialog->parentWidget());
        progress.showProgress(tr("Export"), tr("Please wait. Export can take few minutes."));

        //
        // Настроим параметры экспорта
        //
        BusinessLogic::ExportParameters exportParameters = m_exportDialog->exportParameters();
        exportParameters.scriptName = _scenarioData.value(ScenarioData::NAME_KEY);
        exportParameters.scriptHeader = _scenarioData.value(ScenarioData::HEADER_KEY);
        exportParameters.scriptFooter = _scenarioData.value(ScenarioData::FOOTER_KEY);
        exportParameters.scenesPrefix = _scenarioData.value(ScenarioData::SCENE_NUMBERS_PREFIX_KEY);
        exportParameters.scriptAdditionalInfo = _scenarioData.value(ScenarioData::ADDITIONAL_INFO_KEY);
        exportParameters.scriptGenre = _scenarioData.value(ScenarioData::GENRE_KEY);
        exportParameters.scriptAuthor = _scenarioData.value(ScenarioData::AUTHOR_KEY);
        exportParameters.scriptContacts = _scenarioData.value(ScenarioData::CONTACTS_KEY);
        exportParameters.scriptYear = _scenarioData.value(ScenarioData::YEAR_KEY);
        exportParameters.logline = _scenarioData.value(ScenarioData::LOGLINE_KEY);
        exportParameters.synopsis = _scenarioData.value(ScenarioData::SYNOPSIS_KEY);

        const QString filePath = exportParameters.filePath;
        if (!filePath.isEmpty()) {
            const QFileInfo fileInfo(filePath);

            //
            // Проверяем возможность записи в файл
            //
            QFile file(filePath);
            const bool canWrite = file.open(QIODevice::WriteOnly);
            file.close();

            //
            // Если возможна запись в файл
            //
            if (canWrite) {
                //
                // Определим экспортирующего
                //
                QSharedPointer<BusinessLogic::AbstractExporter> exporter;
                if (m_exportDialog->exportFormat() == "docx") {
                    exporter.reset(new BusinessLogic::DocxExporter);
                } else if (m_exportDialog->exportFormat() == "pdf") {
                    exporter.reset(new BusinessLogic::PdfExporter);
                } else if (m_exportDialog->exportFormat() == "fdx") {
                    exporter.reset(new BusinessLogic::FdxExporter);
                } else {
                    exporter.reset(new BusinessLogic::FountainExporter);
                }

                //
                // Экспортируем документ
                //
                if (exportParameters.isResearch) {
                    exporter->exportTo(m_researchModelProxy, exportParameters);
                } else {
                    exporter->exportTo(_scenario, exportParameters);
                }
            }
            //
            // Если невозможно записать в файл
            //
            else {
                //
                // ... предупреждаем
                //
                QString errorMessage;
                if (!fileInfo.dir().exists()) {
                    errorMessage =
                        tr("You try export to nonexistent folder <b>%1</b>. Please, choose other location for exported file.")
                        .arg(fileInfo.dir().absolutePath());
                } else if (fileInfo.exists()) {
                    errorMessage =
                        tr("Can't write to file. Maybe it is opened by another application. Please close it and retry export.");
                } else {
                    errorMessage =
                        tr("Can't write to file. Check permissions to write in choosed folder. Please, choose other folder.");
                }
                QLightBoxMessage::critical(&progress, tr("Export error"), errorMessage);
                //
                // ... и перезапускаем экспорт
                //
                QTimer::singleShot(0, [=] { exportScenario(_scenario, _scenarioData); });
            }
        }

        //
        // Закроем уведомление
        //
        progress.finish();
    }

    m_currentScenario = 0;
    m_scenarioData.clear();
}

void ExportManager::printPreview(BusinessLogic::ScenarioDocument* _scenario,
    const QMap<QString, QString>& _scenarioData, ManagementLayer::ExportType _type)
{
    initExportDialog();

    //
    // Покажем уведомление пользователю
    //
    QLightBoxProgress progress(m_exportDialog->parentWidget());
    progress.showProgress(tr("Print Preview"), tr("Please wait. Preparing document to preview can take few minutes."));

    //
    // Настроим параметры экспорта
    //
    BusinessLogic::ExportParameters exportParameters = m_exportDialog->exportParameters();
    exportParameters.scriptName = _scenarioData.value(ScenarioData::NAME_KEY);
    exportParameters.scriptHeader = _scenarioData.value(ScenarioData::HEADER_KEY);
    exportParameters.scriptFooter = _scenarioData.value(ScenarioData::FOOTER_KEY);
    exportParameters.scenesPrefix = _scenarioData.value(ScenarioData::SCENE_NUMBERS_PREFIX_KEY);
    exportParameters.scriptAdditionalInfo = _scenarioData.value(ScenarioData::ADDITIONAL_INFO_KEY);
    exportParameters.scriptGenre = _scenarioData.value(ScenarioData::GENRE_KEY);
    exportParameters.scriptAuthor = _scenarioData.value(ScenarioData::AUTHOR_KEY);
    exportParameters.scriptContacts = _scenarioData.value(ScenarioData::CONTACTS_KEY);
    exportParameters.scriptYear = _scenarioData.value(ScenarioData::YEAR_KEY);
    exportParameters.logline = _scenarioData.value(ScenarioData::LOGLINE_KEY);
    exportParameters.synopsis = _scenarioData.value(ScenarioData::SYNOPSIS_KEY);

    //
    // Формируем предварительный просмот
    //
    BusinessLogic::PdfExporter exporter;
    const bool isNeedExportResearch
            = _type == ExportType::Research
              || (_type == ExportType::Auto && exportParameters.isResearch);
    if (isNeedExportResearch) {
        exporter.printPreview(m_researchModelProxy, exportParameters);
    } else {
        //
        // Принудительно задаём флаг печати сценария, т.к. он может быть не установлен
        // если диалог экспорта не был показан, или там стоит другой тип экспортируемого
        // документа, но мы попали сюда из меню
        //
        if (exportParameters.isOutline == false
            && exportParameters.isScript == false) {
            exportParameters.isScript = true;
        }
        exporter.printPreview(_scenario, exportParameters);
    }

    //
    // Закроем уведомление
    //
    progress.finish();
}

void ExportManager::loadCurrentProjectSettings(const QString& _projectPath)
{
    //
    // Очистим галочки модели разработки
    //
    m_researchModelProxy->clearCheckStates();

    //
    // Загрузим параметры экспорта
    //
    const QString projectKey = QString("projects/%1/export").arg(_projectPath);
    m_exportDialog->setExportType(
                StorageFacade::settingsStorage()->value(
                    QString("%1/export-type").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings).toInt()
                );
    m_exportDialog->setResearchExportFilePath(
                StorageFacade::settingsStorage()->value(
                    QString("%1/research-file-path").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                );
    m_exportDialog->setScriptExportFilePath(
                StorageFacade::settingsStorage()->value(
                    QString("%1/file-path").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                );

    m_exportDialog->setCheckPageBreaks(
                StorageFacade::settingsStorage()->value(
                    QString("%1/check-page-breaks").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings, "1").toInt()
                );
    QString exportStyle = StorageFacade::settingsStorage()->value(
                              QString("%1/style").arg(projectKey),
                              DataStorageLayer::SettingsStorage::ApplicationSettings);
    if (exportStyle.isEmpty()) {
        exportStyle = DataStorageLayer::StorageFacade::settingsStorage()->value(
                          "scenario-editor/current-style",
                          DataStorageLayer::SettingsStorage::ApplicationSettings);
    }
    m_exportDialog->setCurrentStyle(exportStyle);
    m_exportDialog->setPageNumbering(
                StorageFacade::settingsStorage()->value(
                    QString("%1/page-numbering").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings,
                    TRUE_VALUE).toInt()
                );
    m_exportDialog->setScenesNumbering(
                StorageFacade::settingsStorage()->value(
                    QString("%1/scenes-numbering").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings,
                    TRUE_VALUE).toInt()
                );
    m_exportDialog->setDialoguesNumbering(
                StorageFacade::settingsStorage()->value(
                    QString("%1/dialogues-numbering").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings,
                    FALSE_VALUE).toInt()
                );
    m_exportDialog->setSaveReviewMarks(
                StorageFacade::settingsStorage()->value(
                    QString("%1/save-review-marks").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings,
                    TRUE_VALUE).toInt()
                );
    m_exportDialog->setPrintTitle(
                StorageFacade::settingsStorage()->value(
                    QString("%1/print-title").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings,
                    TRUE_VALUE).toInt()
                );
    m_exportDialog->setPrintWatermark(
                StorageFacade::settingsStorage()->value(
                    QString("%1/print-watermark").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings,
                    FALSE_VALUE).toInt()
                );
    m_exportDialog->setWatermark(
                StorageFacade::settingsStorage()->value(
                    QString("%1/watermark").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                );
}

void ExportManager::saveCurrentProjectSettings(const QString& _projectPath)
{
    const QString projectKey = QString("projects/%1/export").arg(_projectPath);

    //
    // Сохраним параметры экспорта
    //
    StorageFacade::settingsStorage()->setValue(
                QString("%1/export-type").arg(projectKey),
                QString::number(m_exportDialog->exportType()),
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/research-file-path").arg(projectKey),
                m_exportDialog->researchFilePath(),
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/file-path").arg(projectKey),
                m_exportDialog->scriptFilePath(),
                DataStorageLayer::SettingsStorage::ApplicationSettings);

    BusinessLogic::ExportParameters exportParameters = m_exportDialog->exportParameters();
    StorageFacade::settingsStorage()->setValue(
                QString("%1/check-page-breaks").arg(projectKey),
                exportParameters.checkPageBreaks ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/style").arg(projectKey),
                exportParameters.style,
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/page-numbering").arg(projectKey),
                exportParameters.printPagesNumbers ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/scenes-numbering").arg(projectKey),
                exportParameters.printScenesNumbers ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/dialogues-numbering").arg(projectKey),
                exportParameters.printDialoguesNumbers ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/save-review-marks").arg(projectKey),
                exportParameters.saveReviewMarks ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/print-title").arg(projectKey),
                exportParameters.printTilte ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/print-watermark").arg(projectKey),
                exportParameters.printWatermark ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/watermark").arg(projectKey),
                exportParameters.watermark,
                DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void ExportManager::aboutExportStyleChanged(const QString& _styleName)
{
    StorageFacade::settingsStorage()->setValue("export/style", _styleName,
                                               DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void ExportManager::initView()
{
    //
    // Загрузить библиотеку стилей
    //
    m_exportDialog->setStylesModel(BusinessLogic::ScenarioTemplateFacade::templatesList());

    //
    // Загрузить настройки
    //
    QString exportTemplate = StorageFacade::settingsStorage()->value(
                                 "export/style",
                                 DataStorageLayer::SettingsStorage::ApplicationSettings);
    if (exportTemplate.isEmpty()) {
        exportTemplate = StorageFacade::settingsStorage()->value(
                             "scenario-editor/current-style",
                             DataStorageLayer::SettingsStorage::ApplicationSettings);
    }
    m_exportDialog->setCurrentStyle(exportTemplate);
}

void ExportManager::initConnections()
{
    connect(m_exportDialog, SIGNAL(currentStyleChanged(QString)),
            this, SLOT(aboutExportStyleChanged(QString)));
    connect(m_exportDialog, &ExportDialog::printPreviewPressed, this, [this] {
        //
        // 1. скрываем диалог настроек
        // 2. показываем предпросмотр
        // 3. а потом вновь показываем диалог
        //
        m_exportDialog->hide();
        printPreview(m_currentScenario, m_scenarioData);
        m_exportDialog->show();
    });
}

void ExportManager::initExportDialog()
{
    //
    // Установка имени файла
    //
    QString exportFileName = StorageFacade::scenarioDataStorage()->name();
    if (exportFileName.isEmpty()) {
        const auto& currentProject = ProjectsManager::currentProject();
        //
        // Для удаленных проектов используем имя проекта + id проекта
        //
        if (currentProject.isRemote()) {
            exportFileName = QString("%1 [%2]").arg(currentProject.name()).arg(currentProject.id());
        }
        //
        // а для локальных имя файла
        //
        else {
            QFileInfo fileInfo(currentProject.path());
            exportFileName = fileInfo.completeBaseName();
        }
    }
    m_exportDialog->setResearchExportFileName(exportFileName);
    m_exportDialog->setScriptExportFileName(exportFileName);

    //
    // Загрузим и установим модель документов разработки
    //
    m_exportDialog->setResearchModel(m_researchModelProxy);
}

#include "ExportManager.h"

#include <ManagementLayer/Project/ProjectsManager.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/Export/DocxExporter.h>
#include <BusinessLayer/Export/PdfExporter.h>
#include <BusinessLayer/Export/FdxExporter.h>
#include <BusinessLayer/Export/FountainExporter.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

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
using ManagementLayer::ProjectsManager;
using DataStorageLayer::StorageFacade;
using UserInterface::ExportDialog;

namespace {
    /**
     * @brief Истинное значение для умолчальных значений, загружаемых из настроек
     */
    const QString TRUE_VALUE = "1";
}


ExportManager::ExportManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_currentScenario(0),
    m_exportDialog(new ExportDialog(_parentWidget))
{
    initView();
    initConnections();
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
        exportParameters.scenarioName = _scenarioData.value(ScenarioData::NAME_KEY);
        exportParameters.scenarioAdditionalInfo = _scenarioData.value(ScenarioData::ADDITIONAL_INFO_KEY);
        exportParameters.scenarioGenre = _scenarioData.value(ScenarioData::GENRE_KEY);
        exportParameters.scenarioAuthor = _scenarioData.value(ScenarioData::AUTHOR_KEY);
        exportParameters.scenarioContacts = _scenarioData.value(ScenarioData::CONTACTS_KEY);
        exportParameters.scenarioYear = _scenarioData.value(ScenarioData::YEAR_KEY);

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
                BusinessLogic::AbstractExporter* exporter = 0;
                if (m_exportDialog->exportFormat() == "docx") {
                    exporter = new BusinessLogic::DocxExporter;
                } else if (m_exportDialog->exportFormat() == "pdf") {
                    exporter = new BusinessLogic::PdfExporter;
                } else if (m_exportDialog->exportFormat() == "fdx") {
                    exporter = new BusinessLogic::FdxExporter;
                } else {
                    exporter = new BusinessLogic::FountainExporter;
                }

                //
                // Экспортируем документ
                //
                exporter->exportTo(_scenario, exportParameters);
                delete exporter;
                exporter = nullptr;
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
                        tr("Can't write to file. Maybe it opened in other application. Please, close it and retry export.");
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

void ExportManager::printPreviewScenario(BusinessLogic::ScenarioDocument* _scenario,
    const QMap<QString, QString>& _scenarioData)
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
    exportParameters.scenarioName = _scenarioData.value(ScenarioData::NAME_KEY);
    exportParameters.scenarioAdditionalInfo = _scenarioData.value(ScenarioData::ADDITIONAL_INFO_KEY);
    exportParameters.scenarioGenre = _scenarioData.value(ScenarioData::GENRE_KEY);
    exportParameters.scenarioAuthor = _scenarioData.value(ScenarioData::AUTHOR_KEY);
    exportParameters.scenarioContacts = _scenarioData.value(ScenarioData::CONTACTS_KEY);
    exportParameters.scenarioYear = _scenarioData.value(ScenarioData::YEAR_KEY);

    //
    // Формируем предварительный просмот
    //
    BusinessLogic::PdfExporter exporter;
    exporter.printPreview(_scenario, exportParameters);

    //
    // Закроем уведомление
    //
    progress.finish();
}

void ExportManager::loadCurrentProjectSettings(const QString& _projectPath)
{
    const QString projectKey = QString("projects/%1/export").arg(_projectPath);

    //
    // Загрузим параметры экспорта
    //
    m_exportDialog->setExportFilePath(
                StorageFacade::settingsStorage()->value(
                    QString("%1/file-path").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                );
    m_exportDialog->setCheckPageBreaks(
                StorageFacade::settingsStorage()->value(
                    QString("%1/check-page-breaks").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings).toInt()
                );
    m_exportDialog->setCurrentStyle(
                StorageFacade::settingsStorage()->value(
                    QString("%1/style").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                );
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
    m_exportDialog->setScenesPrefix(
                StorageFacade::settingsStorage()->value(
                    QString("%1/scenes-prefix").arg(projectKey),
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
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
}

void ExportManager::saveCurrentProjectSettings(const QString& _projectPath)
{
    const QString projectKey = QString("projects/%1/export").arg(_projectPath);

    //
    // Сохраним параметры экспорта
    //
    BusinessLogic::ExportParameters exportParameters = m_exportDialog->exportParameters();
    StorageFacade::settingsStorage()->setValue(
                QString("%1/file-path").arg(projectKey),
                exportParameters.filePath,
                DataStorageLayer::SettingsStorage::ApplicationSettings);
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
                QString("%1/scenes-prefix").arg(projectKey),
                exportParameters.scenesPrefix,
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/save-review-marks").arg(projectKey),
                exportParameters.saveReviewMarks ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                QString("%1/print-title").arg(projectKey),
                exportParameters.printTilte ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void ExportManager::aboutExportStyleChanged(const QString& _styleName)
{
    StorageFacade::settingsStorage()->setValue("export/style", _styleName,
                                                                 DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void ExportManager::aboutPrintPreview()
{
    //
    // Скрываем окно настроек, показываем предпросмотр, а потом вновь показываем его
    //
    m_exportDialog->hide();
    printPreviewScenario(m_currentScenario, m_scenarioData);
    m_exportDialog->show();
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
    connect(m_exportDialog, SIGNAL(printPreview()), this, SLOT(aboutPrintPreview()));
}

void ExportManager::initExportDialog()
{
    //
    // Установка имени файла
    //
    QString exportFileName = StorageFacade::scenarioDataStorage()->name();
    if (exportFileName.isEmpty()) {
        QFileInfo fileInfo(ProjectsManager::currentProject().path());
        exportFileName = fileInfo.completeBaseName();
    }
    m_exportDialog->setExportFileName(exportFileName);
}

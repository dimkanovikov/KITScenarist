#include "ExportManager.h"

#include <ManagementLayer/Project/ProjectsManager.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/Export/DocxExporter.h>
#include <BusinessLayer/Export/PdfExporter.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <Domain/Scenario.h>
#include <Domain/ScenarioData.h>

#include <UserInterfaceLayer/Export/ExportDialog.h>

#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardItemModel>

using ManagementLayer::ExportManager;
using ManagementLayer::ProjectsManager;
using DataStorageLayer::StorageFacade;
using UserInterface::ExportDialog;

namespace {
	/**
	 * @brief По умолчанию необходимо экспортировать редакторские пометки
	 */
	const QString SAVE_REVIEW_MARKS_BY_DEFAULT = "1";
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
		ProgressWidget progress(m_exportDialog->parentWidget());
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
			// Проверяем существование папки, в которую пользователь экспортирует
			//
			if (fileInfo.dir().exists()) {
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
					if (fileInfo.suffix() == "docx") {
						exporter = new BusinessLogic::DocxExporter;
					} else if (fileInfo.suffix() == "pdf") {
						exporter = new BusinessLogic::PdfExporter;
					} else {
						Q_ASSERT_X(0, Q_FUNC_INFO, qPrintable("Unknown file extension: " + fileInfo.suffix()));
					}

					//
					// Экспортируем документ
					//
					exporter->exportTo(_scenario, exportParameters);
					delete exporter;
					exporter = 0;
				}
				//
				// Если невозможно записать в файл, предупреждаем пользователя и отваливаемся
				//
				else {
					QLightBoxMessage::critical(&progress, tr("Export error"),
						tr("Can't write to file. Maybe it opened in other application. Please, close it and restart export."));
				}
			}
			//
			// Если папки не существует, уведомляем и отваливаемся
			//
			else {
				QLightBoxMessage::critical(&progress, tr("Export error"),
					tr("You try export to nonexistent folder <b>%1</b>. Please, choose other location for exported file.")
					.arg(fileInfo.dir().absolutePath()));
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
	ProgressWidget progress(m_exportDialog->parentWidget());
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
					DataStorageLayer::SettingsStorage::ApplicationSettings).toInt()
				);
	m_exportDialog->setScenesNumbering(
				StorageFacade::settingsStorage()->value(
					QString("%1/scenes-numbering").arg(projectKey),
					DataStorageLayer::SettingsStorage::ApplicationSettings).toInt()
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
					SAVE_REVIEW_MARKS_BY_DEFAULT).toInt()
				);
	m_exportDialog->setPrintTitle(
				StorageFacade::settingsStorage()->value(
					QString("%1/print-title").arg(projectKey),
					DataStorageLayer::SettingsStorage::ApplicationSettings).toInt()
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
	m_exportDialog->setCurrentStyle(
				StorageFacade::settingsStorage()->value(
					"export/style",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);
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

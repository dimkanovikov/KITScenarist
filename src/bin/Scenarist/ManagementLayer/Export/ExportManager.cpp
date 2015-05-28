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

#include <UserInterfaceLayer/Export/ExportDialog.h>

#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardItemModel>

using ManagementLayer::ExportManager;
using ManagementLayer::ProjectsManager;
using DataStorageLayer::StorageFacade;
using UserInterface::ExportDialog;


ExportManager::ExportManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_currentScenario(0),
	m_exportDialog(new ExportDialog(_parentWidget))
{
	initView();
	initConnections();
}

void ExportManager::exportScenario(BusinessLogic::ScenarioDocument* _scenario)
{
	m_currentScenario = _scenario;
	initExportDialog();

	if (m_exportDialog->exec() == QLightBoxDialog::Accepted) {
		//
		// Покажем уведомление пользователю
		//
		ProgressWidget progress(m_exportDialog->parentWidget());
		progress.showProgress(tr("Export"), tr("Please wait. Export can take few minutes."));

		BusinessLogic::ExportParameters exportParameters = m_exportDialog->exportParameters();
		const QString filePath = exportParameters.filePath;

		if (!filePath.isEmpty()) {
			const QFileInfo fileInfo(filePath);

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
		// Закроем уведомление
		//
		progress.finish();
	}

	//
	// Сохраняем информацию о титульном листе
	//
	saveTitleListInfo(m_currentScenario);

	m_currentScenario = 0;
}

void ExportManager::printPreviewScenario(BusinessLogic::ScenarioDocument* _scenario)
{
	initExportDialog();

	//
	// Покажем уведомление пользователю
	//
	ProgressWidget progress(m_exportDialog->parentWidget());
	progress.showProgress(tr("Print Preview"), tr("Please wait. Preparing document to preview can take few minutes."));

	BusinessLogic::PdfExporter exporter;
	exporter.printPreview(_scenario, m_exportDialog->exportParameters());

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
	// Сохраняем информацию о титульном листе
	//
	saveTitleListInfo(m_currentScenario);

	//
	// Скрываем окно настроек, показываем предпросмотр, а потом вновь показываем его
	//
	m_exportDialog->hide();
	printPreviewScenario(m_currentScenario);
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
		exportFileName = fileInfo.baseName();
	}
	m_exportDialog->setExportFileName(exportFileName);

	//
	// Установка информации о титульном листе
	//
	m_exportDialog->setScenarioName(StorageFacade::scenarioDataStorage()->name());
	m_exportDialog->setScenarioAdditionalInfo(StorageFacade::scenarioDataStorage()->additionalInfo());
	m_exportDialog->setScenarioGenre(StorageFacade::scenarioDataStorage()->genre());
	m_exportDialog->setScenarioAuthor(StorageFacade::scenarioDataStorage()->author());
	m_exportDialog->setScenarioContacts(StorageFacade::scenarioDataStorage()->contacts());
	m_exportDialog->setScenarioYear(StorageFacade::scenarioDataStorage()->year());
}

void ExportManager::saveTitleListInfo(BusinessLogic::ScenarioDocument* _scenario)
{
	//
	// Сохраняем информацию о титульном листе
	//
	Domain::Scenario* currentScenario = _scenario->scenario();
	bool isTitleListDataChanged = false;
	if (StorageFacade::scenarioDataStorage()->name() != m_exportDialog->scenarioName()) {
		StorageFacade::scenarioDataStorage()->setName(m_exportDialog->scenarioName());
		emit scenarioNameChanged(StorageFacade::scenarioDataStorage()->name());
		isTitleListDataChanged = true;
	}
	if (StorageFacade::scenarioDataStorage()->additionalInfo() != m_exportDialog->scenarioAdditionalInfo()) {
		StorageFacade::scenarioDataStorage()->setAdditionalInfo(m_exportDialog->scenarioAdditionalInfo());
		isTitleListDataChanged = true;
	}
	if (StorageFacade::scenarioDataStorage()->genre() != m_exportDialog->scenarioGenre()) {
		StorageFacade::scenarioDataStorage()->setGenre(m_exportDialog->scenarioGenre());
		isTitleListDataChanged = true;
	}
	if (StorageFacade::scenarioDataStorage()->author() != m_exportDialog->scenarioAuthor()) {
		StorageFacade::scenarioDataStorage()->setAuthor(m_exportDialog->scenarioAuthor());
		isTitleListDataChanged = true;
	}
	if (StorageFacade::scenarioDataStorage()->contacts() != m_exportDialog->scenarioContacts()) {
		StorageFacade::scenarioDataStorage()->setContacts(m_exportDialog->scenarioContacts());
		isTitleListDataChanged = true;
	}
	if (StorageFacade::scenarioDataStorage()->year() != m_exportDialog->scenarioYear()) {
		StorageFacade::scenarioDataStorage()->setYear(m_exportDialog->scenarioYear());
		isTitleListDataChanged = true;
	}
	//
	// ... если есть изменения
	//
	if (isTitleListDataChanged) {
		emit scenarioTitleListDataChanged();
	}
}

#include "ExportManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>
#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/Export/DocxExporter.h>
#include <BusinessLayer/Export/RtfExporter.h>
#include <BusinessLayer/Export/PdfExporter.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <Domain/Scenario.h>

#include <UserInterfaceLayer/Export/ExportDialog.h>

#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardItemModel>

using ManagementLayer::ExportManager;
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
	initExportDialog(m_currentScenario);

	if (m_exportDialog->exec() == QDialog::Accepted) {
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
			} else if (fileInfo.suffix() == "rtf") {
				exporter = new BusinessLogic::RtfExporter;
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
		progress.close();
	}

	//
	// Сохраняем информацию о титульном листе
	//
	Domain::Scenario* currentScenario = m_currentScenario->scenario();
	bool isTitleListDataChanged = false;
	if (currentScenario->name() != m_exportDialog->scenarioName()) {
		currentScenario->setName(m_exportDialog->scenarioName());
		emit scenarioNameChanged(currentScenario->name());
		isTitleListDataChanged = true;
	}
	if (currentScenario->additionalInfo() != m_exportDialog->scenarioAdditionalInfo()) {
		currentScenario->setAdditionalInfo(m_exportDialog->scenarioAdditionalInfo());
		isTitleListDataChanged = true;
	}
	if (currentScenario->genre() != m_exportDialog->scenarioGenre()) {
		currentScenario->setGenre(m_exportDialog->scenarioGenre());
		isTitleListDataChanged = true;
	}
	if (currentScenario->author() != m_exportDialog->scenarioAuthor()) {
		currentScenario->setAuthor(m_exportDialog->scenarioAuthor());
		isTitleListDataChanged = true;
	}
	if (currentScenario->contacts() != m_exportDialog->scenarioContacts()) {
		currentScenario->setContacts(m_exportDialog->scenarioContacts());
		isTitleListDataChanged = true;
	}
	if (currentScenario->year() != m_exportDialog->scenarioYear()) {
		currentScenario->setYear(m_exportDialog->scenarioYear());
		isTitleListDataChanged = true;
	}
	//
	// ... если есть изменения
	//
	if (isTitleListDataChanged) {
		emit scenarioTitleListDataChanged();
	}

	m_currentScenario = 0;
}

void ExportManager::printPreviewScenario(BusinessLogic::ScenarioDocument* _scenario)
{
	initExportDialog(_scenario);

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
	progress.close();
}

void ExportManager::loadCurrentProjectSettings(const QString& _projectPath)
{
	const QString projectKey = QString("projects/%1/export").arg(_projectPath);

	//
	// Загрузим параметры экспорта
	//
	m_exportDialog->setExportFilePath(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("%1/file-path").arg(projectKey),
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);
	m_exportDialog->setCheckPageBreaks(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("%1/check-page-breaks").arg(projectKey),
					DataStorageLayer::SettingsStorage::ApplicationSettings).toInt()
				);
	m_exportDialog->setCurrentStyle(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("%1/style").arg(projectKey),
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);
	m_exportDialog->setPageNumbering(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("%1/page-numbering").arg(projectKey),
					DataStorageLayer::SettingsStorage::ApplicationSettings).toInt()
				);
	m_exportDialog->setScenesNumbering(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("%1/scenes-numbering").arg(projectKey),
					DataStorageLayer::SettingsStorage::ApplicationSettings).toInt()
				);
	m_exportDialog->setScenesPrefix(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("%1/scenes-prefix").arg(projectKey),
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);
	m_exportDialog->setPrintTitle(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
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
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				QString("%1/file-path").arg(projectKey),
				exportParameters.filePath,
				DataStorageLayer::SettingsStorage::ApplicationSettings);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				QString("%1/check-page-breaks").arg(projectKey),
				exportParameters.checkPageBreaks ? "1" : "0",
				DataStorageLayer::SettingsStorage::ApplicationSettings);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				QString("%1/style").arg(projectKey),
				exportParameters.style,
				DataStorageLayer::SettingsStorage::ApplicationSettings);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				QString("%1/page-numbering").arg(projectKey),
				exportParameters.printPagesNumbers ? "1" : "0",
				DataStorageLayer::SettingsStorage::ApplicationSettings);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				QString("%1/scenes-numbering").arg(projectKey),
				exportParameters.printScenesNumbers ? "1" : "0",
				DataStorageLayer::SettingsStorage::ApplicationSettings);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				QString("%1/scenes-prefix").arg(projectKey),
				exportParameters.scenesPrefix,
				DataStorageLayer::SettingsStorage::ApplicationSettings);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				QString("%1/print-title").arg(projectKey),
				exportParameters.printTilte ? "1" : "0",
				DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void ExportManager::aboutExportStyleChanged(const QString& _styleName)
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue("export/style", _styleName,
																 DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void ExportManager::aboutPrintPreview()
{
	//
	// Закрывать диалоговое окно нельзя, поэтому прячем его за границами экрана,
	// а потом возвращаем на место
	//
    const QPoint POSITION_OUT_OF_SCREEN(10000, 10000);
	QPoint lastPos = m_exportDialog->pos();
	m_exportDialog->move(POSITION_OUT_OF_SCREEN);
	printPreviewScenario(m_currentScenario);
	m_exportDialog->move(lastPos);
}

void ExportManager::initView()
{
	//
	// Загрузить библиотеку стилей
	//
	m_exportDialog->setStylesModel(BusinessLogic::ScenarioStyleFacade::stylesList());

	//
	// Загрузить настройки
	//
	m_exportDialog->setCurrentStyle(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
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

void ExportManager::initExportDialog(BusinessLogic::ScenarioDocument* _scenario)
{
	Domain::Scenario* currentScenario = _scenario->scenario();

	//
	// Установка имени файла
	//
	QString exportFileName = currentScenario->name();
	if (exportFileName.isEmpty()) {
		QFileInfo fileInfo(DatabaseLayer::Database::currentFile());
		exportFileName = fileInfo.baseName();
	}
	m_exportDialog->setExportFileName(exportFileName);

	//
	// Установка информации о титульном листе
	//
	m_exportDialog->setScenarioName(currentScenario->name());
	m_exportDialog->setScenarioAdditionalInfo(currentScenario->additionalInfo());
	m_exportDialog->setScenarioGenre(currentScenario->genre());
	m_exportDialog->setScenarioAuthor(currentScenario->author());
	m_exportDialog->setScenarioContacts(currentScenario->contacts());
	m_exportDialog->setScenarioYear(currentScenario->year());
}

#include "ExportManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>
#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/Export/PdfExporter.h>
#include <BusinessLayer/Export/RtfExporter.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <Domain/Scenario.h>

#include <UserInterfaceLayer/Export/ExportDialog.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardItemModel>

using ManagementLayer::ExportManager;
using UserInterface::ExportDialog;


ExportManager::ExportManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_exportDialog(new ExportDialog(_parentWidget))
{
	initView();
	initConnections();
}

void ExportManager::exportScenario(BusinessLogic::ScenarioDocument* _scenario)
{
	Domain::Scenario* currentScenario = _scenario->scenario();
	//
	// Загрузить информацию о текущем сценарии в диалог
	//

	//
	// TODO: Загрузка настроек сохранённых для конкретного проекта
	//

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


	if (m_exportDialog->exec() == QDialog::Accepted) {
		BusinessLogic::ExportParameters exportParameters = m_exportDialog->exportParameters();
		const QString filePath = exportParameters.filePath;

		if (!filePath.isEmpty()) {
			const QFileInfo fileInfo(filePath);

			//
			// Определим экспортирующего
			//
			BusinessLogic::AbstractExporter* exporter = 0;
			if (fileInfo.suffix() == "rtf") {
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
	}

	//
	// Сохраняем информацию о титульном листе
	//
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
}

void ExportManager::printPreviewScenario(BusinessLogic::ScenarioDocument* _scenario)
{
	BusinessLogic::ExportParameters exportParameters = m_exportDialog->exportParameters();
	//
	// Очищаем путь к файлу, чтобы не экспортировать в файл, а дать возможность
	// отправки документа на принтер
	//
	exportParameters.filePath.clear();

	BusinessLogic::PdfExporter exporter;
	exporter.printPreview(_scenario, exportParameters);
}

void ExportManager::aboutExportStyleChanged(const QString& _styleName)
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue("export/style", _styleName,
		DataStorageLayer::SettingsStorage::ApplicationSettings);
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
}

#include "ExportManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>
#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/Export/PdfExporter.h>
#include <BusinessLayer/Export/RtfExporter.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <UserInterfaceLayer/Export/ExportDialog.h>

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
	if (m_exportDialog->exec() == QDialog::Accepted) {
		const QString filePath = m_exportDialog->exportFilePath();

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
			BusinessLogic::ExportParameters exportParameters;
			exportParameters.filePath = filePath;
			exportParameters.printTilte = m_exportDialog->printTitle();
			exportParameters.printPagesNumbers = m_exportDialog->printPagesNumbering();
			exportParameters.printScenesNubers = m_exportDialog->printScenesNumbering();
			exportParameters.scenesPrefix = m_exportDialog->scenesPrefix();
			exporter->exportTo(_scenario, exportParameters);
			delete exporter;
			exporter = 0;
		}
	}
}

void ExportManager::printPreviewScenario(BusinessLogic::ScenarioDocument* _scenario)
{
	BusinessLogic::ExportParameters exportParameters;
	exportParameters.printTilte = m_exportDialog->printTitle();
	exportParameters.printPagesNumbers = m_exportDialog->printPagesNumbering();
	exportParameters.printScenesNubers = m_exportDialog->printScenesNumbering();
	exportParameters.scenesPrefix = m_exportDialog->scenesPrefix();

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

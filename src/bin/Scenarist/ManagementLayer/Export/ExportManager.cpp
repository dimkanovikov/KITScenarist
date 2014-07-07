#include "ExportManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
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

void ExportManager::exportScenario(BusinessLogic::ScenarioTextDocument* _document)
{
	if (m_exportDialog->exec() == QDialog::Accepted) {
		const QString filePath = m_exportDialog->exportFilePath();

		if (!filePath.isEmpty()) {
			const QFileInfo fileInfo(filePath);

			if (fileInfo.suffix() == "rtf") {
				BusinessLogic::RtfExporter exporter;
				exporter.exportTo(_document, filePath);
			} else if (fileInfo.suffix() == "pdf") {
				BusinessLogic::PdfExporter exporter;
				exporter.exportTo(_document, filePath);
			} else {
				Q_ASSERT_X(0, Q_FUNC_INFO, qPrintable("Unknown file extension: " + fileInfo.suffix()));
			}
		}
	}
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

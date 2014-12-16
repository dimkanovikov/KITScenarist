#include "ImportManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/Import/DocumentImporter.h>

#include <Domain/Scenario.h>

#include <UserInterfaceLayer/Import/ImportDialog.h>

#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>

using ManagementLayer::ImportManager;
using UserInterface::ImportDialog;


ImportManager::ImportManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_importDialog(new ImportDialog(_parentWidget))
{
	initView();
	initConnections();
}

void ImportManager::importScenario(BusinessLogic::ScenarioDocument* _scenario, int _cursorPosition)
{
	if (m_importDialog->exec() == QDialog::Accepted) {
		BusinessLogic::ImportParameters importParameters = m_importDialog->importParameters();

		//
		// Покажем уведомление пользователю
		//
		ProgressWidget progress(m_importDialog->parentWidget());
		progress.showProgress(tr("Import"), tr("Please wait. Import can take few minutes."));

		//
		// Получим xml-представление импортируемого сценария
		//
		QString importScenarioXml = BusinessLogic::DocumentImporter().importScenario(importParameters);

		//
		// Загрузим импортируемый текст в сценарий
		//
		// ... определим позицию вставки
		//
		int insertPosition = 0;
		switch (importParameters.insertionMode) {
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
		// Закроем уведомление
		//
		progress.close();
	}
}

void ImportManager::initView()
{

}

void ImportManager::initConnections()
{

}

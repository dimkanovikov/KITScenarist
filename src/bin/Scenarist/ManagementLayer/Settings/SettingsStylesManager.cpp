#include "SettingsStylesManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <UserInterfaceLayer/Settings/StyleDialog.h>

using ManagementLayer::SettingsStylesManager;
using BusinessLogic::ScenarioStyleFacade;
using UserInterface::StyleDialog;


SettingsStylesManager::SettingsStylesManager(QObject *parent, QWidget* _parentWidget) :
	QObject(parent),
	m_view(new StyleDialog(_parentWidget))
{
}

void SettingsStylesManager::newStyle()
{
	m_view->setScenarioStyle(ScenarioStyleFacade::style());
	m_view->exec();
}

#include "SettingsStylesManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <UserInterfaceLayer/Settings/StyleDialog.h>

using ManagementLayer::SettingsStylesManager;
using BusinessLogic::ScenarioStyleFacade;
using BusinessLogic::ScenarioStyle;
using UserInterface::StyleDialog;


SettingsStylesManager::SettingsStylesManager(QObject *parent, QWidget* _parentWidget) :
	QObject(parent),
	m_view(new StyleDialog(_parentWidget))
{
}

bool SettingsStylesManager::newStyle()
{
	bool success = false;

	//
	// Настраиваем диалог настройки стиля в стандартные значения
	//
	bool isNew = true;
	m_view->setScenarioStyle(ScenarioStyleFacade::style(tr("Default")), isNew);

	//
	// Отображаем диалог пользователю
	//
	if (m_view->exec() == QDialog::Accepted) {
		//
		// Сохраняем созданный стиль
		//

		ScenarioStyle newStyle = m_view->scenarioStyle();

		//
		// Формируем имя нового стиля
		//
		{
			//
			// Нельзя чтобы имена повторялись, поэтому, в случае повторения, просто добавляем суффикс
			//
			int styleNameCounter = 0;
			QString newStyleName = newStyle.name();
			while (ScenarioStyleFacade::containsStyle(newStyleName)) {
				newStyleName = QString("%1_%2").arg(newStyle.name()).arg(++styleNameCounter);
			}
			newStyle.setName(newStyleName);
		}

		//
		// Сохраняем новый стиль
		//
		ScenarioStyleFacade::saveStyle(newStyle);
		success = true;
	}

	return success;
}

bool SettingsStylesManager::editStyle(const QString& _styleName)
{
	bool success = false;

	//
	// Настраиваем диалог настройки стиля в стандартные значения
	//
	bool isNew = false;
	m_view->setScenarioStyle(ScenarioStyleFacade::style(_styleName), isNew);

	//
	// Отображаем диалог пользователю
	//
	if (m_view->exec() == QDialog::Accepted) {
		//
		// Сохраняем изменённый стиль
		//
		ScenarioStyle editedStyle = m_view->scenarioStyle();
		ScenarioStyleFacade::saveStyle(editedStyle);
		success = true;

		//
		// Если стиль сохраняется под новым именем, нужно удалить старый
		//
		if (editedStyle.name() != _styleName) {
			ScenarioStyleFacade::removeStyle(_styleName);
		}
	}

	return success;
}

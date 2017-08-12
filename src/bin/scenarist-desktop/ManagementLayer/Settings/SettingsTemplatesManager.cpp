#include "SettingsTemplatesManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <UserInterfaceLayer/Settings/TemplateDialog.h>

using ManagementLayer::SettingsTemplatesManager;
using BusinessLogic::ScenarioTemplateFacade;
using BusinessLogic::ScenarioTemplate;
using UserInterface::TemplateDialog;


SettingsTemplatesManager::SettingsTemplatesManager(QObject *parent, QWidget* _parentWidget) :
    QObject(parent),
    m_view(new TemplateDialog(_parentWidget))
{
}

bool SettingsTemplatesManager::newTemplate()
{
    bool success = false;

    //
    // Настраиваем диалог настройки шаблона в стандартные значения
    //
    bool isNew = true;
    m_view->setScenarioTemplate(ScenarioTemplateFacade::getTemplate(ScenarioTemplate::defaultTemplateName()), isNew);

    //
    // Отображаем диалог пользователю
    //
    if (m_view->exec() == QLightBoxDialog::Accepted) {
        //
        // Сохраняем созданный шаблон
        //

        ScenarioTemplate newTemplate = m_view->scenarioTemplate();

        //
        // Формируем имя нового шаблона
        //
        {
            //
            // Нельзя чтобы имена повторялись, поэтому, в случае повторения, просто добавляем суффикс
            //
            int templateNameCounter = 0;
            QString newTemplateName = newTemplate.name();
            while (ScenarioTemplateFacade::containsTemplate(newTemplateName)) {
                newTemplateName = QString("%1_%2").arg(newTemplate.name()).arg(++templateNameCounter);
            }
            newTemplate.setName(newTemplateName);
        }

        //
        // Сохраняем новый шаблон
        //
        ScenarioTemplateFacade::saveTemplate(newTemplate);
        success = true;
    }

    return success;
}

bool SettingsTemplatesManager::editTemplate(const QString& _templateName)
{
    bool success = false;

    //
    // Настраиваем диалог настройки шаблона в стандартные значения
    //
    bool isNew = false;
    m_view->setScenarioTemplate(ScenarioTemplateFacade::getTemplate(_templateName), isNew);

    //
    // Отображаем диалог пользователю
    //
    if (m_view->exec() == QLightBoxDialog::Accepted) {
        //
        // Сохраняем изменённый шаблон
        //
        ScenarioTemplate editedTemplate = m_view->scenarioTemplate();
        ScenarioTemplateFacade::saveTemplate(editedTemplate);
        success = true;

        //
        // Если шаблон сохраняется под новым именем, нужно удалить старый
        //
        if (editedTemplate.name() != _templateName) {
            ScenarioTemplateFacade::removeTemplate(_templateName);
        }
    }

    return success;
}

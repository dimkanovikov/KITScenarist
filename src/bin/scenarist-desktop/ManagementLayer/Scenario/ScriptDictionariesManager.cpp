#include "ScriptDictionariesManager.h"

#include <UserInterfaceLayer/Scenario/ScriptDictionaries/ScriptDictionaries.h>

using ManagementLayer::ScriptDictionariesManager;
using UserInterface::ScriptDictionaries;


ScriptDictionariesManager::ScriptDictionariesManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new ScriptDictionaries(_parentWidget))
{
    initView();
}

QWidget* ScriptDictionariesManager::view() const
{
    return m_view;
}

void ScriptDictionariesManager::setCommentOnly(bool _isCommentOnly)
{

}

void ScriptDictionariesManager::initView()
{
    m_view->setObjectName("scenarioScriptDictionariesView");
}

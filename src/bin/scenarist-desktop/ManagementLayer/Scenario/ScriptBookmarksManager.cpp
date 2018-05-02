#include "ScriptBookmarksManager.h"

#include <BusinessLayer/ScenarioDocument/ScriptBookmarksModel.h>

#include <UserInterfaceLayer/Scenario/ScriptBookmarks/ScriptBookmarks.h>

using ManagementLayer::ScriptBookmarksManager;
using UserInterface::ScriptBookmarks;


ScriptBookmarksManager::ScriptBookmarksManager(QObject* _parent, QWidget* _parentWidget)
    : QObject(_parent),
      m_view(new ScriptBookmarks(_parentWidget))
{
    initView();
    initConnection();
}

QWidget* ScriptBookmarksManager::view() const
{
    return m_view;
}

void ScriptBookmarksManager::setBookmarksModel(BusinessLogic::ScriptBookmarksModel* _model)
{
    m_view->setModel(_model);
}

void ScriptBookmarksManager::setCommentOnly(bool _isCommentOnly)
{
    m_view->setCommentOnly(_isCommentOnly);
}

void ScriptBookmarksManager::initView()
{
    m_view->setObjectName("scenarioScriptBookmarksView");
}

void ScriptBookmarksManager::initConnection()
{

}

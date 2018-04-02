#include "ToolsManager.h"

#include <UserInterfaceLayer/Tools/ToolsView.h>

using ManagementLayer::ToolsManager;
using UserInterface::ToolsView;


ToolsManager::ToolsManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new ToolsView(_parentWidget))
{
    initView();
    initConnections();
}

QWidget* ToolsManager::view() const
{
    return m_view;
}

void ToolsManager::initView()
{

}

void ToolsManager::initConnections()
{

}

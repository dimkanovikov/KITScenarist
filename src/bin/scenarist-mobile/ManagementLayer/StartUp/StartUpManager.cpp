#include "StartUpManager.h"

#include <UserInterfaceLayer/StartUp/StartUpView.h>

using ManagementLayer::StartUpManager;
using UserInterface::StartUpView;


StartUpManager::StartUpManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new StartUpView(_parentWidget))
{
	initConnections();
}

QWidget* StartUpManager::toolbar() const
{
	return m_view->toolbar();
}

QWidget* StartUpManager::view() const
{
	return m_view;
}

void StartUpManager::setRecentProjects(QAbstractItemModel* _model)
{
	m_view->setRecentProjects(_model);
}

void StartUpManager::setRemoteProjects(QAbstractItemModel* _model)
{
	m_view->setRemoteProjects(_model);
}

void StartUpManager::initConnections()
{
	connect(m_view, SIGNAL(createProjectClicked()), this, SIGNAL(createProjectRequested()));

	connect(m_view, SIGNAL(openRecentProjectClicked(QModelIndex)),
			this, SIGNAL(openRecentProjectRequested(QModelIndex)));
	connect(m_view, SIGNAL(openRemoteProjectClicked(QModelIndex)),
			this, SIGNAL(openRemoteProjectRequested(QModelIndex)));
}

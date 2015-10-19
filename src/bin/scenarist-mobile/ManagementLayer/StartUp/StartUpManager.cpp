#include "StartUpManager.h"

#include <UserInterfaceLayer/StartUp/StartUpView.h>
#include <UserInterfaceLayer/StartUp/AddProjectDialog.h>

#include <3rd_party/Widgets/WAF/Animation.h>

using ManagementLayer::StartUpManager;
using UserInterface::StartUpView;
using UserInterface::AddProjectDialog;


StartUpManager::StartUpManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new StartUpView(_parentWidget)),
	m_addProjectDialog(new AddProjectDialog(_parentWidget))
{
	initView();
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

void StartUpManager::showRemoteProjects()
{
	m_view->showRemoteProjects();
}

void StartUpManager::hideRemoteProjects()
{
	m_view->hideRemoteProjects();
}

void StartUpManager::initView()
{
	m_addProjectDialog->hide();
}

void StartUpManager::initConnections()
{
	connect(m_view, &StartUpView::createProjectClicked, [=](){
		WAF::Animation::sideSlideIn(m_addProjectDialog, WAF::TopSide);
	});
	connect(m_view, SIGNAL(openRecentProjectClicked(QModelIndex)),
			this, SIGNAL(openRecentProjectRequested(QModelIndex)));
	connect(m_view, SIGNAL(openRemoteProjectClicked(QModelIndex)),
			this, SIGNAL(openRemoteProjectRequested(QModelIndex)));


	connect(m_addProjectDialog, &AddProjectDialog::createClicked, [=](){
		WAF::Animation::sideSlideOut(m_addProjectDialog);
		emit createProjectRequested(m_addProjectDialog->projectName());
	});
	connect(m_addProjectDialog, &AddProjectDialog::cancelClicked, [=](){
		WAF::Animation::sideSlideOut(m_addProjectDialog);
	});
}

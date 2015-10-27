#include "StartUpManager.h"

#include <UserInterfaceLayer/StartUp/StartUpView.h>
#include <UserInterfaceLayer/StartUp/AddProjectDialog.h>

#include <3rd_party/Widgets/WAF/Animation.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSysInfo>
#include <QUrl>

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

    sendStatistics();
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
        //
        // Создаём проект, если имя задано
        //
        if (!m_addProjectDialog->projectName().isEmpty()) {
            WAF::Animation::sideSlideOut(m_addProjectDialog);
            emit createProjectRequested(m_addProjectDialog->projectName());
        }
	});
	connect(m_addProjectDialog, &AddProjectDialog::cancelClicked, [=](){
		WAF::Animation::sideSlideOut(m_addProjectDialog);
    });
}

void StartUpManager::sendStatistics()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);

    //
    // Сформируем uuid для приложения, по которому будем идентифицировать данного пользователя
    //
    QString uuid
            = DataStorageLayer::StorageFacade::settingsStorage()->value(
                  "application/uuid", DataStorageLayer::SettingsStorage::ApplicationSettings);
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
        "application/uuid", uuid, DataStorageLayer::SettingsStorage::ApplicationSettings);

    //
    // Построим ссылку, чтобы учитывать запрос на проверку обновлений
    //
    QString url = QString("https://kitscenarist.ru/api/app/updates/");

    url.append("?system_type=");
    url.append(
#ifdef Q_OS_ANDROID
                "android"
#elif defined Q_OS_IOS
                "ios"
#else
                QSysInfo::kernelType()
#endif
                );

    url.append("&system_name=");
    url.append(QSysInfo::prettyProductName().toUtf8().toPercentEncoding());

    url.append("&uuid=");
    url.append(uuid);

    url.append("&application_version=");
    url.append(QApplication::applicationVersion());

    QNetworkRequest request = QNetworkRequest(QUrl(url));
    manager->get(request);
}

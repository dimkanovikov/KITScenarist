#include "StartUpManager.h"

#include <ManagementLayer/Project/ProjectsManager.h>

#include <UserInterfaceLayer/StartUp/StartUpView.h>
#include <UserInterfaceLayer/StartUp/LoginDialog.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/PasswordStorage.h>

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QMutableMapIterator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

using ManagementLayer::StartUpManager;
using ManagementLayer::ProjectsManager;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;
using UserInterface::StartUpView;
using UserInterface::LoginDialog;

StartUpManager::StartUpManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
    m_view(new StartUpView(_parentWidget)),
    m_loginDialog(new LoginDialog(m_view))
{
    m_loginDialog->hide();

	initData();
	initConnections();

	checkNewVersion();
}

QWidget* StartUpManager::view() const
{
	return m_view;
}

void StartUpManager::aboutUserLogged()
{
	const bool isLogged = true;
	m_view->setUserLogged(isLogged, m_userName);
    m_loginDialog->hide();
}

void StartUpManager::userSignUp()
{
    //
    // Покажем пользователю окно с вводом проверочного кода
    //
    m_loginDialog->showVerification();
}

void StartUpManager::userPassRestored()
{
    m_loginDialog->showRestore();
}

void StartUpManager::userVerified()
{
    //
    // После того, как пользователь зарегистрировался, сразу выполним вход
    //
    emit loginRequested(m_loginDialog->signUpEmail(), m_loginDialog->signUpPassword());
}

void StartUpManager::aboutRetryLogin(const QString& _error)
{
    //
    // Покажем пользователю ошибку авторизации
    //
    m_loginDialog->setLoginError(_error);
}

void StartUpManager::retrySignUp(const QString &_error)
{
    //
    // Покажем пользователю ошибку регистрации
    //
    m_loginDialog->setSignUpError(_error);
}

void StartUpManager::retryVerify(const QString &_error)
{
    //
    // Покажем пользователю ошибку ввода проверочного кода
    m_loginDialog->setVerificationError(_error);
}

void StartUpManager::aboutUserUnlogged()
{
	const bool isLogged = false;
	m_view->setUserLogged(isLogged);
}

void StartUpManager::setRecentProjects(QAbstractItemModel* _model)
{
	m_view->setRecentProjects(_model);
}

void StartUpManager::setRemoteProjects(QAbstractItemModel* _model)
{
	m_view->setRemoteProjects(_model);
}

void StartUpManager::aboutLoadUpdatesInfo(QNetworkReply* _reply)
{
	if (_reply != 0) {
		QString updatesPageData = _reply->readAll().simplified();

		//
		// Извлекаем все версии и формируем ссылку на последнюю из них
		//
#ifdef Q_OS_WIN
		QRegularExpression rx_updateFiner("scenarist-setup-(\\d+.\\d+.\\d+).exe");
#elif defined Q_OS_LINUX
		QRegularExpression rx_updateFiner("scenarist-setup-(\\d+.\\d+.\\d+)_i386.deb");
#elif defined Q_OS_MAC
		QRegularExpression rx_updateFiner("scenarist-setup-(\\d+.\\d+.\\d+).dmg");
#endif

		QRegularExpressionMatch match = rx_updateFiner.match(updatesPageData);
		QList<QString> versions;
		while (match.hasMatch()) {
			versions.append(match.captured(1));
			match = rx_updateFiner.match(updatesPageData, match.capturedEnd(1));
		}

		//
		// Если версии найдены
		//
		if (versions.count() > 0) {
			//
			// Сортируем
			//
			qSort(versions);

			//
			// Извлекаем последнюю версию
			//
			QString maxVersion = versions.last();

			//
			// Если она больше текущей версии программы, выводим информацию
			//
			if (QApplication::applicationVersion() < maxVersion) {
				QString localeSuffix;
				if (QLocale().language() == QLocale::English) {
					localeSuffix = "_en";
				} else if (QLocale().language() == QLocale::Spanish) {
					localeSuffix = "_es";
				} else if (QLocale().language() == QLocale::French) {
					localeSuffix = "_fr";
				}
				QString updateInfo =
						tr("Released version %1 ").arg(maxVersion)
#ifdef Q_OS_WIN
						+ "<a href=\"https://kitscenarist.ru/downloads/windows/scenarist-setup-" + maxVersion + ".exe\" "
#elif defined Q_OS_LINUX
#ifdef Q_PROCESSOR_X86_64
						+ "<a href=\"https://kitscenarist.ru/downloads/linux/scenarist-setup-" + maxVersion + localeSuffix + "_amd64.deb\" "
#else
						+ "<a href=\"https://kitscenarist.ru/downloads/linux/scenarist-setup-" + maxVersion + localeSuffix + "_i386.deb\" "
#endif
#elif defined Q_OS_MAC
						+ "<a href=\"https://kitscenarist.ru/downloads/mac/scenarist-setup-" + maxVersion + localeSuffix + ".dmg\" "
#endif
						+ "style=\"color:#2b78da;\">" + tr("download") + "</a> "
						+ tr("or") + " <a href=\"https://kitscenarist.ru/history.html\" "
						+ "style=\"color:#2b78da;\">" + tr("read more") + "</a>.";
				m_view->setUpdateInfo(updateInfo);
			}
		}
	}
}

void StartUpManager::initData()
{
	//
	// Загрузим имя пользователя и пароль из настроек
	//
	m_userName =
			StorageFacade::settingsStorage()->value(
				"application/user-name",
				SettingsStorage::ApplicationSettings);
	m_password =
			PasswordStorage::load(
				StorageFacade::settingsStorage()->value(
					"application/password",
					SettingsStorage::ApplicationSettings),
				m_userName
				);
}

void StartUpManager::initConnections()
{
    //
    // Показать пользователю диалог авторизации/регистрации
    // Предварительно его очистив
    //
    connect(m_view, &StartUpView::loginClicked,
            [this]() {m_loginDialog->clear();
                            m_loginDialog->show();});

    connect(m_view, &StartUpView::logoutClicked,
            this, &StartUpManager::logoutRequested);
    connect(m_view, &StartUpView::createProjectClicked,
            this, &StartUpManager::createProjectRequested);
    connect(m_view, &StartUpView::openProjectClicked,
            this, &StartUpManager::openProjectRequested);
    connect(m_view, &StartUpView::helpClicked,
            this, &StartUpManager::helpRequested);


    connect(m_view, &StartUpView::openRecentProjectClicked,
            this, &StartUpManager::openRecentProjectRequested);
    connect(m_view, &StartUpView::openRemoteProjectClicked,
            this, &StartUpManager::openRemoteProjectRequested);
    connect(m_view, &StartUpView::refreshProjects,
            this, &StartUpManager::refreshProjectsRequested);

    connect(m_loginDialog, &LoginDialog::login,
            [this]() {emit loginRequested(m_loginDialog->loginEmail(),
                                          m_loginDialog->loginPassword());});
    connect(m_loginDialog, &LoginDialog::signUp,
            [this]() {emit signUpRequested(m_loginDialog->signUpEmail(),
                                           m_loginDialog->signUpPassword(),
                                           m_loginDialog->signUpType());});
    connect(m_loginDialog, &LoginDialog::verify,
            [this]() {emit verifyRequested(m_loginDialog->verificationCode());});
    connect(m_loginDialog, &LoginDialog::restore,
            [this]() {emit restoreRequested(m_loginDialog->loginEmail());});
}

void StartUpManager::checkNewVersion()
{
	QNetworkAccessManager* manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(aboutLoadUpdatesInfo(QNetworkReply*)));

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
#ifdef Q_OS_WIN
				"windows"
#elif defined Q_OS_LINUX
				"linux"
#elif defined Q_OS_MAC
				"mac"
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

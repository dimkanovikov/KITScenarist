/*
* Copyright (C) 2014 Dimka Novikov, to@dimkanovikov.pro
* Copyright (C) 2016 Alexey Polushkin, armijo38@yandex.ru
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* Full license: http://dimkanovikov.pro/license/GPLv3
*/

#include "StartUpManager.h"

#include <ManagementLayer/Project/ProjectsManager.h>

#include <UserInterfaceLayer/StartUp/StartUpView.h>
#include <UserInterfaceLayer/StartUp/LoginDialog.h>
#include <UserInterfaceLayer/StartUp/ChangePasswordDialog.h>
#include <UserInterfaceLayer/StartUp/RenewSubscriptionDialog.h>
#include <UserInterfaceLayer/StartUp/CrashReportDialog.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/PasswordStorage.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <NetworkRequest.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QMutableMapIterator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStandardPaths>
#include <QTimer>

using ManagementLayer::StartUpManager;
using ManagementLayer::ProjectsManager;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;
using UserInterface::StartUpView;
using UserInterface::LoginDialog;
using UserInterface::ChangePasswordDialog;
using UserInterface::RenewSubscriptionDialog;
using UserInterface::CrashReportDialog;


StartUpManager::StartUpManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new StartUpView(_parentWidget)),
	m_loginDialog(new LoginDialog(m_view)),
	m_changePasswordDialog(new ChangePasswordDialog(m_view)),
	m_renewSubscriptionDialog(new RenewSubscriptionDialog(m_view))
{
	initView();

	initData();
	initConnections();

	//
	// Проверим наличие отчётов об ошибках в работе программы
	//
	QTimer::singleShot(0, this, &StartUpManager::checkCrashReports);
	//
	// Проверяем наличие новой версии уже после старта программы
	//
	QTimer::singleShot(0, this, &StartUpManager::checkNewVersion);
}

QWidget* StartUpManager::view() const
{
	return m_view;
}

bool StartUpManager::isUserLogged() const
{
	return !m_userEmail.isEmpty();
}

void StartUpManager::completeLogin(const QString& _userName, const QString& _userEmail)
{
	m_userEmail = _userEmail;

	const bool isLogged = true;
	m_view->setUserLogged(isLogged, _userName, m_userEmail);
	m_loginDialog->unblock();
	m_loginDialog->hide();
}

void StartUpManager::verifyUser()
{
	//
	// Покажем пользователю окно с вводом проверочного кода
	//
	m_loginDialog->showVerificationSuccess();
}

void StartUpManager::userAfterSignUp()
{
	//
	// После того, как пользователь зарегистрировался, сразу выполним вход
	//
	emit loginRequested(m_loginDialog->signUpEmail(), m_loginDialog->signUpPassword());
}

void StartUpManager::userPassRestored()
{
	m_loginDialog->showRestoreSuccess();
}

void StartUpManager::completeLogout()
{
	m_userEmail.clear();

	const bool isLogged = false;
	m_view->setUserLogged(isLogged);
}

void StartUpManager::passwordChanged()
{
	m_changePasswordDialog->stopAndHide();
	QLightBoxMessage::information(m_view, tr("Password changed"),
								  tr("Password successfully changed"));
}

void StartUpManager::showPasswordError(const QString& _error)
{
	if (m_loginDialog->isVisible()) {
		//
		// Если активно окно авторизации, то покажем ошибку там
		//
		retrySignUp(_error);
	} else {
		//
		// Иначе, активно окно смены пароля
		//
		m_changePasswordDialog->stopAndHide();
		QLightBoxMessage::critical(m_view, tr("Can not change password"),
									  _error);
		m_changePasswordDialog->showUnprepared();
	}
}

void StartUpManager::setSubscriptionInfo(bool _isActive, const QString &_expiredDate)
{
	if (m_renewSubscriptionDialog->isVisible()) {
		//
		// Если окно продления подписки показано, значит,
		// необходимо обновлять, пока не получим изменения
		//
		if (_expiredDate != m_subscriptionEndDate) {
			//
			// Обновилось, обновим окно и поле в StartUpView
			//
			m_renewSubscriptionDialog->showThanks(_expiredDate);
			m_view->setSubscriptionInfo(_isActive, _expiredDate);
		} else {
			//
			// Не обновилось, запросим еще раз
			//
			QTimer::singleShot(3000, this, &StartUpManager::getSubscriptionInfoRequested);
		}
	} else {
		//
		// Иначе, это обычный запрос на обновление
		//
		m_subscriptionEndDate = _expiredDate;
		m_view->setSubscriptionInfo(_isActive, _expiredDate);
	}
}

void StartUpManager::setRecentProjects(QAbstractItemModel* _model)
{
	m_view->setRecentProjects(_model);
}

void StartUpManager::setRemoteProjects(QAbstractItemModel* _model)
{
	m_view->setRemoteProjects(_model);
}

void StartUpManager::retryLogin(const QString& _error)
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
	//
    m_loginDialog->setVerificationError(_error);
}

void StartUpManager::retryLastAction(const QString &_error)
{
    m_loginDialog->setLastActionError(_error);
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
}

void StartUpManager::initView()
{
	m_loginDialog->hide();
}

void StartUpManager::initConnections()
{
	//
	// Показать пользователю диалог авторизации/регистрации
	// Предварительно его очистив
	//
	connect(m_view, &StartUpView::loginClicked, [this] {
		m_loginDialog->showPrepared();
	});

	connect(m_view, &StartUpView::logoutClicked, this, &StartUpManager::logoutRequested);
	connect(m_view, &StartUpView::createProjectClicked, this, &StartUpManager::createProjectRequested);
	connect(m_view, &StartUpView::openProjectClicked, this, &StartUpManager::openProjectRequested);
	connect(m_view, &StartUpView::helpClicked, this, &StartUpManager::helpRequested);

	connect(m_view, &StartUpView::openRecentProjectClicked, this, &StartUpManager::openRecentProjectRequested);
	connect(m_view, &StartUpView::hideRecentProjectRequested, this, &StartUpManager::hideRecentProjectRequested);
	connect(m_view, &StartUpView::openRemoteProjectClicked, this, &StartUpManager::openRemoteProjectRequested);
	connect(m_view, &StartUpView::editRemoteProjectRequested, this, &StartUpManager::editRemoteProjectRequested);
	connect(m_view, &StartUpView::removeRemoteProjectRequested, this, &StartUpManager::removeRemoteProjectRequested);
	connect(m_view, &StartUpView::shareRemoteProjectRequested, this, &StartUpManager::shareRemoteProjectRequested);
	connect(m_view, &StartUpView::unshareRemoteProjectRequested, this, &StartUpManager::unshareRemoteProjectRequested);
	connect(m_view, &StartUpView::refreshProjects, this, &StartUpManager::refreshProjectsRequested);

	connect(m_loginDialog, &LoginDialog::loginRequested, [this] {
		emit loginRequested(m_loginDialog->loginEmail(),
							m_loginDialog->loginPassword());}
	);
	connect(m_loginDialog, &LoginDialog::signUpRequested, [this] {
		emit signUpRequested(m_loginDialog->signUpEmail(),
							 m_loginDialog->signUpPassword());
	});
	connect(m_loginDialog, &LoginDialog::verifyRequested, [this] {
		emit verifyRequested(m_loginDialog->verificationCode());
	});
	connect(m_loginDialog, &LoginDialog::restoreRequested, [this] {
		emit restoreRequested(m_loginDialog->loginEmail());
	});
	connect(m_view, &StartUpView::userNameChanged,
			this, &StartUpManager::userNameChangeRequested);
	connect(m_view, &StartUpView::getSubscriptionInfoClicked,
			this, &StartUpManager::getSubscriptionInfoRequested);
	connect(m_view, &StartUpView::renewSubscriptionClicked,
			m_renewSubscriptionDialog, &RenewSubscriptionDialog::showPrepared);
	connect(m_view, &StartUpView::passwordChangeClicked, [this] {
		m_changePasswordDialog->showPrepared();
	});

	connect(m_changePasswordDialog, &ChangePasswordDialog::changeRequested, [this] {
		emit passwordChangeRequested(m_changePasswordDialog->password(),
									 m_changePasswordDialog->newPassword());
	});

	connect(m_renewSubscriptionDialog, &RenewSubscriptionDialog::renewSubsciptionRequested, [this] {
		emit renewSubscriptionRequested(m_renewSubscriptionDialog->duration(), m_renewSubscriptionDialog->paymentSystemType());
	});
	connect(m_renewSubscriptionDialog, &RenewSubscriptionDialog::renewSubsciptionRequested, [this] {
		QTimer::singleShot(3000, this, &StartUpManager::getSubscriptionInfoRequested);
	});
}

void StartUpManager::checkCrashReports()
{
	const QString SENDED = "sended";
	const QString IGNORED = "ignored";

	//
	// Настроим отлавливание ошибок
	//
	QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString crashReportsFolderPath = appDataFolderPath + QDir::separator() + "CrashReports";
	bool hasUnhandledReports = false;
	QString unhandledReportPath;
	for (const QFileInfo& crashReportFileInfo : QDir(crashReportsFolderPath).entryInfoList(QDir::Files)) {
		if (crashReportFileInfo.suffix() != SENDED
			&& crashReportFileInfo.suffix() != IGNORED) {
			hasUnhandledReports = true;
			unhandledReportPath = crashReportFileInfo.filePath();
			break;
		}
	}

	//
	// Если есть необработанные отчёты, показать диалог
	//
	if (hasUnhandledReports) {
		CrashReportDialog dialog(m_view);
		QString handledReportPath = unhandledReportPath;
		if (dialog.exec() == CrashReportDialog::Accepted) {
			dialog.showProgress();
			//
			// Отправляем
			//
			NetworkRequest loader;
			loader.addRequestAttribute("version", QApplication::applicationVersion());
			loader.addRequestAttribute("email", dialog.email());
			loader.addRequestAttribute("message", dialog.message());
			loader.addRequestAttributeFile("report", unhandledReportPath);
			loader.loadSync("https://kitscenarist.ru/api/app/feedback/");

			//
			// Помечаем отчёт, как отправленный
			//
			handledReportPath += "." + SENDED;
		}
		//
		// Помечаем отчёт, как проигнорированный
		//
		else {
			handledReportPath += "." + IGNORED;
		}
		QFile::rename(unhandledReportPath, handledReportPath);
	}
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

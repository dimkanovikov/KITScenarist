/*
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

#include "SynchronizationManagerV2.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <NetworkRequest.h>

#include <QXmlStreamReader>
#include <QEventLoop>
#include <QTimer>
#include <QDesktopServices>
#include <QDateTime>

using ManagementLayer::SynchronizationManagerV2;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;

//
// **** из старого менеджера синхронизации
//

#include <ManagementLayer/Project/ProjectsManager.h>
#include <ManagementLayer/Project/Project.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioChangeStorage.h>
#include <DataLayer/DataStorageLayer/DatabaseHistoryStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>

#include <DataLayer/Database/Database.h>

#include <Domain/Scenario.h>
#include <Domain/ScenarioChange.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <NetworkRequest.h>

#include <QEventLoop>
#include <QHash>
#include <QNetworkConfigurationManager>
#include <QScopedPointer>
#include <QTimer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

using ManagementLayer::ProjectsManager;

namespace {
	/**
	 * @brief Список URL адресов, по которым осуществляются запросы
	 */
	/** @{ */
	const QUrl URL_SIGNUP = QUrl("https://kitscenarist.ru/api/account/register/");
	const QUrl URL_RESTORE = QUrl("https://kitscenarist.ru/api/account/restore/");
	const QUrl URL_LOGIN = QUrl("https://kitscenarist.ru/api/account/login/");
	const QUrl URL_LOGOUT = QUrl("https://kitscenarist.ru/api/account/logout/");
	const QUrl URL_UPDATE = QUrl("https://kitscenarist.ru/api/account/update/");
	const QUrl URL_SUBSCRIBE_STATE = QUrl("https://kitscenarist.ru/api/account/subscribe/state/");
	//
	const QUrl URL_PROJECTS = QUrl("https://kitscenarist.ru/api/projects/");
	const QUrl URL_CREATE_PROJECT = QUrl("https://kitscenarist.ru/api/projects/create/");
	const QUrl URL_UPDATE_PROJECT = QUrl("https://kitscenarist.ru/api/projects/edit/");
	const QUrl URL_REMOVE_PROJECT = QUrl("https://kitscenarist.ru/api/projects/remove/");
	const QUrl URL_CREATE_PROJECT_SUBSCRIPTION = QUrl("https://kitscenarist.ru/api/projects/share/create/");
	const QUrl URL_REMOVE_PROJECT_SUBSCRIPTION = QUrl("https://kitscenarist.ru/api/projects/share/remove/");
	//
	const QUrl URL_SCENARIO_CHANGE_LIST = QUrl("https://kitscenarist.ru/api/projects/scenario/change/list/");
	const QUrl URL_SCENARIO_CHANGE_LOAD = QUrl("https://kitscenarist.ru/api/projects/scenario/change/");
	const QUrl URL_SCENARIO_CHANGE_SAVE = QUrl("https://kitscenarist.ru/api/projects/scenario/change/save/");
	const QUrl URL_SCENARIO_CURSORS = QUrl("https://kitscenarist.ru/api/projects/scenario/cursor/");
	//
	const QUrl URL_SCENARIO_DATA_LIST = QUrl("https://kitscenarist.ru/api/projects/data/list/");
	const QUrl URL_SCENARIO_DATA_LOAD = QUrl("https://kitscenarist.ru/api/projects/data/");
	const QUrl URL_SCENARIO_DATA_SAVE = QUrl("https://kitscenarist.ru/api/projects/data/save/");
	/** @} */

	/**
	 * @brief Список названий параметров для запросов
	 */
	/** @{ */
	const QString KEY_EMAIL = "email";
	const QString KEY_LOGIN = "login";
	const QString KEY_USERNAME = "username";
	const QString KEY_PASSWORD = "password";
	const QString KEY_NEW_PASSWORD = "new_password";
	const QString KEY_SESSION_KEY = "session_key";
	const QString KEY_ROLE = "role";
	//
	const QString KEY_PROJECT_ID = "project_id";
	const QString KEY_PROJECT_NAME = "project_name";
	/** @} */

	/**
	 * @brief Список кодов ошибок и соответствующих им описаний
	 */
	/** @{ */
	const int UNKNOWN_ERROR_CODE = 100;
	const QString UNKNOWN_ERROR_STRING = QObject::tr("Unknown error");

	const int SESSION_KEY_NOT_FOUND_CODE = 101;
	const QString SESSION_KEY_NOT_FOUND_STRING = QObject::tr("Session key not found");
	/** @} */

	//
	// **** из старого менеджера синхронизации
	//

	/**
	 * @brief Ключи для параметров запросов
	 */
	/** @{ */
	const QString KEY_USER_NAME = "login";
	const QString KEY_PROJECT = "project_id";
	const QString KEY_CHANGES = "changes";
	const QString KEY_CHANGES_IDS = "changes_ids";
	const QString KEY_SCENARIO_IS_DRAFT = "scenario_is_draft";
	const QString KEY_FROM_LAST_MINUTES = "from_last_minutes";
	const QString KEY_CURSOR_POSITION = "cursor_position";
	/** @{ */

	/**
	 * @brief Ключи для доступа к данным об изменении сценария
	 */
	/** @{ */
	const QString SCENARIO_CHANGE_ID = "id";
	const QString SCENARIO_CHANGE_DATETIME = "datetime";
	const QString SCENARIO_CHANGE_USERNAME = "username";
	const QString SCENARIO_CHANGE_UNDO_PATCH = "undo_patch";
	const QString SCENARIO_CHANGE_REDO_PATCH = "redo_patch";
	const QString SCENARIO_CHANGE_IS_DRAFT = "is_draft";
	/** @} */

	/**
	 * @brief Ключи для доступа к записям из истории изменений БД
	 */
	/** @{ */
	const QString DBH_ID_KEY = "id";
	const QString DBH_QUERY_KEY = "query";
	const QString DBH_QUERY_VALUES_KEY = "query_values";
	const QString DBH_DATETIME_KEY = "datetime";
	const QString DBH_ORDER_KEY = "order";
	/** @} */

	const bool IS_CLEAN = false;
	const bool IS_DRAFT = true;
	const bool IS_ASYNC = true;
	const bool IS_SYNC = false;

	/**
	 * @brief Код ошибки означающий работу в автономном режиме
	 */
	const int OFFLINE_ERROR_CODE = 0;
}

namespace {
	/**
	 * @brief Преобразовать дату из гггг.мм.дд чч:мм:сс в дд.мм.гггг
	 */
	QString dateTransform(const QString &_date)
	{
		return QDateTime::fromString(_date, "yyyy-MM-dd hh:mm:ss").toString("dd.MM.yyyy");
	}
}

SynchronizationManagerV2::SynchronizationManagerV2(QObject* _parent, QWidget* _parentView) :
	QObject(_parent),
	m_view(_parentView),
	m_networkManager(new QNetworkConfigurationManager(_parent)),
	m_isSubscriptionActive(false),
	m_loader(new NetworkRequest(this))
{
	initConnections();
}

void SynchronizationManagerV2::firstStateConnection()
{
	networkStateChanged(m_networkManager->isOnline());
}

bool SynchronizationManagerV2::isInternetConnectionActive() const
{
	return m_isInternetConnectionActive;
}

bool SynchronizationManagerV2::isLogged() const
{
	return !m_sessionKey.isEmpty();
}

bool SynchronizationManagerV2::isSubscriptionActive() const
{
	return m_isSubscriptionActive;
}

void SynchronizationManagerV2::autoLogin()
{
	//
	// Получим параметры из хранилища
	//
	const QString email =
			StorageFacade::settingsStorage()->value(
				"application/email",
				SettingsStorage::ApplicationSettings);
	const QString password =
			PasswordStorage::load(
				StorageFacade::settingsStorage()->value(
					"application/password",
					SettingsStorage::ApplicationSettings),
				email
				);

	//
	// Если они не пусты, авторизуемся
	//
	if (!email.isEmpty() && !password.isEmpty()) {
		login(email, password);
	}
}

void SynchronizationManagerV2::login(const QString &_email, const QString &_password)
{

	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_LOGIN, _email);
	m_loader->addRequestAttribute(KEY_PASSWORD, _password);
	QByteArray response = m_loader->loadSync(URL_LOGIN);

	//
	// Считываем результат авторизации
	//
	QXmlStreamReader responseReader(response);
	//
	// Успешно ли завершилась авторизация
	//
	if(!isOperationSucceed(responseReader)) {
		return;
	}

	QString userName;
	QString date;

	bool isActiveFind = false;
	//
	// Найдем наш ключ сессии, имя пользователя, информацию о подписке
	//
	m_sessionKey.clear();
	while (!responseReader.atEnd()) {
		responseReader.readNext();
		if (responseReader.name().toString() == "session_key") {
			responseReader.readNext();
			m_sessionKey = responseReader.text().toString();
			responseReader.readNext();
		} else if (responseReader.name().toString() == "user_name") {
			responseReader.readNext();
			userName = responseReader.text().toString();
			responseReader.readNext();
		} else if (responseReader.name().toString() == "subscribe_is_active") {
			isActiveFind = true;
			responseReader.readNext();
			m_isSubscriptionActive = responseReader.text().toString() == "true";
			responseReader.readNext();
		} else if (responseReader.name().toString() == "subscribe_end") {
			responseReader.readNext();
			date = responseReader.text().toString();
			responseReader.readNext();
		}
	}

	if (!isActiveFind || (isActiveFind && m_isSubscriptionActive && date.isEmpty())) {
		handleError(tr("Got wrong result from server"), 404);
		m_sessionKey.clear();
		return;
	}

	//
	// Не нашли ключ сессии
	//
	if (m_sessionKey.isEmpty()) {
		handleError(SESSION_KEY_NOT_FOUND_STRING, SESSION_KEY_NOT_FOUND_CODE);
		return;
	}

	//
	// Если авторизация успешна, сохраним информацию о пользователе
	//
	StorageFacade::settingsStorage()->setValue(
				"application/email",
				_email,
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"application/password",
				PasswordStorage::save(_password, _email),
				SettingsStorage::ApplicationSettings);

	//
	// Запомним email
	//
	m_userEmail = _email;

	emit subscriptionInfoLoaded(m_isSubscriptionActive, dateTransform(date));
	emit loginAccepted(userName, m_userEmail);
}

void SynchronizationManagerV2::signUp(const QString& _email, const QString& _password)
{
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_EMAIL, _email);
	m_loader->addRequestAttribute(KEY_PASSWORD, _password);
	QByteArray response = m_loader->loadSync(URL_SIGNUP);

	//
	// Считываем результат авторизации
	//
	QXmlStreamReader responseReader(response);
	//
	// Успешно ли завершилась авторизация
	//
	if(!isOperationSucceed(responseReader)) {
		return;
	}

	//
	// Найдем наш ключ сессии
	//
	m_sessionKey.clear();
	while (!responseReader.atEnd()) {
		responseReader.readNext();
		if (responseReader.name().toString() == "session_key") {
			responseReader.readNext();
			m_sessionKey = responseReader.text().toString();
			responseReader.readNext();
			break;
		}
	}

	//
	// Не нашли ключ сессии
	//
	if (m_sessionKey.isEmpty()) {
		handleError(SESSION_KEY_NOT_FOUND_STRING, SESSION_KEY_NOT_FOUND_CODE);
		return;
	}

	//
	// Если авторизация прошла
	//
	emit signUpFinished();
}

void SynchronizationManagerV2::verify(const QString& _code)
{
	//
	// FIXME: Поменять в рабочей версии
	//
	QEventLoop event;
	QTimer::singleShot(2000, &event, SLOT(quit()));
	event.exec();

	bool success = false;
	if (_code == "11111") {
		success = true;
	} else {
		handleError("Wrong code", 505);
	}

	if (success) {
		emit verified();
	}
}

void SynchronizationManagerV2::restorePassword(const QString &_email)
{
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_EMAIL, _email);
	QByteArray response = m_loader->loadSync(URL_RESTORE);

	//
	// Считываем результат
	//
	QXmlStreamReader responseReader(response);
	//
	// Успешно ли завершилась операция
	//
	if(!isOperationSucceed(responseReader)) {
		return;
	}

	//
	// Найдем статус
	//
	while (!responseReader.atEnd()) {
		responseReader.readNext();
		if (responseReader.name().toString() == "send_mail_result") {
			responseReader.readNext();
			QString status = responseReader.text().toString();
			responseReader.readNext();
			if (status != "success") {
				handleError(status, UNKNOWN_ERROR_CODE);
				return;
			}
			break;
		}
	}

	//
	// Если успешно отправили письмо
	//
	emit passwordRestored();
}

void SynchronizationManagerV2::logout()
{
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	QByteArray response = m_loader->loadSync(URL_LOGOUT);

	m_sessionKey.clear();
	m_userEmail.clear();

	//
	// Удаляем сохраненные значения, если они были
	//
	StorageFacade::settingsStorage()->setValue(
				"application/email",
				QString::null,
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"application/password",
				QString::null,
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"application/remote-projects",
				QString::null,
				SettingsStorage::ApplicationSettings);

	//
	// Если деавторизация прошла
	//
	emit logoutFinished();
}

void SynchronizationManagerV2::renewSubscription(unsigned _duration,
												 unsigned _type)
{
	QDesktopServices::openUrl(QUrl(QString("http://kitscenarist.ru/api/account/subscribe/?"
										   "user=%1&month=%2&payment_type=%3").
								   arg(m_userEmail).arg(_duration).
								   arg(_type == 0 ? "AC" : "PC")));
}

void SynchronizationManagerV2::changeUserName(const QString &_newUserName)
{
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	m_loader->addRequestAttribute(KEY_USERNAME, _newUserName);
	QByteArray response = m_loader->loadSync(URL_UPDATE);

	//
	// Считываем результат авторизации
	//
	QXmlStreamReader responseReader(response);

	if (!isOperationSucceed(responseReader)) {
		return;
	}
	emit userNameChanged();
}

void SynchronizationManagerV2::loadSubscriptionInfo()
{
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	QByteArray response = m_loader->loadSync(URL_SUBSCRIBE_STATE);

	//
	// Считываем результат авторизации
	//
	QXmlStreamReader responseReader(response);
	if (!isOperationSucceed(responseReader)) {
		return;
	}

	//
	// Распарсим результат
	//
	bool isActive;
	QString date;

	bool isActiveFind = false;
	while (!responseReader.atEnd()) {
		responseReader.readNext();
		if (responseReader.name().toString() == "subscribe_is_active") {
			isActiveFind = true;
			responseReader.readNext();
			m_isSubscriptionActive = responseReader.text().toString() == "true";
			responseReader.readNext();
		} else if (responseReader.name().toString() == "subscribe_end") {
			responseReader.readNext();
			date = responseReader.text().toString();
			responseReader.readNext();
		}
	}

	if (!isActiveFind || (isActiveFind && m_isSubscriptionActive && date.isEmpty())) {
		handleError(tr("Got wrong result from server"), 404);
		return;
	}

	emit subscriptionInfoLoaded(m_isSubscriptionActive, dateTransform(date));
}

void SynchronizationManagerV2::changePassword(const QString& _password,
											  const QString& _newPassword)
{
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	m_loader->addRequestAttribute(KEY_PASSWORD, _password);
	m_loader->addRequestAttribute(KEY_NEW_PASSWORD, _newPassword);
	QByteArray response = m_loader->loadSync(URL_UPDATE);

	//
	// Считываем результат авторизации
	//
	QXmlStreamReader responseReader(response);

	if (!isOperationSucceed(responseReader)) {
		return;
	}
	emit passwordChanged();
}

void SynchronizationManagerV2::loadProjects()
{
	//
	// Получаем список проектов
	//
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	const QByteArray response = m_loader->loadSync(URL_PROJECTS);

	//
	// Считываем результат
	//
	QXmlStreamReader responseReader(response);
	if (!isOperationSucceed(responseReader)) {
		return;
	}

	//
	// Сохраняем список проектов для работы в автономном режиме
	//
	StorageFacade::settingsStorage()->setValue("application/remote-projects",
		response.toBase64(), SettingsStorage::ApplicationSettings);

	//
	// Уведомляем о получении проектов
	//
	emit projectsLoaded(response);
}

int SynchronizationManagerV2::createProject(const QString& _projectName)
{
	const int INVALID_PROJECT_ID = -1;

	//
	// Создаём новый проект
	//
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	m_loader->addRequestAttribute(KEY_PROJECT_NAME, _projectName);
	const QByteArray response = m_loader->loadSync(URL_CREATE_PROJECT);

	//
	// Считываем результат
	//
	QXmlStreamReader responseReader(response);
	if (!isOperationSucceed(responseReader)) {
		return INVALID_PROJECT_ID;
	}

	//
	// Считываем идентификатор проекта
	//
	int newProjectId = INVALID_PROJECT_ID;
	while (!responseReader.atEnd()) {
		responseReader.readNext();
		if (responseReader.name().toString() == "project") {
			newProjectId = responseReader.attributes().value("id").toInt();
			break;
		}
	}

	if (newProjectId == INVALID_PROJECT_ID) {
		handleError(tr("Got wrong result from server"), 404);
		return INVALID_PROJECT_ID;
	}

	//
	// Если проект успешно добавился перечитаем список проектов
	//
	loadProjects();

	return newProjectId;
}

void SynchronizationManagerV2::updateProjectName(int _projectId, const QString& _newProjectName)
{
	//
	// Обновляем проект
	//
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	m_loader->addRequestAttribute(KEY_PROJECT_ID, _projectId);
	m_loader->addRequestAttribute(KEY_PROJECT_NAME, _newProjectName);
	const QByteArray response = m_loader->loadSync(URL_UPDATE_PROJECT);

	//
	// Считываем результат
	//
	QXmlStreamReader responseReader(response);
	if (!isOperationSucceed(responseReader)) {
		return;
	}

	//
	// Если проект успешно обновился перечитаем список проектов
	//
	loadProjects();
}

void SynchronizationManagerV2::removeProject(int _projectId)
{
	//
	// Удаляем проект
	//
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	m_loader->addRequestAttribute(KEY_PROJECT_ID, _projectId);
	const QByteArray response = m_loader->loadSync(URL_REMOVE_PROJECT);

	//
	// Считываем результат
	//
	QXmlStreamReader responseReader(response);
	if (!isOperationSucceed(responseReader)) {
		return;
	}

	//
	// Если проект успешно удалён перечитаем список проектов
	//
	loadProjects();
}

void SynchronizationManagerV2::shareProject(int _projectId, const QString& _userEmail, int _role)
{
	const QString userRole = _role == 0 ? "redactor" : "commentator";

	//
	// ДОбавляем подписчика в проект
	//
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	m_loader->addRequestAttribute(KEY_PROJECT_ID, _projectId);
	m_loader->addRequestAttribute(KEY_EMAIL, _userEmail);
	m_loader->addRequestAttribute(KEY_ROLE, userRole);
	const QByteArray response = m_loader->loadSync(URL_CREATE_PROJECT_SUBSCRIPTION);

	//
	// Считываем результат
	//
	QXmlStreamReader responseReader(response);
	if (!isOperationSucceed(responseReader)) {
		return;
	}

	//
	// Если подписчик добавлен, перечитаем список проектов
	//
	loadProjects();
}

void SynchronizationManagerV2::unshareProject(int _projectId, const QString& _userEmail)
{
	//
	// Убираем подписчика из проекта
	//
	m_loader->setRequestMethod(NetworkRequest::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	m_loader->addRequestAttribute(KEY_PROJECT_ID, _projectId);
	m_loader->addRequestAttribute(KEY_EMAIL, _userEmail.trimmed());
	const QByteArray response = m_loader->loadSync(URL_REMOVE_PROJECT_SUBSCRIPTION);

	//
	// Считываем результат
	//
	QXmlStreamReader responseReader(response);
	if (!isOperationSucceed(responseReader)) {
		return;
	}

	//
	// Если подписчик удалён перечитаем список проектов
	//
	loadProjects();
}

void SynchronizationManagerV2::aboutFullSyncScenario()
{
	if (isCanSync()) {
		//
		// Запоминаем время синхронизации изменений сценария, в дальнейшем будем отправлять
		// изменения произведённые с данного момента
		//
		m_lastChangesSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

		//
		// Получить список патчей проекта
		//
		m_loader->setRequestMethod(NetworkRequest::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		QByteArray response = loadSyncWrapper(URL_SCENARIO_CHANGE_LIST);

		//
		// ... считываем изменения (uuid)
		//
		QList<QString> remoteChanges;
		QXmlStreamReader changesReader(response);
		while (!changesReader.atEnd()) {
			changesReader.readNext();
			if (changesReader.name().toString() == "status") {
				const bool success = changesReader.attributes().value("result").toString() == "true";
				if (success) {
					changesReader.readNextStartElement();
					changesReader.readNextStartElement(); // changes
					while (!changesReader.atEnd()) {
						changesReader.readNextStartElement();
						if (changesReader.name() == "change") {
							const QString changeUuid = changesReader.attributes().value("id").toString();
							if (!changeUuid.isEmpty()) {
								remoteChanges.append(changeUuid);
							}
						}
					}
				} else {
					handleError(response);
					break;
				}
			}
		}


		//
		// Сформируем список изменений сценария хранящихся локально
		//
		QList<QString> localChanges = StorageFacade::scenarioChangeStorage()->uuids();


		//
		// Отправить на сайт все изменения, которых там нет
		//
		{
			QList<QString> changesForUpload;
			foreach (const QString& changeUuid, localChanges) {
				//
				// ... отправлять нужно, если такого изменения нет на сайте
				//
				const bool needUpload = !remoteChanges.contains(changeUuid);

				if (needUpload) {
					changesForUpload.append(changeUuid);
				}
			}
			//
			// ... отправляем
			//
			uploadScenarioChanges(changesForUpload);
		}

		//
		// Скачать все изменения, которых ещё нет
		//
		{
			QStringList changesForDownload;
			foreach (const QString& changeUuid, remoteChanges) {
				//
				// ... сохранять нужно, если такого изменения нет в локальной БД
				//
				const bool needDownload = !localChanges.contains(changeUuid);

				if (needDownload) {
					changesForDownload.append(changeUuid);
				}
			}
			//
			// ... скачиваем
			//
			const QList<QHash<QString, QString> > changes =
					downloadScenarioChanges(changesForDownload.join(";"));
			//
			// ... применять будет пачками
			//
			QList<QString> cleanPatches;
			QList<QString> draftPatches;
			QHash<QString, QString> change;
			foreach (change, changes) {
				if (!change.isEmpty()) {
					//
					// ... добавляем
					//
					DataStorageLayer::StorageFacade::scenarioChangeStorage()->append(
								change.value(SCENARIO_CHANGE_ID), change.value(SCENARIO_CHANGE_DATETIME),
								change.value(SCENARIO_CHANGE_USERNAME), change.value(SCENARIO_CHANGE_UNDO_PATCH),
								change.value(SCENARIO_CHANGE_REDO_PATCH), change.value(SCENARIO_CHANGE_IS_DRAFT).toInt());

					if (change.value(SCENARIO_CHANGE_IS_DRAFT).toInt()) {
						draftPatches.append(change.value(SCENARIO_CHANGE_REDO_PATCH));
					} else {
						cleanPatches.append(change.value(SCENARIO_CHANGE_REDO_PATCH));
					}
				}
			}
			//
			// ... применяем
			//
			if (!cleanPatches.isEmpty()) {
				emit applyPatchesRequested(cleanPatches, IS_CLEAN);
			}
			if (!draftPatches.isEmpty()) {
				emit applyPatchesRequested(draftPatches, IS_DRAFT);
			}

			//
			// ... сохраняем
			//
			DataStorageLayer::StorageFacade::scenarioChangeStorage()->store();
		}
	}
}

void SynchronizationManagerV2::aboutWorkSyncScenario()
{
	if (isCanSync()) {

		//
		// Отправляем новые изменения
		//
		{
			//
			// Запоминаем время синхронизации изменений сценария
			//
			const QString prevChangesSyncDatetime = m_lastChangesSyncDatetime;
			m_lastChangesSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

			//
			// Отправляем
			//
			QList<QString> newChanges =
					StorageFacade::scenarioChangeStorage()->newUuids(prevChangesSyncDatetime);
			const bool changesUploaded = uploadScenarioChanges(newChanges);

			//
			// Не обновляем время последней синхронизации, если изменения не были отправлены
			//
			if (changesUploaded == false) {
				m_lastChangesSyncDatetime = prevChangesSyncDatetime;
			}
		}

		//
		// Загружаем и применяем изменения от других пользователей за последние LAST_MINUTES минут
		//
		{
			const int LAST_MINUTES = 2;

			m_loader->setRequestMethod(NetworkRequest::Post);
			m_loader->clearRequestAttributes();
			m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
			m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
			m_loader->addRequestAttribute(KEY_FROM_LAST_MINUTES, LAST_MINUTES);
			QByteArray response = loadSyncWrapper(URL_SCENARIO_CHANGE_LIST);

			//
			// ... считываем uuid'ы новых изменений
			//
			QList<QString> remoteChanges;
			QXmlStreamReader changesReader(response);
			while (!changesReader.atEnd()) {
				changesReader.readNext();
				if (changesReader.name().toString() == "status") {
					const bool success = changesReader.attributes().value("result").toString() == "true";
					if (success) {
						changesReader.readNextStartElement();
						changesReader.readNextStartElement(); // changes
						while (!changesReader.atEnd()) {
							changesReader.readNextStartElement();
							if (changesReader.name() == "change") {
								const QString changeUuid = changesReader.attributes().value("id").toString();
								if (!changeUuid.isEmpty()) {
									remoteChanges.append(changeUuid);
								}
							}
						}
					} else {
						handleError(response);
						break;
					}
				}
			}

			//
			// ... скачиваем все изменения, которых ещё нет
			//
			QStringList changesForDownload;
			foreach (const QString& changeUuid, remoteChanges) {
				//
				// ... сохранять нужно, если такого изменения нет
				//
				const bool needDownload =
						!DataStorageLayer::StorageFacade::scenarioChangeStorage()->contains(changeUuid);

				if (needDownload) {
					changesForDownload.append(changeUuid);
				}
			}
			//
			// ... скачиваем
			//
			const QList<QHash<QString, QString> > changes = downloadScenarioChanges(changesForDownload.join(";"));
			QHash<QString, QString> change;
			foreach (change, changes) {
				if (!change.isEmpty()) {
					//
					// ... сохраняем
					//
					StorageFacade::scenarioChangeStorage()->append(
						change.value(SCENARIO_CHANGE_ID), change.value(SCENARIO_CHANGE_DATETIME),
						change.value(SCENARIO_CHANGE_USERNAME), change.value(SCENARIO_CHANGE_UNDO_PATCH),
						change.value(SCENARIO_CHANGE_REDO_PATCH), change.value(SCENARIO_CHANGE_IS_DRAFT).toInt());

					//
					// ... применяем
					//
					emit applyPatchRequested(change.value(SCENARIO_CHANGE_REDO_PATCH),
											 change.value(SCENARIO_CHANGE_IS_DRAFT).toInt());
				}
			}
		}
	}
}

void SynchronizationManagerV2::aboutFullSyncData()
{
	if (isCanSync()) {
		//
		// Запоминаем время синхронизации данных, в дальнейшем будем отправлять изменения
		// произведённые с данного момента
		//
		m_lastDataSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

		//
		// Получить список всех изменений данных на сервере
		//
		NetworkRequest loader;
		loader.setRequestMethod(NetworkRequest::Post);
		loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		QByteArray response = loader.loadSync(URL_SCENARIO_DATA_LIST);
		//
		// ... считываем изменения (uuid)
		//
		QList<QString> remoteChanges;
		QXmlStreamReader changesReader(response);
		while (!changesReader.atEnd()) {
			changesReader.readNext();
			if (changesReader.name().toString() == "status") {
				const bool success = changesReader.attributes().value("result").toString() == "true";
				if (success) {
					changesReader.readNextStartElement();
					changesReader.readNextStartElement(); // changes
					while (!changesReader.atEnd()) {
						changesReader.readNextStartElement();
						if (changesReader.name() == "change") {
							const QString changeUuid = changesReader.attributes().value("id").toString();
							if (!changeUuid.isEmpty()) {
								remoteChanges.append(changeUuid);
							}
						}
					}
				} else {
					handleError(response);
					break;
				}
			}
		}

		//
		// Сформируем список изменений сценария хранящихся локально
		//
		QList<QString> localChanges = StorageFacade::databaseHistoryStorage()->history(QString::null);

		//
		// Отправить на сайт все версии, которых на сайте нет
		//
		{
			QList<QString> changesForUpload;
			foreach (const QString& changeUuid, localChanges) {
				//
				// ... отправлять нужно, если такого изменения нет на сайте
				//
				const bool needUpload = !remoteChanges.contains(changeUuid);

				if (needUpload) {
					changesForUpload.append(changeUuid);
				}
			}
			//
			// ... отправляем
			//
			uploadScenarioData(changesForUpload);
		}


		//
		// Сохранить в локальной БД все изменения, которых в ней нет
		//
		{
			QStringList changesForDownloadAndSave;
			foreach (const QString& changeUuid, remoteChanges) {
				//
				// ... сохранять нужно, если такого изменения нет в локальной БД
				//
				bool needSave = !localChanges.contains(changeUuid);

				if (needSave) {
					changesForDownloadAndSave.append(changeUuid);
				}
			}
			//
			// ... скачиваем и сохраняем
			//
			downloadAndSaveScenarioData(changesForDownloadAndSave.join(";"));
		}
	}
}

void SynchronizationManagerV2::aboutWorkSyncData()
{
	if (isCanSync()) {
		//
		// Отправляем новые изменения
		//
		{
			//
			// Запоминаем время синхронизации изменений данных сценария
			//
			const QString prevDataSyncDatetime = m_lastDataSyncDatetime;
			m_lastDataSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

			//
			// Отправляем
			//
			QList<QString> newChanges =
					StorageFacade::databaseHistoryStorage()->history(prevDataSyncDatetime);
			const bool dataUploaded = uploadScenarioData(newChanges);

			//
			// Не обновляем время последней синхронизации, если данные не были отправлены
			//
			if (dataUploaded == false) {
				m_lastDataSyncDatetime = prevDataSyncDatetime;
			}
		}

		//
		// Загружаем и применяем изменения от других пользователей за последние LAST_MINUTES минут
		//
		{
			const int LAST_MINUTES = 2;

			m_loader->setRequestMethod(NetworkRequest::Post);
			m_loader->clearRequestAttributes();
			m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
			m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
			m_loader->addRequestAttribute(KEY_FROM_LAST_MINUTES, LAST_MINUTES);
			QByteArray response = loadSyncWrapper(URL_SCENARIO_DATA_LIST);

			//
			// ... считываем uuid'ы новых изменений
			//
			QList<QString> remoteChanges;
			QXmlStreamReader changesReader(response);
			while (!changesReader.atEnd()) {
				changesReader.readNext();
				if (changesReader.name().toString() == "status") {
					const bool success = changesReader.attributes().value("result").toString() == "true";
					if (success) {
						changesReader.readNextStartElement();
						changesReader.readNextStartElement(); // changes
						while (!changesReader.atEnd()) {
							changesReader.readNextStartElement();
							if (changesReader.name() == "change") {
								const QString changeUuid = changesReader.attributes().value("id").toString();
								if (!changeUuid.isEmpty()) {
									remoteChanges.append(changeUuid);
								}
							}
						}
					} else {
						handleError(response);
						break;
					}
				}
			}

			//
			// ... скачиваем все изменения, которых ещё нет
			//
			QStringList changesForDownload;
			foreach (const QString& changeUuid, remoteChanges) {
				//
				// ... сохранять нужно, если такого изменения нет
				//
				const bool needDownload =
						!DataStorageLayer::StorageFacade::databaseHistoryStorage()->contains(changeUuid);

				if (needDownload) {
					changesForDownload.append(changeUuid);
				}
			}
			//
			// ... скачиваем и сохраняем
			//
			downloadAndSaveScenarioData(changesForDownload.join(";"));
		}
	}
}

void SynchronizationManagerV2::aboutUpdateCursors(int _cursorPosition, bool _isDraft)
{
	if (isCanSync()) {
		//
		// Загрузим позиции курсоров
		//
		m_loader->setRequestMethod(NetworkRequest::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CURSOR_POSITION, _cursorPosition);
		m_loader->addRequestAttribute(KEY_SCENARIO_IS_DRAFT, _isDraft ? "1" : "0");
		QByteArray response = loadSyncWrapper(URL_SCENARIO_CURSORS);


		//
		// ... считываем данные о курсорах
		//
		QMap<QString, int> cleanCursors;
		QMap<QString, int> draftCursors;
		QXmlStreamReader cursorsReader(response);
		while (!cursorsReader.atEnd()) {
			cursorsReader.readNext();
			if (cursorsReader.name().toString() == "status") {
				const bool success = cursorsReader.attributes().value("result").toString() == "true";
				if (success) {
					cursorsReader.readNextStartElement();
					while (!cursorsReader.atEnd()
						   && cursorsReader.readNextStartElement()) {
						//
						// Курсоры
						//
						while (cursorsReader.name() == "cursors"
							   && cursorsReader.readNextStartElement()) {
							//
							// Считываем каждый курсор
							//
							while (cursorsReader.name() == "cursor") {
								const QString username = cursorsReader.attributes().value("username").toString();
								const int cursorPosition = cursorsReader.attributes().value("position").toInt();
								const bool isDraft = cursorsReader.attributes().value("is_draft").toInt();

								QMap<QString, int>& cursors = isDraft ? draftCursors : cleanCursors;
								cursors.insert(username, cursorPosition);

								//
								// ... переход к следующему курсору
								//
								cursorsReader.readNextStartElement();
								cursorsReader.readNextStartElement();
							}
						}
					}
				} else {
					handleError(response);
					break;
				}
			}
		}

		//
		// Уведомляем об обновлении курсоров
		//
		emit cursorsUpdated(cleanCursors);
		emit cursorsUpdated(draftCursors, IS_DRAFT);
	}
}

bool SynchronizationManagerV2::isOperationSucceed(QXmlStreamReader& _responseReader)
{
	while (!_responseReader.atEnd()) {
		_responseReader.readNext();
		if (_responseReader.name().toString() == "status") {
			//
			// Операция успешна
			//
			if (_responseReader.attributes().value("result").toString() == "true") {
				return true;
			} else {
				//
				// Попытаемся извлечь код ошибки
				//
				if (!_responseReader.attributes().hasAttribute("errorCode")) {
					//
					// Неизвестная ошибка
					//
					handleError(UNKNOWN_ERROR_STRING, UNKNOWN_ERROR_CODE);
					m_sessionKey.clear();
					return false;
				}

				int errorCode = _responseReader.attributes().value("errorCode").toInt();

				//
				// Попытаемся извлечь текст ошибки
				//
				QString errorText = UNKNOWN_ERROR_STRING;
				if (_responseReader.attributes().hasAttribute("error")) {
					errorText = _responseReader.attributes().value("error").toString();
				}

				//
				// Скажем про ошибку
				//
				handleError(errorText, errorCode);
				return false;
			}
		}
	}
	//
	// Ничего не нашли про статус. Скорее всего пропал интернет
	//
	handleError(tr("Can't estabilish network connection."), 0);
	return false;
}

void SynchronizationManagerV2::handleError(const QString &_error, int _code)
{
	emit syncClosedWithError(_code, _error);
}

QByteArray SynchronizationManagerV2::loadSyncWrapper(const QUrl& _url)
{
	QByteArray response;

	//
	// Если соединение активно, делаем запрос
	//
	if (m_isInternetConnectionActive) {
		response = m_loader->loadSync(_url);

		//
		// Если пропало соединение с интернетом, уведомляем об этом и запускаем процесс проверки связи
		//
		if (response.isEmpty()) {

			emit syncClosedWithError(OFFLINE_ERROR_CODE, tr("Can't estabilish network connection."));
			emit cursorsUpdated(QMap<QString, int>());
			emit cursorsUpdated(QMap<QString, int>(), IS_DRAFT);

			networkStateChanged(false);
		}
	}

	return response;
}

bool SynchronizationManagerV2::isCanSync() const
{
	return
			m_isInternetConnectionActive
			&& ProjectsManager::currentProject().isRemote()
			&& ProjectsManager::currentProject().isSyncAvailable()
			&& !m_sessionKey.isEmpty();
}

bool SynchronizationManagerV2::uploadScenarioChanges(const QList<QString>& _changesUuids)
{
	bool changesUploaded = false;

	if (isCanSync()
		&& !_changesUuids.isEmpty()) {
		//
		// Сформировать xml для отправки
		//
		QString changesXml;
		QXmlStreamWriter xmlWriter(&changesXml);
		xmlWriter.writeStartDocument();
		xmlWriter.writeStartElement("changes");
		foreach (const QString& changeUuid, _changesUuids) {
			const ScenarioChange change = StorageFacade::scenarioChangeStorage()->change(changeUuid);

			xmlWriter.writeStartElement("change");

			xmlWriter.writeTextElement(SCENARIO_CHANGE_ID, change.uuid().toString());

			xmlWriter.writeTextElement(SCENARIO_CHANGE_DATETIME, change.datetime().toString("yyyy-MM-dd hh:mm:ss"));

			xmlWriter.writeStartElement(SCENARIO_CHANGE_UNDO_PATCH);
			xmlWriter.writeCDATA(change.undoPatch());
			xmlWriter.writeEndElement();

			xmlWriter.writeStartElement(SCENARIO_CHANGE_REDO_PATCH);
			xmlWriter.writeCDATA(change.redoPatch());
			xmlWriter.writeEndElement();

			xmlWriter.writeTextElement(SCENARIO_CHANGE_IS_DRAFT, change.isDraft() ? "1" : "0");

			xmlWriter.writeEndElement(); // change
		}
		xmlWriter.writeEndElement(); // changes
		xmlWriter.writeEndDocument();

		//
		// Отправить данные
		//
		m_loader->setRequestMethod(NetworkRequest::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CHANGES, changesXml);
		const QByteArray response = loadSyncWrapper(URL_SCENARIO_CHANGE_SAVE);

		//
		// Изменения отправлены, если сервер это подтвердил
		//
		changesUploaded = !response.isEmpty();
	}

	return changesUploaded;
}

QList<QHash<QString, QString> > SynchronizationManagerV2::downloadScenarioChanges(const QString& _changesUuids)
{
	QList<QHash<QString, QString> > changes;

	if (isCanSync()
		&& !_changesUuids.isEmpty()) {
		//
		// ... загружаем изменения
		//
		m_loader->setRequestMethod(NetworkRequest::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CHANGES_IDS, _changesUuids);
		QByteArray response = loadSyncWrapper(URL_SCENARIO_CHANGE_LOAD);


		//
		// ... считываем данные об изменениях
		//
		QXmlStreamReader changesReader(response);
		while (!changesReader.atEnd()) {
			changesReader.readNext();
			if (changesReader.name().toString() == "status") {
				const bool success = changesReader.attributes().value("result").toString() == "true";
				if (success) {
					changesReader.readNextStartElement();
					while (!changesReader.atEnd()
						   && changesReader.readNextStartElement()) {
						//
						// Изменения
						//
						while (changesReader.name() == "changes"
							   && changesReader.readNextStartElement()) {
							//
							// Считываем каждое изменение
							//
							while (changesReader.name() == "change"
								   && changesReader.readNextStartElement()) {
								//
								// Данные изменения
								//
								QHash<QString, QString> change;
								while (changesReader.name() != "change") {
									const QString key = changesReader.name().toString();
									const QString value = changesReader.readElementText();
									if (!value.isEmpty()) {
										change.insert(key, value);
									}

									//
									// ... переходим к следующему элементу
									//
									changesReader.readNextStartElement();
								}

								if (!change.isEmpty()) {
									changes.append(change);
								}
							}
						}
					}
				} else {
					handleError(response);
					break;
				}
			}
		}
	}

	return changes;
}

bool SynchronizationManagerV2::uploadScenarioData(const QList<QString>& _dataUuids)
{
	bool dataUploaded = false;

	if (isCanSync()
		&& !_dataUuids.isEmpty()) {
		//
		// Сформировать xml для отправки
		//
		QString dataChangesXml;
		QXmlStreamWriter xmlWriter(&dataChangesXml);
		xmlWriter.writeStartDocument();
		xmlWriter.writeStartElement("changes");
		int order = 0;
		foreach (const QString& dataUuid, _dataUuids) {
			const QMap<QString, QString> historyRecord =
					StorageFacade::databaseHistoryStorage()->historyRecord(dataUuid);

			//
			// NOTE: Вынесено на уровень AbstractMapper::executeSql
			// Нас интересуют изменения из всех таблиц, кроме сценария и истории изменений сценария,
			// они синхронизируются самостоятельно
			//

			xmlWriter.writeStartElement("change");
			//
			xmlWriter.writeTextElement(DBH_ID_KEY, historyRecord.value(DBH_ID_KEY));
			//
			xmlWriter.writeStartElement(DBH_QUERY_KEY);
			xmlWriter.writeCDATA(historyRecord.value(DBH_QUERY_KEY));
			xmlWriter.writeEndElement();
			//
			xmlWriter.writeStartElement(DBH_QUERY_VALUES_KEY);
			xmlWriter.writeCDATA(historyRecord.value(DBH_QUERY_VALUES_KEY));
			xmlWriter.writeEndElement();
			//
			xmlWriter.writeTextElement(DBH_DATETIME_KEY, historyRecord.value(DBH_DATETIME_KEY));
			//
			xmlWriter.writeTextElement(DBH_ORDER_KEY, QString::number(order++));
			//
			xmlWriter.writeEndElement(); // change
		}
		xmlWriter.writeEndElement(); // changes
		xmlWriter.writeEndDocument();

		//
		// Отправить данные
		//
		m_loader->setRequestMethod(NetworkRequest::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CHANGES, dataChangesXml);
		//
		// NOTE: При отправке большого объёма данных возможно зависание
		//
		const QByteArray response = loadSyncWrapper(URL_SCENARIO_DATA_SAVE);

		//
		// Данные отправлены, если сервер это подтвердил
		//
		dataUploaded = !response.isEmpty();
	}

	return dataUploaded;
}

void SynchronizationManagerV2::downloadAndSaveScenarioData(const QString& _dataUuids)
{
	if (isCanSync()
		&& !_dataUuids.isEmpty()) {
		//
		// ... загружаем изменения
		//
		m_loader->setRequestMethod(NetworkRequest::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CHANGES_IDS, _dataUuids);
		QByteArray response = loadSyncWrapper(URL_SCENARIO_DATA_LOAD);


		//
		// ... считываем данные об изменениях
		//
		QList<QHash<QString, QString> > changes;
		QXmlStreamReader changesReader(response);
		while (!changesReader.atEnd()) {
			changesReader.readNext();
			if (changesReader.name().toString() == "status") {
				const bool success = changesReader.attributes().value("result").toString() == "true";
				if (success) {
					changesReader.readNextStartElement();
					while (!changesReader.atEnd()
						   && changesReader.readNextStartElement()) {
						//
						// Изменения
						//
						while (changesReader.name() == "changes"
							   && changesReader.readNextStartElement()) {
							//
							// Считываем каждое изменение
							//
							while (changesReader.name() == "change"
								   && changesReader.readNextStartElement()) {
								//
								// Данные изменения
								//
								QHash<QString, QString> change;
								while (changesReader.name() != "change") {
									const QString key = changesReader.name().toString();
									const QString value = changesReader.readElementText();
									if (!value.isEmpty()) {
										change.insert(key, value);
									}

									//
									// ... переходим к следующему элементу
									//
									changesReader.readNextStartElement();
								}

								if (!change.isEmpty()) {
									changes.append(change);
								}
							}
						}
					}
				} else {
					handleError(response);
					break;
				}
			}
		}


		//
		// Применяем изменения
		//
		QHash<QString, QString> changeValues;
		DatabaseLayer::Database::transaction();
		foreach (changeValues, changes) {
			DataStorageLayer::StorageFacade::databaseHistoryStorage()->storeAndApplyHistoryRecord(
				changeValues.value(DBH_ID_KEY), changeValues.value(DBH_QUERY_KEY),
				changeValues.value(DBH_QUERY_VALUES_KEY), changeValues.value(DBH_DATETIME_KEY));
		}
		DatabaseLayer::Database::commit();


		//
		// Обновляем данные
		//
		DataStorageLayer::StorageFacade::refreshStorages();
	}
}

void SynchronizationManagerV2::networkStateChanged(bool _state)
{
	//
	// Запомним состояние интернета и кинем соответствующий сигнал
	//
	m_isInternetConnectionActive = _state;
	if (m_isInternetConnectionActive) {
		//
		// Переавторизуемся, раз интернет появился
		//
		//autoLogin();
	}

	emit networkStatusChanged(_state);
}

void SynchronizationManagerV2::initConnections()
{
	connect(this, &SynchronizationManagerV2::loginAccepted, this, &SynchronizationManagerV2::loadProjects);
	connect(m_networkManager, &QNetworkConfigurationManager::onlineStateChanged, this, &SynchronizationManagerV2::networkStateChanged);
}

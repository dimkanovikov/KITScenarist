#include "SynchronizationManager.h"

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

#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <WebLoader.h>

#include <QDebug>
#include <QEventLoop>
#include <QHash>
#include <QScopedPointer>
#include <QTimer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

using ManagementLayer::SynchronizationManager;
using ManagementLayer::ProjectsManager;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;

namespace {
	/**
	 * @brief Ссылки для запросов
	 */
	/** @{ */
	const QUrl URL_LOGIN = QUrl("https://kitscenarist.ru/api/account/login/");
	const QUrl URL_LOGOUT = QUrl("https://kitscenarist.ru/api/account/logout/");
	const QUrl URL_PROJECTS = QUrl("https://kitscenarist.ru/api/projects/");


	const QUrl URL_SCENARIO_LOAD = QUrl("https://kitscenarist.ru/api/projects/scenario/");
	const QUrl URL_SCENARIO_SAVE = QUrl("https://kitscenarist.ru/api/projects/scenario/save/");

	const QUrl URL_SCENARIO_CHANGE_LIST = QUrl("https://kitscenarist.ru/api/projects/scenario/change/list/");
	const QUrl URL_SCENARIO_CHANGE_LOAD = QUrl("https://kitscenarist.ru/api/projects/scenario/change/");
	const QUrl URL_SCENARIO_CHANGE_SAVE = QUrl("https://kitscenarist.ru/api/projects/scenario/change/save/");
	const QUrl URL_SCENARIO_CURSORS = QUrl("https://kitscenarist.ru/api/projects/scenario/cursor/");

	const QUrl URL_SCENARIO_DATA_LIST = QUrl("https://kitscenarist.ru/api/projects/data/list/");
	const QUrl URL_SCENARIO_DATA_LOAD = QUrl("https://kitscenarist.ru/api/projects/data/");
	const QUrl URL_SCENARIO_DATA_SAVE = QUrl("https://kitscenarist.ru/api/projects/data/save/");
	/** @} */

	/**
	 * @brief Ключи для параметров запросов
	 */
	/** @{ */
	const QString KEY_USER_NAME = "login";
	const QString KEY_PASSWORD = "password";
	const QString KEY_SESSION_KEY = "session_key";
	const QString KEY_PROJECT = "project_id";
	const QString KEY_CHANGES = "changes";
	const QString KEY_CHANGES_IDS = "changes_ids";
	const QString KEY_SCENARIO_ID = "scenario_id";
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
	/** @} */

	const bool IS_DRAFT = true;
	const bool IS_ASYNC = true;
	const bool IS_SYNC = false;
}


SynchronizationManager::SynchronizationManager(QObject* _parent, QWidget* _parentView) :
	QObject(_parent),
	m_view(_parentView),
	m_loader(new WebLoader(this))
{
	initConnections();
}

void SynchronizationManager::login()
{
	//
	// Авторизуемся, если параметры сохранены
	//
	const QString login =
			PasswordStorage::load(
				StorageFacade::settingsStorage()->value(
					"application/user-name",
					SettingsStorage::ApplicationSettings)
				);
	const QString password =
			PasswordStorage::load(
				StorageFacade::settingsStorage()->value(
					"application/password",
					SettingsStorage::ApplicationSettings),
				login
				);

	if (!login.isEmpty() && !password.isEmpty()) {
		aboutLogin(login, password);
	}
}

void SynchronizationManager::aboutLogin(const QString& _userName, const QString& _password)
{
	//
	// Информация для пользователя
	//
	ProgressWidget progress(m_view);
	progress.showProgress(tr("Authorizing"), tr("Checking correction of user name and password."));

	//
	// Авторизуемся
	//
	m_loader->setRequestMethod(WebLoader::Post);
	m_loader->clearRequestAttributes();
	m_loader->addRequestAttribute(KEY_USER_NAME, _userName);
	m_loader->addRequestAttribute(KEY_PASSWORD, _password);
	QByteArray response = m_loader->loadSync(URL_LOGIN);

	//
	// Считываем результат авторизации
	//
	QXmlStreamReader responseReader(response);
	//
	// Успешно ли завершилась авторизация
	//
	bool success = false;
	QString sessionKey;
	QString errorMessage;
	while (!responseReader.atEnd()) {
		responseReader.readNext();
		if (responseReader.name().toString() == "status") {
			success = responseReader.attributes().value("result").toString() == "true";
			//
			// Если авторизация успешна, сохраняем ключ сессии
			//
			if (success) {
				responseReader.readNext(); // закрытие тэга status
				responseReader.readNext(); // тэг session_key
				responseReader.readNext(); // содержимое тэга session_key
				sessionKey = responseReader.text().toString().simplified();
				break;
			}
			//
			// В противном случае считываем сообщение об ошибке
			//
			else {
				errorMessage = responseReader.attributes().value("error").toString();
				break;
			}
		}
	}

	//
	// Закрываем уведомление для пользователя
	//
	sleepALittle();
	progress.finish();

	//
	// Если авторизация прошла
	//
	if (success) {
		//
		// ... сохраняем информацию о сессии
		//
		m_sessionKey = sessionKey;
		//
		// ... и о пользователе
		//
		StorageFacade::settingsStorage()->setValue(
			"application/user-name",
			PasswordStorage::save(_userName),
			SettingsStorage::ApplicationSettings);
		StorageFacade::settingsStorage()->setValue(
			"application/password",
			PasswordStorage::save(_password, _userName),
			SettingsStorage::ApplicationSettings);

		emit loginAccepted(_userName);
	} else {
		emit loginNotAccepted(_userName, _password, errorMessage);
	}
}

void SynchronizationManager::aboutLogout()
{
	if (!m_sessionKey.isEmpty()) {
		//
		// Информация для пользователя
		//
		ProgressWidget progress(m_view);
		progress.showProgress(tr("Authorizing"), tr("Close working session."));

		//
		// Закрываем авторизацию
		//
		m_loader->setRequestMethod(WebLoader::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->loadSync(URL_LOGOUT);

		//
		// Удаляем сохранённые значения, если они были
		//
		StorageFacade::settingsStorage()->setValue(
			"application/user-name",
			QString::null,
			SettingsStorage::ApplicationSettings);
		StorageFacade::settingsStorage()->setValue(
			"application/password",
			QString::null,
			SettingsStorage::ApplicationSettings);

		//
		// Закрываем уведомление для пользователя
		//
		sleepALittle();
		progress.finish();

		emit logoutAccepted();
	}
}

void SynchronizationManager::aboutLoadProjects()
{
	if (!m_sessionKey.isEmpty()) {
		//
		// Информация для пользователя
		//
		ProgressWidget progress(m_view);
		progress.showProgress(tr("Loading"), tr("Loading projects list from remote server."));

		//
		// Получаем список проектов
		//
		m_loader->setRequestMethod(WebLoader::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		QByteArray response = m_loader->loadSync(URL_PROJECTS);

		//
		// Считываем результат
		//
		QXmlStreamReader responseReader(response);
		//
		// Успешно ли завершилось получение
		//
		bool success = false;
		QString errorMessage;
		while (!responseReader.atEnd()) {
			responseReader.readNext();
			if (responseReader.name().toString() == "status") {
				success = responseReader.attributes().value("result").toString() == "true";

				//
				// Ни чего не делаем, отправим проекты в конце функции с сигналом
				//
				if (success) {
					break;
				}
				//
				// Если получить список проектов не удалось, считываем сообщение об ошибке
				//
				else {
					errorMessage = responseReader.attributes().value("error").toString();
					break;
				}
			}
		}

		//
		// Если проекты получены
		//
		if (success) {
			//
			// Сохраняем список проектов для работы в автономном режиме
			//
			StorageFacade::settingsStorage()->setValue(
						"application/remote-projects", response.toBase64(), SettingsStorage::ApplicationSettings);

			emit remoteProjectsLoaded(response);
		} else {
			emit remoteProjectsNotLoaded(errorMessage);
		}

		//
		// Закрываем уведомление для пользователя
		//
		sleepALittle();
		progress.finish();
	}
}

void SynchronizationManager::aboutFullSyncScenario()
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
		m_loader->setRequestMethod(WebLoader::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		QByteArray response = m_loader->loadSync(URL_SCENARIO_CHANGE_LIST);

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
			QHash<QString, QString> change;
			foreach (change, changes) {
				if (!change.isEmpty()) {
					//
					// ... сохраняем
					//
					DataStorageLayer::StorageFacade::scenarioChangeStorage()->append(
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

void SynchronizationManager::aboutWorkSyncScenario()
{
	if (isCanSync()) {
		//
		// Запоминаем время синхронизации изменений сценария
		//
		const QString prevChangesSyncDatetime = m_lastChangesSyncDatetime;
		m_lastChangesSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

		//
		// Отправляем новые изменения
		//
		{
			QList<QString> newChanges =
					StorageFacade::scenarioChangeStorage()->newUuids(prevChangesSyncDatetime);
			uploadScenarioChanges(newChanges);
		}

		//
		// Загружаем и применяем изменения от других пользователей за последние LAST_MINUTES минут
		//
		{
			const int LAST_MINUTES = 2;

			m_loader->setRequestMethod(WebLoader::Post);
			m_loader->clearRequestAttributes();
			m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
			m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
			m_loader->addRequestAttribute(KEY_FROM_LAST_MINUTES, LAST_MINUTES);
			QByteArray response = m_loader->loadSync(URL_SCENARIO_CHANGE_LIST);

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

void SynchronizationManager::aboutUpdateCursors(int _cursorPosition, bool _isDraft)
{
	if (isCanSync()) {
		//
		// Загрузим позиции курсоров
		//
		m_loader->setRequestMethod(WebLoader::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CURSOR_POSITION, _cursorPosition);
		m_loader->addRequestAttribute(KEY_SCENARIO_IS_DRAFT, _isDraft ? "1" : "0");
		QByteArray response = m_loader->loadSync(URL_SCENARIO_CURSORS);


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

void SynchronizationManager::aboutFullSyncData()
{
	//
	// Запоминаем время синхронизации данных, в дальнейшем будем отправлять изменения
	// произведённые с данного момента
	//
	m_lastDataSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

	//
	// Получить список всех изменений данных на сервере
	//
	WebLoader loader;
	loader.setRequestMethod(WebLoader::Post);
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

void SynchronizationManager::aboutWorkSyncData()
{
	if (isCanSync()) {
		//
		// Запоминаем время синхронизации изменений данных сценария
		//
		const QString prevDataSyncDatetime = m_lastDataSyncDatetime;
		m_lastDataSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

		//
		// Отправляем новые изменения
		//
		{
			QList<QString> newChanges =
					StorageFacade::databaseHistoryStorage()->history(prevDataSyncDatetime);
			uploadScenarioData(newChanges);
		}

		//
		// Загружаем и применяем изменения от других пользователей за последние LAST_MINUTES минут
		//
		{
			const int LAST_MINUTES = 2;

			m_loader->setRequestMethod(WebLoader::Post);
			m_loader->clearRequestAttributes();
			m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
			m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
			m_loader->addRequestAttribute(KEY_FROM_LAST_MINUTES, LAST_MINUTES);
			QByteArray response = m_loader->loadSync(URL_SCENARIO_DATA_LIST);

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

bool SynchronizationManager::isCanSync() const
{
	return
			ProjectsManager::currentProject().isRemote()
			&& !m_sessionKey.isEmpty();
}

void SynchronizationManager::uploadScenarioChanges(const QList<QString>& _changesUuids)
{
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
		m_loader->setRequestMethod(WebLoader::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CHANGES, changesXml);
		m_loader->loadSync(URL_SCENARIO_CHANGE_SAVE);
	}
}

QList<QHash<QString, QString> > SynchronizationManager::downloadScenarioChanges(const QString& _changesUuids)
{
	QList<QHash<QString, QString> > changes;

	if (isCanSync()
		&& !_changesUuids.isEmpty()) {
		//
		// ... загружаем изменения
		//
		m_loader->setRequestMethod(WebLoader::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CHANGES_IDS, _changesUuids);
		QByteArray response = m_loader->loadSync(URL_SCENARIO_CHANGE_LOAD);


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
				}
			}
		}
	}

	return changes;
}

void SynchronizationManager::uploadScenarioData(const QList<QString>& _dataUuids)
{
	if (isCanSync()
		&& !_dataUuids.isEmpty()) {
		//
		// Сформировать xml для отправки
		//
		QString dataChangesXml;
		QXmlStreamWriter xmlWriter(&dataChangesXml);
		xmlWriter.writeStartDocument();
		xmlWriter.writeStartElement("changes");
		foreach (const QString& dataUuid, _dataUuids) {
			const QMap<QString, QString> historyRecord =
					StorageFacade::databaseHistoryStorage()->historyRecord(dataUuid);

			//
			// Нас интересуют изменения из всех таблиц, кроме сценария и истории изменений сценария,
			// они синхронизируются самостоятельно
			//
			const QString query = historyRecord.value(DBH_QUERY_KEY);
			if (!query.contains(" scenario_changes ")
				&& !query.contains(" scenario ")) {
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
				xmlWriter.writeEndElement(); // change
			}
		}
		xmlWriter.writeEndElement(); // changes
		xmlWriter.writeEndDocument();

		//
		// Отправить данные
		//
		m_loader->setRequestMethod(WebLoader::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CHANGES, dataChangesXml);
		//
		// NOTE: При отправке большого объёма данных возможно зависание
		//
		m_loader->loadSync(URL_SCENARIO_DATA_SAVE);
	}
}

void SynchronizationManager::downloadAndSaveScenarioData(const QString& _dataUuids)
{

	if (isCanSync()
		&& !_dataUuids.isEmpty()) {
		//
		// ... загружаем изменения
		//
		m_loader->setRequestMethod(WebLoader::Post);
		m_loader->clearRequestAttributes();
		m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		m_loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		m_loader->addRequestAttribute(KEY_CHANGES_IDS, _dataUuids);
		QByteArray response = m_loader->loadSync(URL_SCENARIO_DATA_LOAD);


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



























//void SynchronizationManager::aboutSaveDataToServer()
//{
//	if (!m_sessionKey.isEmpty()) {
//		//
//		// Получить данные, которых на сервере ещё нет
//		//
//		QList<QMap<QString, QString> > databaseHistory =
//				StorageFacade::databaseHistoryStorage()->history(m_lastDataSyncDatetime);

//		//
//		// Сформировать xml для отправки
//		//
//		QString dataChangesXml;
//		QXmlStreamWriter xmlWriter(&dataChangesXml);
//		xmlWriter.writeStartDocument();
//		xmlWriter.writeStartElement("changes");
//		QMap<QString, QString> historyRecord;
//		foreach (historyRecord, databaseHistory) {
//			//
//			// Отправляем изменения из всех таблиц, кроме сценария
//			//
//			if (!historyRecord.value(DBH_QUERY_KEY).contains(" scenario ")) {
//				//
//				xmlWriter.writeStartElement("change");
//				//
//				xmlWriter.writeTextElement(DBH_ID_KEY, historyRecord.value(DBH_ID_KEY));
//				//
//				xmlWriter.writeStartElement(DBH_QUERY_KEY);
//				xmlWriter.writeCDATA(historyRecord.value(DBH_QUERY_KEY));
//				xmlWriter.writeEndElement();
//				//
//				xmlWriter.writeStartElement(DBH_QUERY_VALUES_KEY);
//				xmlWriter.writeCDATA(historyRecord.value(DBH_QUERY_VALUES_KEY));
//				xmlWriter.writeEndElement();
//				//
//				xmlWriter.writeTextElement(DBH_DATETIME_KEY, historyRecord.value(DBH_DATETIME_KEY));
//				//
//				xmlWriter.writeEndElement(); // change

//				//
//				// Обновляем дату и время последней синхронизации изменений
//				//
//				m_lastDataSyncDatetime = historyRecord.value(DBH_DATETIME_KEY);
//			}
//		}
//		xmlWriter.writeEndElement(); // changes
//		xmlWriter.writeEndDocument();

//		//
//		// Отправить данные
//		//
//		WebLoader* loader = new WebLoader;
//		connect(loader, SIGNAL(finished()), loader, SLOT(deleteLater()));

//		loader->setRequestMethod(WebLoader::Post);
//		loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
//		loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
//		loader->addRequestAttribute(KEY_CHANGES, dataChangesXml);
//		loader->loadAsync(URL_SCENARIO_DATA_SAVE);
//	}
//}

//void SynchronizationManager::aboutSaveDataToServer(const QString& _changeUuid)
//{
//	if (!m_sessionKey.isEmpty()) {
//		//
//		// Получить данные для сохранения
//		//
//		QMap<QString, QString> historyRecord =
//				StorageFacade::databaseHistoryStorage()->historyRecord(_changeUuid);

//		//
//		// Отправить данные
//		//
//		{
//			//
//			// Сформировать xml для отправки
//			//
//			QString dataChangesXml;
//			QXmlStreamWriter xmlWriter(&dataChangesXml);
//			xmlWriter.writeStartDocument();
//			xmlWriter.writeStartElement("changes");
//			xmlWriter.writeStartElement("change");
//			//
//			xmlWriter.writeTextElement(DBH_ID_KEY, historyRecord.value(DBH_ID_KEY));
//			//
//			xmlWriter.writeStartElement(DBH_QUERY_KEY);
//			xmlWriter.writeCDATA(historyRecord.value(DBH_QUERY_KEY));
//			xmlWriter.writeEndElement();
//			//
//			xmlWriter.writeStartElement(DBH_QUERY_VALUES_KEY);
//			xmlWriter.writeCDATA(historyRecord.value(DBH_QUERY_VALUES_KEY));
//			xmlWriter.writeEndElement();
//			//
//			xmlWriter.writeTextElement(DBH_DATETIME_KEY, historyRecord.value(DBH_DATETIME_KEY));
//			//
//			xmlWriter.writeEndElement(); // change
//			xmlWriter.writeEndElement(); // changes
//			xmlWriter.writeEndDocument();

//			//
//			// Отправить данные
//			//
//			WebLoader* loader = new WebLoader;
//			connect(loader, SIGNAL(finished()), loader, SLOT(deleteLater()));

//			loader->setRequestMethod(WebLoader::Post);
//			loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
//			loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
//			loader->addRequestAttribute(KEY_CHANGES, dataChangesXml);
//			loader->loadAsync(URL_SCENARIO_DATA_SAVE);
//		}
//	}
//}

//void SynchronizationManager::aboutSaveDataToDB(const QString& _changeUuid)
//{
//	if (!_changeUuid.isEmpty()) {
//		//
//		// ... загружаем данные
//		//
//		WebLoader loader;
//		loader.setRequestMethod(WebLoader::Post);
//		loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
//		loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
//		loader.addRequestAttribute(KEY_SCENARIO_CHANGE_ID, _changeUuid);
//		QByteArray response = loader.loadSync(URL_SCENARIO_DATA_LOAD);

//		//
//		// ... считываем данные об изменении
//		//
//		QXmlStreamReader changeReader(response);
//		QHash<QString, QString> changeValues;
//		while (!changeReader.atEnd()) {
//			changeReader.readNext();
//			if (changeReader.name().toString() == "status") {
//				const bool success = changeReader.attributes().value("result").toString() == "true";
//				if (success) {
//					changeReader.readNextStartElement();
//					changeReader.readNextStartElement();
//					while (!changeReader.atEnd()) {
//						changeReader.readNextStartElement();
//						const QString key = changeReader.name().toString();
//						const QString value = changeReader.readElementText();
//						if (!value.isEmpty()) {
//							changeValues.insert(key, value);
//						}
//					}
//				}
//			}
//		}

//		//
//		// ... сохраняем
//		//
//		DataStorageLayer::StorageFacade::databaseHistoryStorage()->storeAndApplyHistoryRecord(
//			changeValues.value(DBH_ID_KEY), changeValues.value(DBH_QUERY_KEY),
//			changeValues.value(DBH_QUERY_VALUES_KEY), changeValues.value(DBH_DATETIME_KEY));
//	}
//}

void SynchronizationManager::initConnections()
{
	connect(this, SIGNAL(loginAccepted(QString)), this, SLOT(aboutLoadProjects()));
}

void SynchronizationManager::sleepALittle()
{
	QEventLoop loop;
	QTimer::singleShot(10, &loop, SLOT(quit()));
	loop.exec();
}

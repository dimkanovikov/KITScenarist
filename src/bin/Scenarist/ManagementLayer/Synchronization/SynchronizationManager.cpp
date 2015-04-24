#include "SynchronizationManager.h"

#include <ManagementLayer/Project/ProjectsManager.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/DatabaseHistoryStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>

#include <DataLayer/Database/Database.h>

#include <Domain/Scenario.h>

#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>
#include <3rd_party/Helpers/DiffMatchPatch.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <WebLoader.h>

#include <QEventLoop>
#include <QHash>
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
	const QUrl URL_SCENARIO_VERSIONS = QUrl("https://kitscenarist.ru/api/projects/scenario/list/");
	const QUrl URL_SCENARIO_SAVE = QUrl("https://kitscenarist.ru/api/projects/scenario/save/");
	const QUrl URL_SCENARIO_DATA_LOAD = QUrl("https://kitscenarist.ru/api/projects/data/");
	const QUrl URL_SCENARIO_DATA_LIST = QUrl("https://kitscenarist.ru/api/projects/data/list/");
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
	const QString KEY_SCENARIO_ID = "scenario_id";
	const QString KEY_SCENARIO_IS_DRAFT = "scenario_is_draft";
	const QString KEY_SCENARIO_CHANGE_ID = "change_id";
	/** @{ */

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

	/**
	 * @brief Объединить тексты
	 */
	static QString mergeTexts(const QString& _lastSyncedVersion, const QString& _currentVersion,
		const QString& _currentRemoteVersion) {
		//
		// Преобразуем и обрабатываем строки
		//
		std::wstring currentRemoteVersion = _currentRemoteVersion.toStdWString();
		std::wstring currentVersion = _currentVersion.isEmpty() ? currentRemoteVersion : _currentVersion.toStdWString();
		std::wstring lastSyncedVersion = _lastSyncedVersion.isEmpty() ? currentRemoteVersion : _lastSyncedVersion.toStdWString();
		//
		// Объединяем текст версий
		//
		diff_match_patch<std::wstring> dmp;
		std::wstring strPatch = dmp.patch_toText(dmp.patch_make(lastSyncedVersion, currentVersion));
		std::pair<std::wstring, std::vector<bool> > out = dmp.patch_apply(dmp.patch_fromText(strPatch), currentRemoteVersion);
		//
		return QString::fromStdWString(out.first);
	}
}


SynchronizationManager::SynchronizationManager(QObject* _parent, QWidget* _parentView) :
	QObject(_parent),
	m_view(_parentView)
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
	WebLoader loader;
	loader.setRequestMethod(WebLoader::Post);
	loader.addRequestAttribute(KEY_USER_NAME, _userName);
	loader.addRequestAttribute(KEY_PASSWORD, _password);
	QByteArray response = loader.loadSync(URL_LOGIN);

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
		WebLoader loader;
		loader.setRequestMethod(WebLoader::Post);
		loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		loader.loadSync(URL_LOGOUT);

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
	//
	// Информация для пользователя
	//
	ProgressWidget progress(m_view);
	progress.showProgress(tr("Loading"), tr("Loading projects list from remote server."));

	//
	// Получаем список проектов
	//
	WebLoader loader;
	loader.setRequestMethod(WebLoader::Post);
	loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
	QByteArray response = loader.loadSync(URL_PROJECTS);

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

//void SynchronizationManager::aboutSyncScenario(bool _isDraft)
//{
//	//
//	// Скачать последнюю версию сценария и актуализировать её
//	//
//	aboutSaveScenarioToDB(QString::null, _isDraft);

//	//
//	// Синхронизация всех остальных версий сценария
//	//
//	{
//		//
//		// Получить список версий сценария на сервере
//		//
//		WebLoader loader;
//		loader.setRequestMethod(WebLoader::Post);
//		loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
//		loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
//		QByteArray response = loader.loadSync(URL_SCENARIO_VERSIONS);
//		//
//		// ... считываем версии (uuid, время конца сессии)
//		//
//		QHash<QString, QString> remoteVersions;
//		QXmlStreamReader scenarioReader(response);
//		while (!scenarioReader.atEnd()) {
//			scenarioReader.readNext();
//			if (scenarioReader.name().toString() == "status") {
//				const bool success = scenarioReader.attributes().value("result").toString() == "true";
//				if (success) {
//					scenarioReader.readNextStartElement();
//					scenarioReader.readNextStartElement(); // versions
//					while (!scenarioReader.atEnd()) {
//						scenarioReader.readNextStartElement();
//						if (scenarioReader.name() == "version") {
//							const QString versionUuid = scenarioReader.attributes().value("id").toString();
//							const QString versionDatetime = scenarioReader.attributes().value("version_end_datetime").toString();
//							remoteVersions.insert(versionUuid, versionDatetime);
//						}
//					}
//				}
//			}
//		}

//		//
//		// Сформируем список версий сценария хранящихся локально
//		//
//		QHash<QString, Scenario*> localVersions;
//		foreach (DomainObject* domainObject, DataStorageLayer::StorageFacade::scenarioStorage()->all()->toList()) {
//			if (Scenario* scenario = dynamic_cast<Scenario*>(domainObject)) {
//				localVersions.insert(scenario->uuid(), scenario);
//			}
//		}

//		//
//		// Отправить на сайт все версии, которых на сайте нет, включая текущую объединённую
//		//
//		foreach (const QString& versionUuid, localVersions.keys()) {
//			//
//			// ... отправлять нужно, если такой версии нет на сайте
//			//
//			bool needSend = !remoteVersions.contains(versionUuid);
//			if (needSend == false) {
//				//
//				// ... или если её дата изменения меньше даты изменения её локальной копии
//				//
//				Scenario* scenario = localVersions.value(versionUuid);
//				const QString scenarioVersionEndDatetime = scenario->versionEndDatetime().toString("yyyy-MM-dd hh:mm:ss");
//				needSend = remoteVersions.value(versionUuid) < scenarioVersionEndDatetime;
//			}

//			//
//			// ... отправляем
//			//
//			if (needSend) {
//				aboutSaveScenarioToServer(localVersions.value(versionUuid), IS_SYNC);
//			}
//		}

//		//
//		// Сохранить в локальной БД все версии, которых в ней нет
//		//
//		DatabaseLayer::Database::transaction();
//		foreach (const QString& versionUuid, remoteVersions.keys()) {
//			//
//			// ... сохранять нужно, если такой версии нет в локальной БД
//			//
//			bool needSave = !localVersions.contains(versionUuid);
//			if (needSave == false) {
//				//
//				// ... или если её дата изменения больше даты изменения её локальной копии
//				//
//				Scenario* scenario = localVersions.value(versionUuid);
//				const QString scenarioVersionEndDatetime = scenario->versionEndDatetime().toString("yyyy-MM-dd hh:mm:ss");
//				needSave = remoteVersions.value(versionUuid) > scenarioVersionEndDatetime;
//			}

//			//
//			// ... сохраняем
//			//
//			if (needSave) {
//				aboutSaveScenarioToDB(versionUuid, _isDraft);
//			}
//		}
//		DatabaseLayer::Database::commit();
//	}
//}

//void SynchronizationManager::aboutSyncData()
//{
//	//
//	// Получить список всех изменений данных на сервере
//	//
//	WebLoader loader;
//	loader.setRequestMethod(WebLoader::Post);
//	loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
//	loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
//	QByteArray response = loader.loadSync(URL_SCENARIO_DATA_LIST);
//	//
//	// ... считываем изменения (uuid)
//	//
//	QList<QString> remoteChanges;
//	QXmlStreamReader changesReader(response);
//	while (!changesReader.atEnd()) {
//		changesReader.readNext();
//		if (changesReader.name().toString() == "status") {
//			const bool success = changesReader.attributes().value("result").toString() == "true";
//			if (success) {
//				changesReader.readNextStartElement();
//				changesReader.readNextStartElement(); // changes
//				while (!changesReader.atEnd()) {
//					changesReader.readNextStartElement();
//					if (changesReader.name() == "change") {
//						const QString changeUuid = changesReader.attributes().value("id").toString();
//						remoteChanges.append(changeUuid);
//					}
//				}
//			}
//		}
//	}

//	//
//	// Сформируем список изменений сценария хранящихся локально
//	//
//	QList<QString> localChanges;
//	QMap<QString, QString> historyRecord;
//	foreach (historyRecord, StorageFacade::databaseHistoryStorage()->history(QString::null)) {
//		//
//		// Нас интересуют изменения из всех таблиц, кроме сценария
//		//
//		if (!historyRecord.value(DBH_QUERY_KEY).contains(" scenario ")) {
//			localChanges.append(historyRecord.value(DBH_ID_KEY));
//		}
//	}

//	//
//	// Отправить на сайт все версии, которых на сайте нет
//	//
//	foreach (const QString& changeUuid, localChanges) {
//		//
//		// ... отправлять нужно, если такого изменения нет на сайте
//		//
//		bool needSend = !remoteChanges.contains(changeUuid);

//		//
//		// ... отправляем
//		//
//		if (needSend) {
//			aboutSaveDataToServer(changeUuid);
//		}
//	}

//	//
//	// Сохранить в локальной БД все изменения, которых в ней нет
//	//
//	DatabaseLayer::Database::transaction();
//	foreach (const QString& changeUuid, remoteChanges) {
//		//
//		// ... сохранять нужно, если такого изменения нет в локальной БД
//		//
//		bool needSave = !localChanges.contains(changeUuid);

//		//
//		// ... сохраняем
//		//
//		if (needSave) {
//			aboutSaveDataToDB(changeUuid);
//		}
//	}
//	DatabaseLayer::Database::commit();

//	//
//	// Запоминаем время синхронизации данных, в дальнейшем будем отправлять изменения
//	// произведённые с данного момента
//	//
//	m_lastDataSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");
//}

//void SynchronizationManager::aboutUpdateScenario(bool _isDraft)
//{
//	Scenario* current = DataStorageLayer::StorageFacade::scenarioStorage()->current(_isDraft);
//	if (current != 0) {
//		aboutUpdateScenario(current->name(), current->synopsis(), current->text(), _isDraft);
//	}
//}

//void SynchronizationManager::aboutUpdateScenario(const QString& _name, const QString& _synopsis,
//	const QString& _text, bool _isDraft)
//{
//	//
//	// Получить последнюю версию с сервера
//	//
//	const QHash<QString, QString> scenarioValues = aboutLoadScenario(QString::null, _isDraft);
//	const QString remoteName = scenarioValues.value("name");
//	const QString remoteSynopsis = scenarioValues.value("synopsis");
//	const QString remoteText = scenarioValues.value("text");

//	//
//	// Если между текущей версией и версией с сервера есть отличия
//	//
//	if (remoteName != _name
//		|| remoteSynopsis != _synopsis
//		|| remoteText != _text) {
//		//
//		// Объеденить текущий текст с версией с сервера
//		//
//		const QString mergedName = ::mergeTexts(lastSyncedName(_isDraft), _name, remoteName);
//		const QString mergedSynopsis = ::mergeTexts(lastSyncedSynopsis(_isDraft), _synopsis, remoteSynopsis);
//		const QString mergedText = ::mergeTexts(lastSyncedText(_isDraft), _text, remoteText);

//		Scenario* currentScenario = DataStorageLayer::StorageFacade::scenarioStorage()->current(_isDraft);
//		if (currentScenario == 0) {
//			currentScenario = DataStorageLayer::StorageFacade::scenarioStorage()->storeScenario(_name, _synopsis, _text);
//		}

//		currentScenario->setName(mergedName);
//		currentScenario->setSynopsis(mergedSynopsis);
//		currentScenario->setText(mergedText);

//		//
//		// Отправляем на сервер результат объединения
//		//
//		aboutSaveScenarioToServer(_isDraft, IS_ASYNC);

//		//
//		// Применяем результат объединения к текущему тексту
//		//
//		emit scenarioUpdated(mergedName, mergedSynopsis, mergedText, _isDraft);
//	}
//}

//void SynchronizationManager::aboutUpdateData()
//{
//	//
//	// Получить новые изменения данных с сервера с момента m_lastDataSyncDatetime
//	//

//	//
//	// Сохранить их в БД
//	//

//	//
//	// Отправить новые изменения данных на сервер
//	//
//	aboutSaveDataToServer();

//	//
//	// Актуализация данных в хранилищах
//	//
//}

//void SynchronizationManager::aboutSaveScenarioToServer(bool _isDraft, bool _isAsync)
//{
//	//
//	// Отправляем последнюю версию на сервер
//	//
//	Scenario* scenario = DataStorageLayer::StorageFacade::scenarioStorage()->current(_isDraft);
//	aboutSaveScenarioToServer(scenario, _isAsync);

//	//
//	// Сохраняем последние отправленные значения
//	//
//	setLastSyncedName(scenario->name(), scenario->isDraft());
//	setLastSyncedSynopsis(scenario->synopsis(), scenario->isDraft());
//	setLastSyncedText(scenario->text(), scenario->isDraft());

//	//
//	// Пометим сценарий, как синхронизированный
//	//
//	scenario->setIsSynced(true);
//}

//void SynchronizationManager::aboutSaveScenarioToServer(Scenario* _scenario, bool _isAsync)
//{
//	if (!m_sessionKey.isEmpty()) {
//		WebLoader* loader = new WebLoader;
//		connect(loader, SIGNAL(finished()), loader, SLOT(deleteLater()));

//		loader->setRequestMethod(WebLoader::Post);
//		loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
//		loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
//		loader->addRequestAttribute(KEY_SCENARIO_ID, _scenario->uuid());
//		loader->addRequestAttribute("scenario_name", _scenario->name());
//		loader->addRequestAttribute("scenario_additional_info", _scenario->additionalInfo());
//		loader->addRequestAttribute("scenario_genre", _scenario->genre());
//		loader->addRequestAttribute("scenario_author", _scenario->author());
//		loader->addRequestAttribute("scenario_contacts", _scenario->contacts());
//		loader->addRequestAttribute("scenario_year", _scenario->year());
//		loader->addRequestAttribute("scenario_synopsis", _scenario->synopsis());
//		loader->addRequestAttribute("scenario_text", _scenario->text());
//		loader->addRequestAttribute(KEY_SCENARIO_IS_DRAFT, _scenario->isDraft() ? 1 : 0);
//		loader->addRequestAttribute("scenario_version_start_datetime", _scenario->versionStartDatetime().toString("yyyy-MM-dd hh:mm:ss"));
//		loader->addRequestAttribute("scenario_version_end_datetime", _scenario->versionEndDatetime().toString("yyyy-MM-dd hh:mm:ss"));
//		loader->addRequestAttribute("scenario_version_comment", _scenario->versionComment());
//		if (_isAsync) {
//			loader->loadAsync(URL_SCENARIO_SAVE);
//		} else {
//			loader->loadSync(URL_SCENARIO_SAVE);
//		}
//	}
//}

//QHash<QString, QString> SynchronizationManager::aboutLoadScenario(const QString& _uuid, bool _isDraft)
//{
//	QHash<QString, QString> scenarioValues;

//	if (!m_sessionKey.isEmpty()) {
//		//
//		// ... загружаем сценарий
//		//
//		WebLoader loader;
//		loader.setRequestMethod(WebLoader::Post);
//		loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
//		loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
//		loader.addRequestAttribute(KEY_SCENARIO_ID, _uuid); // пустой uuid == текущий
//		loader.addRequestAttribute(KEY_SCENARIO_IS_DRAFT, _isDraft ? 1 : 0);
//		QByteArray response = loader.loadSync(URL_SCENARIO_LOAD);
//		//
//		// ... считываем данные о сценарии
//		//
//		QXmlStreamReader scenarioReader(response);
//		while (!scenarioReader.atEnd()) {
//			scenarioReader.readNext();
//			if (scenarioReader.name().toString() == "status") {
//				const bool success = scenarioReader.attributes().value("result").toString() == "true";
//				if (success) {
//					scenarioReader.readNextStartElement();
//					scenarioReader.readNextStartElement(); // scenario
//					while (!scenarioReader.atEnd()) {
//						scenarioReader.readNextStartElement();
//						const QString key = scenarioReader.name().toString();
//						const QString value = scenarioReader.readElementText();
//						if (!value.isEmpty()) {
//							scenarioValues.insert(key, value);
//						}
//					}
//				}
//			}
//		}
//	}

//	return scenarioValues;
//}

//void SynchronizationManager::aboutSaveScenarioToDB(const QString& _uuid, bool _isDraft)
//{
//	//
//	// ... загружаем сценарий
//	//
//	QHash<QString, QString> scenarioValues = aboutLoadScenario(_uuid, _isDraft);

//	//
//	// ... актуализируем
//	//
//	if (!scenarioValues.isEmpty()) {
//		//
//		// Для текущей версии объединяем тексты
//		//
//		if (_uuid.isEmpty()) {
//			//
//			// ... получим данные для объединения
//			//
//			const Scenario* currentScenario = DataStorageLayer::StorageFacade::scenarioStorage()->current(_isDraft);
//			const QString lastSyncedVersion = lastSyncedText(_isDraft);
//			const QString currentVersion = currentScenario != 0 ? currentScenario->text() : QString::null;
//			const QString currentRemoteVersion = scenarioValues.value("text");
//			//
//			// ... объединяем текст версий
//			//
//			QString mergedVersion = ::mergeTexts(lastSyncedVersion, currentVersion, currentRemoteVersion);
//			//
//			// ... сохраняем актуализированную версию
//			//
//			DataStorageLayer::StorageFacade::scenarioStorage()->storeActualScenario(
//				scenarioValues.value("name"), scenarioValues.value("additional_info"),
//				scenarioValues.value("genre"), scenarioValues.value("author"),
//				scenarioValues.value("contacts"), scenarioValues.value("year"),
//				scenarioValues.value("synopsis"), mergedVersion, _isDraft,
//				QDateTime::fromString(scenarioValues.value("version_start_datetime"), "yyyy-MM-dd hh:mm:ss"),
//				QDateTime::fromString(scenarioValues.value("version_end_datetime"), "yyyy-MM-dd hh:mm:ss"),
//				scenarioValues.value("version_comment"), scenarioValues.value("id"));
//		}
//		//
//		// Для старых версий, просто обновление
//		//
//		else {
//			const bool isSynced = true;
//			DataStorageLayer::StorageFacade::scenarioStorage()->storeOldScenario(
//				scenarioValues.value("name"), scenarioValues.value("additional_info"),
//				scenarioValues.value("genre"), scenarioValues.value("author"),
//				scenarioValues.value("contacts"), scenarioValues.value("year"),
//				scenarioValues.value("synopsis"), scenarioValues.value("text"), _isDraft,
//				QDateTime::fromString(scenarioValues.value("version_start_datetime"), "yyyy-MM-dd hh:mm:ss"),
//				QDateTime::fromString(scenarioValues.value("version_end_datetime"), "yyyy-MM-dd hh:mm:ss"),
//				scenarioValues.value("version_comment"), scenarioValues.value("id"), isSynced);
//		}
//	}
//}

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

//QString SynchronizationManager::lastSyncedName(bool _isDraft) const
//{
//	QString result = _isDraft ? m_lastSyncedDraftName : m_lastSyncedName;
//	if (result.isEmpty()) {
//		Scenario* lastSynced = DataStorageLayer::StorageFacade::scenarioStorage()->lastSynced(_isDraft);
//		if (lastSynced != 0) {
//			result = lastSynced->name();
//		}
//	}

//	return result;
//}

//QString SynchronizationManager::lastSyncedSynopsis(bool _isDraft) const
//{
//	QString result = _isDraft ? m_lastSyncedDraftSynopsis : m_lastSyncedSynopsis;
//	if (result.isEmpty()) {
//		Scenario* lastSynced = DataStorageLayer::StorageFacade::scenarioStorage()->lastSynced(_isDraft);
//		if (lastSynced != 0) {
//			result = lastSynced->synopsis();
//		}
//	}

//	return result;
//}

//QString SynchronizationManager::lastSyncedText(bool _isDraft) const
//{
//	QString result = _isDraft ? m_lastSyncedDraftText : m_lastSyncedText;
//	if (result.isEmpty()) {
//		Scenario* lastSynced = DataStorageLayer::StorageFacade::scenarioStorage()->lastSynced(_isDraft);
//		if (lastSynced != 0) {
//			result = lastSynced->text();
//		}
//	}

//	return result;
//}

//void SynchronizationManager::setLastSyncedName(const QString& _name, bool _isDraft)
//{
//	if (_isDraft) {
//		m_lastSyncedDraftName = _name;
//	} else {
//		m_lastSyncedName = _name;
//	}
//}

//void SynchronizationManager::setLastSyncedSynopsis(const QString& _synopsis, bool _isDraft)
//{
//	if (_isDraft) {
//		m_lastSyncedDraftSynopsis = _synopsis;
//	} else {
//		m_lastSyncedSynopsis = _synopsis;
//	}
//}

//void SynchronizationManager::setLastSyncedText(const QString& _text, bool _isDraft)
//{
//	if (_isDraft) {
//		m_lastSyncedDraftText = _text;
//	} else {
//		m_lastSyncedText = _text;
//	}
//}






//
// Пока не используемые функции
//

//void SynchronizationManager::setCursorPosition(int _position)
//{
//	if (m_cursorPosition != _position) {
//		m_cursorPosition = _position;
//	}
//}

//void SynchronizationManager::initConnections()
//{
//	QTimer* sendChangesTimer = new QTimer(this);
//	connect(sendChangesTimer, SIGNAL(timeout()), this, SLOT(aboutSendChangesToServer()));

//	//
//	// Отправляем/получаем изменения, каждые SEND_CHANGES_INTERVAL мсек
//	//
//	const int SEND_CHANGES_INTERVAL = 1000;
//	sendChangesTimer->start(SEND_CHANGES_INTERVAL);
//}


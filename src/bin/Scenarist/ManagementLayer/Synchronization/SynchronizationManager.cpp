#include "SynchronizationManager.h"

#include <ManagementLayer/Project/ProjectsManager.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/DatabaseHistoryStorage.h>

#include <Domain/Scenario.h>

#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <WebLoader.h>

#include <QEventLoop>
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
	const QUrl URL_LOGIN = QUrl("http://kitscenarist.ru/api/account/login/");
	const QUrl URL_LOGOUT = QUrl("http://kitscenarist.ru/api/account/logout/");
	const QUrl URL_PROJECTS = QUrl("http://kitscenarist.ru/api/projects/");
	const QUrl URL_SCENARIO_SAVE = QUrl("http://kitscenarist.ru/api/projects/scenario/save/");
	const QUrl URL_SCENARIO_DATA_SAVE = QUrl("http://kitscenarist.ru/api/projects/data/save/");
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

void SynchronizationManager::aboutSaveScenario(const Domain::Scenario* _scenario)
{
	if (!m_sessionKey.isEmpty()) {
		//
		// Отправляем последнюю версию на сервер
		//
		WebLoader* loader = new WebLoader;
		connect(loader, SIGNAL(finished()), loader, SLOT(deleteLater()));

		loader->setRequestMethod(WebLoader::Post);
		loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		loader->addRequestAttribute("scenario_id", _scenario->id().value());
		loader->addRequestAttribute("scenario_name", _scenario->name());
		loader->addRequestAttribute("scenario_additional_info", _scenario->additionalInfo());
		loader->addRequestAttribute("scenario_genre", _scenario->genre());
		loader->addRequestAttribute("scenario_author", _scenario->author());
		loader->addRequestAttribute("scenario_contacts", _scenario->contacts());
		loader->addRequestAttribute("scenario_year", _scenario->year());
		loader->addRequestAttribute("scenario_synopsis", _scenario->synopsis());
		loader->addRequestAttribute("scenario_text", _scenario->text());
		loader->addRequestAttribute("scenario_is_draft", _scenario->isDraft() ? 1 : 0);
		loader->addRequestAttribute("scenario_version_start_datetime", _scenario->versionStartDatetime().toString("yyyy-MM-dd hh:mm:ss"));
		loader->addRequestAttribute("scenario_version_end_datetime", _scenario->versionEndDatetime().toString("yyyy-MM-dd hh:mm:ss"));
		loader->addRequestAttribute("scenario_version_comment", _scenario->versionComment());
		loader->loadAsync(URL_SCENARIO_SAVE);
	}
}

void SynchronizationManager::aboutSaveData()
{
	if (!m_sessionKey.isEmpty()) {
		//
		// Получить данные, которых на сервере ещё нет
		//
		QList<QMap<QString, QString> > databaseHistory =
				StorageFacade::databaseHistoryStorage()->history(m_lastSendedDataDatetime);

		//
		// Сформировать xml для отправки
		//
		QString dataChangesXml;
		QXmlStreamWriter xmlWriter(&dataChangesXml);
		xmlWriter.writeStartDocument();
		xmlWriter.writeStartElement("changes");
		QMap<QString, QString> historyRecord;
		foreach (historyRecord, databaseHistory) {
			//
			// Отправляем изменения из всех таблиц, кроме сценария
			//
			if (!historyRecord.value(DBH_QUERY_KEY).contains(" scenario ")) {
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

				//
				// Обновляем дату и время последнего отправленного изменения
				//
				m_lastSendedDataDatetime = historyRecord.value(DBH_DATETIME_KEY);
			}
		}
		xmlWriter.writeEndElement(); // changes
		xmlWriter.writeEndDocument();

		//
		// Отправить данные
		//
		WebLoader* loader = new WebLoader;
		connect(loader, SIGNAL(finished()), loader, SLOT(deleteLater()));

		loader->setRequestMethod(WebLoader::Post);
		loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
		loader->addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
		loader->addRequestAttribute(KEY_CHANGES, dataChangesXml);
		loader->loadAsync(URL_SCENARIO_DATA_SAVE);
	}
}

void SynchronizationManager::initConnections()
{
	connect(this, SIGNAL(loginAccepted(QString)), this, SLOT(aboutLoadProjects()));
}

void SynchronizationManager::sleepALittle()
{
	QEventLoop loop;
	QTimer::singleShot(2000, &loop, SLOT(quit()));
	loop.exec();
}






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


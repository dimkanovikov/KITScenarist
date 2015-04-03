#include "SynchronizationManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <WebLoader.h>

#include <QTimer>
#include <QXmlStreamReader>

using ManagementLayer::SynchronizationManager;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;

namespace {
	/**
	 * @brief Ссылки для запросов
	 */
	/** @{ */
	const QUrl URL_LOGIN = QUrl("http://kitscenarist.ru/api/account/login/");
	const QUrl URL_LOGOUT = QUrl("http://kitscenarist.ru/api/account/logout/");
	/** @} */

	/**
	 * @brief Ключи для параметров запросов
	 */
	/** @{ */
	const QString KEY_USER_NAME = "login";
	const QString KEY_PASSWORD = "password";
	const QString KEY_SESSION_KEY = "session_key";
	/** @{ */
}


SynchronizationManager::SynchronizationManager(QObject* _parent, QWidget* _parentView) :
	QObject(_parent),
	m_view(_parentView)
{
	//	initConnections();
}

void SynchronizationManager::login()
{
	//
	// Авторизуемся, если параметры сохранены
	//
	bool authParamsStored =
			StorageFacade::settingsStorage()->value(
				"application/save-user-name-and-password",
				SettingsStorage::ApplicationSettings
				).toInt();
	if (authParamsStored) {
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
		aboutLogin(login, password, authParamsStored);
	}
}

void SynchronizationManager::aboutLogin(const QString& _userName, const QString& _password, bool _rememberUser)
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
			"application/save-user-name-and-password",
			_rememberUser ? "1" : "0",
			SettingsStorage::ApplicationSettings);
		if (_rememberUser) {
			StorageFacade::settingsStorage()->setValue(
				"application/user-name",
				PasswordStorage::save(_userName),
				SettingsStorage::ApplicationSettings);
			StorageFacade::settingsStorage()->setValue(
				"application/password",
				PasswordStorage::save(_password, _userName),
				SettingsStorage::ApplicationSettings);
		}

		emit loginAccepted(_userName);
	} else {
		emit loginNotAccepted(_userName, _password, _rememberUser, errorMessage);
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
			"application/save-user-name-and-password",
			QString::null,
			SettingsStorage::ApplicationSettings);
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
		progress.finish();

		emit logoutAccepted();
	}
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


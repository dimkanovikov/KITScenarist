#ifndef SYNCHRONIZATIONMANAGER_H
#define SYNCHRONIZATIONMANAGER_H

#include <QObject>
#include <QHash>

namespace Domain {
	class Scenario;
	class ScenarioChange;
}

class WebLoader;


namespace ManagementLayer
{
	/**
	 * @brief Управляющий синхронизацией
	 */
	class SynchronizationManager : public QObject
	{
		Q_OBJECT

	public:
		explicit SynchronizationManager(QObject* _parent, QWidget* _parentView);

	public slots:
		/**
		 * @brief Авторизоваться используя сохранённые параметры авторизации
		 */
		void login();

		/**
		 * @brief Авторизоваться
		 */
		void aboutLogin(const QString& _userName, const QString& _password);

		/**
		 * @brief Закрыть авторизацию
		 */
		void aboutLogout();

		/**
		 * @brief Загрузить список доступных проектов
		 */
		void aboutLoadProjects();

		/**
		 * @brief Полная синхронизация сценария
		 */
		void aboutFullSyncScenario();

		/**
		 * @brief Синхронизация сценария во время работы над ним
		 */
		void aboutWorkSyncScenario();

		/**
		 * @brief Загрузить информацию о курсорах соавторов и отправить информацию о своём
		 */
		void aboutUpdateCursors(int _cursorPosition, bool _isDraft);

		/**
		 * @brief Полная синхронизация данных
		 */
		void aboutFullSyncData();

		/**
		 * @brief Синхронизация данных во время работы
		 */
		void aboutWorkSyncData();

	private:
		/**
		 * @brief Обработать ошибку работы с API
		 */
		void handleError(const QByteArray& _response);

		/**
		 * @brief Возможно ли использовать методы синхронизации
		 */
		bool isCanSync() const;

		/**
		 * @brief Отправить изменения сценария на сервер
		 * @return Удалось ли отправить данные
		 */
		bool uploadScenarioChanges(const QList<QString>& _changesUuids);

		/**
		 * @brief Скачать изменения с сервера
		 */
		QList<QHash<QString, QString> > downloadScenarioChanges(const QString& _changesUuids);

		/**
		 * @brief Отправить изменения данных на сервер
		 * @return Удалось ли отправить данные
		 */
		bool uploadScenarioData(const QList<QString>& _dataUuids);

		/**
		 * @brief Скачать и сохранить в БД изменения с сервера
		 */
		void downloadAndSaveScenarioData(const QString& _dataUuids);

	signals:
		/**
		 * @brief Авторизация пройдена успешно
		 */
		void loginAccepted();

		/**
		 * @brief Авторизация закрыта
		 */
		void logoutAccepted();

		/**
		 * @brief Список доступных проектов получен
		 */
		void remoteProjectsLoaded(const QString& _projectsXml);

		/**
		 * @brief Необходимо применить патч
		 */
		void applyPatchRequested(const QString& _patch, bool _isDraft);

		/**
		 * @brief Получены новые позиции курсоров пользователей
		 */
		void cursorsUpdated(const QMap<QString, int>& _cursors, bool _isDraft = false);

		/**
		 * @brief Сессия закрыта с ошибкой
		 */
		void syncClosedWithError(int errorCode, const QString& _errorText);

	private:
		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

		/**
		 * @brief Заблокировать ненадолго выполнение функции
		 * @note Применяется для того, чтобы интерфейс не моргал
		 */
		void sleepALittle();

	private:
		/**
		 * @brief Указатель на главную форму приложения
		 */
		QWidget* m_view;

		/**
		 * @brief Загрузчик данных
		 */
		WebLoader* m_loader;

		/**
		 * @brief Ключ сессии
		 */
		QString m_sessionKey;

		/**
		 * @brief Дата и время последней синхронизации изменений сценария
		 */
		QString m_lastChangesSyncDatetime;

		/**
		 * @brief Дата и время последней синхронизации изменений данных
		 */
		QString m_lastDataSyncDatetime;
	};
}

#endif // SYNCHRONIZATIONMANAGER_H

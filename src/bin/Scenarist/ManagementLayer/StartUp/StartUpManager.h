#ifndef STARTUPMANAGER_H
#define STARTUPMANAGER_H

#include <QObject>

#include <QMap>

namespace UserInterface {
	class StartUpView;
}

class QAbstractItemModel;
class QNetworkReply;


namespace ManagementLayer
{
	/**
	 * @brief Управляющий стартовой страницей
	 */
	class StartUpManager : public QObject
	{
		Q_OBJECT

	public:
		explicit StartUpManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

	public slots:
		/**
		 * @brief Пользователь с заданным именем успешно авторизован на сервере
		 */
		void aboutUserLogged(const QString& _userName);

		/**
		 * @brief Попробовать повторно авторизоваться, после неудачной попытки
		 */
		void aboutRetryLogin(const QString& _userName, const QString& _password, const QString& _error);

		/**
		 * @brief Пользователь закрыл авторизацию
		 */
		void aboutUserUnlogged();

		/**
		 * @brief Установить список недавно используемых проектов
		 */
		void setRecentProjects(QAbstractItemModel* _model);

		/**
		 * @brief Установить список проектов из облака
		 */
		void setRemoteProjects(QAbstractItemModel* _model);

	signals:
		/**
		 * @brief Пользователь хочет авторизоваться
		 */
		void loginRequested(const QString& _userName, const QString& _password);

		/**
		 * @brief Пользователь хочет выйти
		 */
		void logoutRequested();

		/**
		 * @brief Создать проект
		 */
		void createProjectRequested();

		/**
		 * @brief Открыть проект
		 */
		void openProjectRequested();

		/**
		 * @brief Помощь
		 */
		void helpRequested();

		/**
		 * @brief Обновить список проектов из облака
		 */
		void refreshProjectsRequested();

		/**
		 * @brief Выбран один из недавних файлов для открытия
		 */
		void openRecentProjectRequested(const QModelIndex& _recentProjectIndex);

		/**
		 * @brief Выбран один из проектов из облака для открытия
		 */
		void openRemoteProjectRequested(const QModelIndex& _remoteProjectIndex);

	private slots:
		/**
		 * @brief Нажата кнопка войти
		 */
		void aboutLoginClicked();

		/**
		 * @brief Загрузилась страница с информацией об обновлениях
		 */
		void aboutLoadUpdatesInfo(QNetworkReply* _reply);

	private:

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

		/**
		 * @brief Проверить наличие новой версии
		 */
		void checkNewVersion();

	private:
		/**
		 * @brief Представление для стартовой страницы
		 */
		UserInterface::StartUpView* m_view;
	};
}

#endif // STARTUPMANAGER_H

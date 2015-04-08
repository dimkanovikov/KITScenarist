#ifndef STARTUPMANAGER_H
#define STARTUPMANAGER_H

#include <QObject>

#include <QMap>

namespace UserInterface {
	class StartUpView;
}

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
		~StartUpManager();

		QWidget* view() const;

		/**
		 * @brief Добавить недавно открытый файл в список
		 */
		void addRecentFile(const QString& _filePath, const QString& _projectName = QString());

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
		 * @brief Загружен список проектов доступных из облака
		 */
		void aboutRemoteProjectsLoaded();

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
		void refreshRemoteProjectsRequested();

		/**
		 * @brief Выбран один из недавних файлов для открытия
		 */
		void openRecentProjectRequested(const QString& _filePath);

	private slots:
		/**
		 * @brief Нажата кнопка войти
		 */
		void aboutLoginClicked();

		/**
		 * @brief Выбран один из недавних файлов для открытия
		 */
		void aboutOpenRecentProjectRequested(const QModelIndex& _projectIndex);

		/**
		 * @brief Загрузилась страница с информацией об обновлениях
		 */
		void aboutLoadUpdatesInfo(QNetworkReply* _reply);

		/**
		 * @brief Обновить список недавних файлов, удаляя несуществующие
		 */
		void aboutRefreshProjects();

	private:
		/**
		 * @brief Загрузить данные
		 */
		void initData();

		/**
		 * @brief Настроить представление
		 */
		void initView();

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

		/**
		 * @brief Недавно используемые файлы проектов
		 *
		 * key - путь к файлу проекта
		 * value - название проекта
		 */
		QMap<QString, QString> m_recentFiles;

		/**
		 * @brief Порядок использования недавних файлов
		 *
		 * key - путь к файлу проекта
		 * value - последнее использование
		 */
		QMap<QString, QString> m_recentFilesUsing;
	};
}

#endif // STARTUPMANAGER_H

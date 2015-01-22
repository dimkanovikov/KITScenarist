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

	signals:
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
		 * @brief Выбран один из недавних файлов для открытия
		 */
		void openRecentProjectRequested(const QString& _filePath);

	private slots:
		/**
		 * @brief Выбран один из недавних файлов для открытия
		 */
		void aboutOpenRecentProjectRequested(const QString& _filePath);

		/**
		 * @brief Загрузилась страница с информацией об обновлениях
		 */
		void aboutLoadUpdatesInfo(QNetworkReply* _reply);

		/**
		 * @brief Обновить список недавних файлов, удаляя несуществующие
		 */
		void aboutRefreshRecentFiles();

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

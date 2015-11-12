#ifndef STARTUPMANAGER_H
#define STARTUPMANAGER_H

#include <QObject>

namespace UserInterface {
	class StartUpView;
	class AddProjectDialog;
}

class QAbstractItemModel;


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

		/**
		 * @brief Панель инструментов представления
		 */
		QWidget* toolbar() const;

		/**
		 * @brief Представление
		 */
		QWidget* view() const;

		/**
		 * @brief Установить список недавно используемых проектов
		 */
		void setRecentProjects(QAbstractItemModel* _model);

		/**
		 * @brief Установить список проектов из облака
		 */
		void setRemoteProjects(QAbstractItemModel* _model);

		/**
		 * @brief Настроить видимость списка проектов из облака
		 */
		/** @{ */
		void showRemoteProjects();
		void hideRemoteProjects();
		/** @} */

	signals:
		/**
		 * @brief Создать проект
		 */
		void createProjectRequested(const QString& _projectName);

		/**
		 * @brief Открыть проект
		 */
		void openProjectRequested();

		/**
		 * @brief Выбран один из недавних файлов для открытия
		 */
		void openRecentProjectRequested(const QModelIndex& _recentProjectIndex);

		/**
		 * @brief Выбран один из проектов из облака для открытия
		 */
		void openRemoteProjectRequested(const QModelIndex& _remoteProjectIndex);

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

        /**
         * @brief Отправить статистику о запуске
         */
        void sendStatistics();

	private:
		/**
		 * @brief Представление для стартовой страницы
		 */
		UserInterface::StartUpView* m_view;

		/**
		 * @brief Диалог создания нового проекта
		 */
		UserInterface::AddProjectDialog* m_addProjectDialog;
	};
}

#endif // STARTUPMANAGER_H

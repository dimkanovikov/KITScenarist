#ifndef PROJECTSMANAGER_H
#define PROJECTSMANAGER_H

#include "Project.h"

#include <QMap>

class QAbstractItemModel;
class QModelIndex;


namespace ManagementLayer
{
	/**
	 * @brief Управляющий проектами
	 */
	class ProjectsManager
	{
	public:
		/**
		 * @brief Установить список недавно используемых проектов
		 */
		static void setRecentProjects(
				const QMap<QString, QString>& _recentFiles,
				const QMap<QString, QString>& _recentFilesUsing);

		/**
		 * @brief Получить список недавно используемых проектов
		 * @note Владение моделью передаётся клиенту
		 */
		static QAbstractItemModel* recentProjects();

		/**
		 * @brief Установить список проектов из облака в виде xml с сервиса
		 */
		static void setRemoteProjects(const QString& _xml);

		/**
		 * @brief Получить список доступных проектов из облака
		 * @note Владение моделью передаётся клиенту
		 */
		static QAbstractItemModel* remoteProjects();

		/**
		 * @brief Установить текущий проект, с которым хочет работать пользователь
		 */
		static void setCurrentProject(const QModelIndex& _index, bool _isRemote);

		/**
		 * @brief Получить текущий проект, с которым работает пользователь
		 */
		static const Project& currentProject();

	private:
		/**
		 * @brief Недавно используемые проекты
		 */
		static QList<Project> s_recentProjects;

		/**
		 * @brief Проекты доступные из облака
		 */
		static QList<Project> s_remoteProjects;

		/**
		 * @brief Текущий проект
		 */
		static Project s_currentProject;
	};
}

#endif // PROJECTSMANAGER_H

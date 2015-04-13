#include "ProjectsManager.h"

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QStandardItemModel>
#include <QXmlStreamReader>

using ManagementLayer::ProjectsManager;
using ManagementLayer::Project;

namespace {
	const int MAX_RECENT_FILES_COUNT = 10;
	const QString RECENT_FILES_LIST_SETTINGS_KEY = "application/recent-files/list";
	const QString RECENT_FILES_USING_SETTINGS_KEY = "application/recent-files/using";
}


const ManagementLayer::Project&ProjectsManager::currentProject()
{
	return s_currentProject;
}

Project ProjectsManager::s_currentProject;


// ********


ProjectsManager::ProjectsManager(QObject* _parent) :
	QObject(_parent)
{
	loadRecentProjects();
}

ProjectsManager::~ProjectsManager()
{
	saveRecentProjects();
}

QAbstractItemModel* ProjectsManager::recentProjects()
{
	//
	// Создаём модель
	//
	QStandardItemModel* recentProjectsModel = new QStandardItemModel;
	foreach (const Project& project, m_recentProjects) {
		QStandardItem* item = new QStandardItem;
		item->setData(project.displayName(), Qt::DisplayRole);
		item->setData(project.displayPath(), Qt::WhatsThisRole);
		recentProjectsModel->appendRow(item);
	}

	return recentProjectsModel;
}

QAbstractItemModel* ProjectsManager::remoteProjects()
{
	//
	// Создаём модель
	//
	QStandardItemModel* remoteProjectsModel = new QStandardItemModel;
	foreach (const Project& project, m_remoteProjects) {
		QStandardItem* item = new QStandardItem;
		item->setData(project.displayName(), Qt::DisplayRole);
		item->setData(project.displayPath(), Qt::WhatsThisRole);
		remoteProjectsModel->appendRow(item);
	}

	return remoteProjectsModel;
}

void ProjectsManager::setCurrentProject(const QString& _path, bool _isLocal)
{
	//
	// Приведём путь к нативному виду
	//
	const QString projectPath = QDir::toNativeSeparators(_path);

	//
	// Делаем проект текущим и загружаем из него БД
	// или создаём, если ранее его не существовало
	//
	DatabaseLayer::Database::setCurrentFile(projectPath);

	Project newCurrentProject;
	//
	// Для локальных файлов делаем обработку списка недавно используемых
	//
	if (_isLocal) {
		//
		// Проверяем находится ли проект в списке недавно используемых
		//
		foreach (const Project& project, m_recentProjects) {
			if (project.path() == projectPath) {
				newCurrentProject = project;
				break;
			}
		}

		//
		// Если проект был в списке недавних делаем его первым
		//
		if (newCurrentProject.type() != Project::Invalid) {
			m_recentProjects.removeOne(newCurrentProject);
			newCurrentProject.setLastEditDatetime(QDateTime::currentDateTime());
			m_recentProjects.prepend(newCurrentProject);
		}
		//
		// Если не был добавляем в начало списка ранее используемых
		//
		else {
			//
			// Определим название проекта
			//
			QFileInfo fileInfo(projectPath);
			QString projectName = fileInfo.baseName();
			//
			// Создаём проект
			//
			newCurrentProject = Project(Project::Local, projectName, projectPath, QDateTime::currentDateTime());
			//
			// Если в списке больше допустимого кол-ва используемых файлов удалим давно используемый
			//
			if (m_recentProjects.size() >= MAX_RECENT_FILES_COUNT) {
				m_recentProjects.removeLast();
			}
			//
			// Добавляем проект в список
			//
			m_recentProjects.prepend(newCurrentProject);
		}

		//
		// Уведомляем об обновлении
		//
		emit recentProjectsUpdated();
	}
	//
	// Для проектов из облака просто определяем сам проект
	//
	else {
		foreach (const Project& project, m_remoteProjects) {
			if (project.path() == projectPath) {
				newCurrentProject = project;
				break;
			}
		}
	}

	//
	// Запоминаем проект, как текущий
	//
	s_currentProject = newCurrentProject;
}

void ProjectsManager::setCurrentProject(const QModelIndex& _index, bool _isLocal)
{
	//
	// Определим проект
	//
	QString newCurrentProjectPath;
	if (_isLocal) {
		if (m_recentProjects.size() > _index.row()) {
			newCurrentProjectPath = m_recentProjects.at(_index.row()).path();
		}
	} else {
		if (m_remoteProjects.size() > _index.row()) {
			newCurrentProjectPath = m_remoteProjects.at(_index.row()).path();
		}
	}
	//
	// ... и установим его текущим
	//
	setCurrentProject(newCurrentProjectPath, _isLocal);
}

void ProjectsManager::setCurrentProjectName(const QString& _projectName)
{
	//
	// Если имя не задано, то по умолчанию используется название файла, нам ни чего делать не надо
	//
	if (!_projectName.isEmpty()) {
		//
		// Определим источник хранения проекта
		//
		QMutableListIterator<ManagementLayer::Project> projectsIterator(m_recentProjects);
		if (s_currentProject.type() == Project::Remote) {
			projectsIterator = QMutableListIterator<ManagementLayer::Project>(m_remoteProjects);
		}
		//
		// Обновим название проекта
		//
		while (projectsIterator.hasNext()) {
			Project& project = projectsIterator.next();
			if (project == s_currentProject) {
				s_currentProject.setName(_projectName);
				projectsIterator.setValue(s_currentProject);
				break;
			}
		}

		//
		// Уведомляем об обновлении
		//
		if (s_currentProject.type() == Project::Local) {
			emit recentProjectsUpdated();
		} else {
			emit remoteProjectsUpdated();
		}
	}
}

void ProjectsManager::closeCurrentProject()
{
	s_currentProject = Project();
}

void ProjectsManager::refreshProjects()
{
	//
	// Обновляем локальные
	//
	{
		//
		// Удаляем все несуществующие файлы
		//
		QMutableListIterator<Project> checker(m_recentProjects);
		while (checker.hasNext()) {
			const Project& project = checker.next();
			if (!QFile::exists(project.path())) {
				checker.remove();
			}
		}
	}

	//
	// Уведомляем об обновлении
	//
	emit recentProjectsUpdated();
}

void ProjectsManager::setRemoteProjects(const QString& _xml)
{
	m_remoteProjects.clear();

	//
	// Считываем список проектов из xml
	//
	QXmlStreamReader projectsReader(_xml);
	while (!projectsReader.atEnd()) {
		projectsReader.readNext();
		if (projectsReader.tokenType() == QXmlStreamReader::StartElement
			&& projectsReader.name().toString() == "project") {
			const QString name = projectsReader.attributes().value("name").toString();
			const QString path = QString::null;
			const QString lastEditDatetimeText = projectsReader.attributes().value("modified_at").toString();
			const QDateTime lastEditDatetime = QDateTime::fromString(lastEditDatetimeText, "yyyy-MM-dd hh:mm:ss");
			const int id = projectsReader.attributes().value("id").toInt();
			const QString owner = projectsReader.attributes().value("owner").toString();
			const QString roleText = projectsReader.attributes().value("role").toString();
			Project::Role role = Project::roleFromString(roleText);

			m_remoteProjects.append(Project(Project::Remote, name, path, lastEditDatetime, id, owner, role));
		}
	}

	//
	// Уведомляем об обновлении
	//
	emit remoteProjectsUpdated();
}

void ProjectsManager::loadRecentProjects()
{
	//
	// Загрузим список недавних файлов из настроек
	//
	QMap<QString, QString> recentFiles =
			DataStorageLayer::StorageFacade::settingsStorage()->values(
				RECENT_FILES_LIST_SETTINGS_KEY,
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	QMap<QString, QString> recentFilesUsing =
			DataStorageLayer::StorageFacade::settingsStorage()->values(
				RECENT_FILES_USING_SETTINGS_KEY,
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);

	//
	// Сохраним недавно используемые проекты
	//
	m_recentProjects.clear();

	//
	// Формируем список недавно используемых проектов в порядке убывания даты изменения
	//
	QStringList usingDates = recentFilesUsing.values();
	qSort(usingDates.begin(), usingDates.end(), qGreater<QString>());
	foreach (const QString& usingDate, usingDates) {
		//
		// Путь к проекту
		//
		const QString path = recentFilesUsing.key(usingDate);
		//
		// Название проекта
		//
		const QString name = recentFiles.value(path);
		//
		// Сам проект
		//
		m_recentProjects.append(
			Project(Project::Local, name, path, QDateTime::fromString(usingDate, "yyyy-MM-dd hh:mm:ss")));
	}

	//
	// Уведомляем об обновлении
	//
	emit recentProjectsUpdated();
}

void ProjectsManager::saveRecentProjects()
{
	//
	// Формируем список недавно используемых файлов для сохранения
	//

	/*
	 * @brief Недавно используемые файлы проектов
	 *
	 * key - путь к файлу проекта
	 * value - название проекта
	 */
	QMap<QString, QString> recentFiles;
	/*
	 * @brief Порядок использования недавних файлов
	 *
	 * key - путь к файлу проекта
	 * value - последнее использование
	 */
	QMap<QString, QString> recentFilesUsing;

	foreach (const Project& project, m_recentProjects) {
		recentFiles.insert(project.path(), project.name());
		recentFilesUsing.insert(project.path(), project.lastEditDatetime().toString("yyyy-MM-dd hh:mm:ss"));
	}


	//
	// Сохраняем
	//
	DataStorageLayer::StorageFacade::settingsStorage()->setValues(
				recentFiles,
				RECENT_FILES_LIST_SETTINGS_KEY,
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValues(
				recentFilesUsing,
				RECENT_FILES_USING_SETTINGS_KEY,
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
}

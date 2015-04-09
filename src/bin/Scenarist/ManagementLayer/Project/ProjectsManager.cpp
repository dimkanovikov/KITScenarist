#include "ProjectsManager.h"

#include <QFileInfo>
#include <QList>
#include <QStandardItemModel>
#include <QXmlStreamReader>

using ManagementLayer::ProjectsManager;
using ManagementLayer::Project;


void ProjectsManager::setRecentProjects(const QMap<QString, QString>& _recentFiles,
	const QMap<QString, QString>& _recentFilesUsing)
{
	s_recentProjects.clear();

	//
	// Формируем список недавно используемых проектов в порядке убывания даты изменения
	//
	QStringList usingDates = _recentFilesUsing.values();
	qSort(usingDates.begin(), usingDates.end(), qGreater<QString>());
	foreach (const QString& usingDate, usingDates) {
		//
		// Путь к проекту
		//
		const QString path = _recentFilesUsing.key(usingDate);
		//
		// Название проекта
		//
		const QString name = _recentFiles.value(path);

		s_recentProjects.append(
			Project(Project::Local, name, path, QDateTime::fromString(usingDate, "yyyy-MM-dd hh:mm:ss")));
	}
}

QAbstractItemModel* ProjectsManager::recentProjects()
{
	//
	// Создаём модель
	//
	QStandardItemModel* recentProjectsModel = new QStandardItemModel;
	foreach (const Project& project, s_recentProjects) {
		QStandardItem* item = new QStandardItem;
		item->setData(project.displayName(), Qt::DisplayRole);
		item->setData(project.displayPath(), Qt::WhatsThisRole);
		recentProjectsModel->appendRow(item);
	}

	return recentProjectsModel;
}

void ProjectsManager::setRemoteProjects(const QString& _xml)
{
	s_remoteProjects.clear();

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

			s_remoteProjects.append(Project(Project::Remote, name, path, lastEditDatetime, id, owner, role));
		}
	}
}

QAbstractItemModel* ProjectsManager::remoteProjects()
{
	//
	// Создаём модель
	//
	QStandardItemModel* remoteProjectsModel = new QStandardItemModel;
	foreach (const Project& project, s_remoteProjects) {
		QStandardItem* item = new QStandardItem;
		item->setData(project.displayName(), Qt::DisplayRole);
		item->setData(project.displayPath(), Qt::WhatsThisRole);
		remoteProjectsModel->appendRow(item);
	}

	return remoteProjectsModel;
}

void ProjectsManager::setCurrentProject(const QModelIndex& _index, bool _isRemote)
{
	if (_isRemote) {
		if (s_remoteProjects.size() > _index.row()) {
			s_currentProject = s_remoteProjects.at(_index.row());
		}
	} else {
		if (s_recentProjects.size() > _index.row()) {
			s_currentProject = s_recentProjects.at(_index.row());
		}
	}
}

const ManagementLayer::Project&ProjectsManager::currentProject()
{
	return s_currentProject;
}

QList<Project> ProjectsManager::s_recentProjects;
QList<Project> ProjectsManager::s_remoteProjects;
Project ProjectsManager::s_currentProject;

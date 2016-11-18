#include "Project.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/PasswordStorage.h>

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

using ManagementLayer::Project;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;

namespace {
	const QString PROJECT_FILE_EXTENSION = ".kitsp"; // kit scenarist project
}


QString Project::roleToString(Project::Role _role)
{
	QString result;
	switch (_role) {
		case Owner: result = QApplication::translate("ManagementLayer::Project", "Owner"); break;
		case Redactor: result = QApplication::translate("ManagementLayer::Project", "Redactor"); break;
		case Commentator: result = QApplication::translate("ManagementLayer::Project", "Commentator"); break;
	}

	return result;
}

Project::Role Project::roleFromString(const QString& _role)
{
	return _role == "owner" ? Owner : (_role == "redactor" ? Redactor : Commentator);
}

Project::Project() :
	m_type(Invalid),
	m_id(0),
	m_role(Owner),
	m_isSyncAvailable(false)
{
}

Project::Project(Type _type, const QString& _name, const QString& _path,
	const QDateTime& _lastEditDatetime, int _id, const QString& _owner, Role _role,
	const QStringList& _users) :
	m_type(_type),
	m_name(_name),
	m_path(_path),
	m_lastEditDatetime(_lastEditDatetime),
	m_id(_id),
	m_owner(_owner),
	m_role(_role),
	m_users(_users),
	m_isSyncAvailable(false)
{
	//
	// Сформируем путь к файлам проектов из облака
	//
	if (m_type == Remote) {
		//
		// Настроим путь к папке с проектами для текущего пользователя
		//
		const QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
		const QString login =
				StorageFacade::settingsStorage()->value(
					"application/user-name",
					SettingsStorage::ApplicationSettings);
		const QString remoteProjectsFolderPath =
				QString("%1%4%2%4%3").arg(appDataFolderPath).arg("Projects").arg(login).arg(QDir::separator());
		//
		// ... создаём папку для пользовательских файлов
		//
		QDir rootFolder = QDir::root();
		rootFolder.mkpath(remoteProjectsFolderPath);
		//
		// ... формируем путь к файлу проекта
		//
		m_path = remoteProjectsFolderPath + QDir::separator() + QString::number(m_id) + PROJECT_FILE_EXTENSION;
		//
		// ... корректируем путь
		//
		m_path = QDir::toNativeSeparators(m_path);

		//
		// Устанавливаем флаг доступности синхронизации
		//
		m_isSyncAvailable = true;
	}
}

Project::Type Project::type() const
{
	return m_type;
}

bool Project::isRemote() const
{
	return m_type == Remote;
}

QString Project::displayName() const
{
	QString result = m_name;
	if (m_type == Remote) {
		result += QString(" [%1]").arg(roleToString(m_role));
	}

	return result;
}

QString Project::name() const
{
	return m_name;
}

void Project::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;
	}
}

QString Project::displayPath() const
{
	QString result = m_path;
	if (m_type == Remote) {
		result = QString("http://kitscenarist.ru/%1/%2%3")
				 .arg(m_owner).arg(m_name).arg(PROJECT_FILE_EXTENSION);
	}

	return result;
}

QString Project::path() const
{
	return m_path;
}

QDateTime Project::lastEditDatetime() const
{
	return m_lastEditDatetime;
}

void Project::setLastEditDatetime(const QDateTime& _datetime)
{
	if (m_lastEditDatetime != _datetime) {
		m_lastEditDatetime = _datetime;
	}
}

int Project::id() const
{
	return m_id;
}

bool Project::isUserOwner() const
{
	return m_role == Owner;
}

bool Project::isCommentOnly() const
{
	return m_role == Commentator;
}

QStringList Project::users() const
{
	return m_users;
}

bool Project::isSyncAvailable(int* _errorCode) const
{
	if (_errorCode != 0) {
		*_errorCode = m_errorCode;
	}
	return m_isSyncAvailable;
}

void Project::setSyncAvailable(bool _syncAvailable, int _errorCode)
{
	if (m_isSyncAvailable != _syncAvailable) {
		m_isSyncAvailable = _syncAvailable;
		m_errorCode = _errorCode;
	}
}


bool ManagementLayer::operator==(const ManagementLayer::Project& _lhs, const ManagementLayer::Project& _rhs)
{
	return _lhs.path() == _rhs.path();
}

#include "Project.h"

#include <QApplication>

using ManagementLayer::Project;


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
	m_role(Owner)
{
}

Project::Project(Type _type, const QString& _name, const QString& _path,
	const QDateTime& _lastEditDatetime, int _id, const QString& _owner, Role _role) :
	m_type(_type),
	m_name(_name),
	m_path(_path),
	m_lastEditDatetime(_lastEditDatetime),
	m_id(_id),
	m_owner(_owner),
	m_role(_role)
{

}

QString Project::displayName() const
{
	QString result = m_name;
	if (m_type == Remote) {
		result += QString(" [%1]").arg(roleToString(m_role));
	}

	return result;
}

QString Project::displayPath() const
{
	QString result = m_path;
	if (m_type == Remote) {
		result = QString("http://kitscenarist.ru/%1/%2.kitsp").arg(m_owner).arg(m_name);
	}

	return result;
}

QString Project::path() const
{
	return m_path;
}

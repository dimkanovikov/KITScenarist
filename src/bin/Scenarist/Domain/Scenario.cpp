#include "Scenario.h"

using namespace Domain;


Scenario::Scenario(const Identifier& _id, const QString& _name, const QString& _synopsis,
	const QString& _text, const QDateTime& _versionStartDatetime,
	const QDateTime& _versionEndDatetime, const QString& _versionComment, const QString& _uuid,
	bool _isSynced) :
	DomainObject(_id),
	m_name(_name),
	m_synopsis(_synopsis),
	m_text(_text),
	m_isDraft(false),
	m_versionStartDatetime(_versionStartDatetime),
	m_versionEndDatetime(_versionEndDatetime),
	m_versionComment(_versionComment),
	m_uuid(_uuid),
	m_isSynced(_isSynced)
{
}

QString Scenario::name() const
{
	return m_name;
}

void Scenario::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;

		changesNotStored();
	}
}

QString Scenario::additionalInfo() const
{
	return m_additionalInfo;
}

void Scenario::setAdditionalInfo(const QString& _additionalInfo)
{
	if (m_additionalInfo != _additionalInfo) {
		m_additionalInfo = _additionalInfo;

		changesNotStored();
	}
}

QString Scenario::genre() const
{
	return m_genre;
}

void Scenario::setGenre(const QString& _genre)
{
	if (m_genre != _genre) {
		m_genre = _genre;

		changesNotStored();
	}
}

QString Scenario::author() const
{
	return m_author;
}

void Scenario::setAuthor(const QString _author)
{
	if (m_author != _author) {
		m_author = _author;

		changesNotStored();
	}
}

QString Scenario::contacts() const
{
	return m_contacts;
}

void Scenario::setContacts(const QString& _contacts)
{
	if (m_contacts != _contacts) {
		m_contacts = _contacts;

		changesNotStored();
	}
}

QString Scenario::year() const
{
	return m_year;
}

void Scenario::setYear(const QString& _year)
{
	if (m_year != _year) {
		m_year = _year;

		changesNotStored();
	}
}

QString Scenario::synopsis() const
{
	return m_synopsis;
}

void Scenario::setSynopsis(const QString& _synopsis)
{
	if (m_synopsis != _synopsis) {
		m_synopsis = _synopsis;

		changesNotStored();
	}
}

QString Scenario::text() const
{
	return m_text;
}

void Scenario::setText(const QString& _text)
{
	if (m_text != _text) {
		m_text = _text;

		changesNotStored();
	}
}

bool Scenario::isDraft() const
{
	return m_isDraft;
}

void Scenario::setIsDraft(bool _isDraft)
{
	if (m_isDraft != _isDraft) {
		m_isDraft = _isDraft;

		changesNotStored();
	}
}

QDateTime Scenario::versionStartDatetime() const
{
	return m_versionStartDatetime;
}

void Scenario::setVersionStartDatetime(const QDateTime& _datetime)
{
	if (m_versionStartDatetime != _datetime) {
		m_versionStartDatetime = _datetime;

		changesNotStored();
	}
}

QDateTime Scenario::versionEndDatetime() const
{
	return m_versionEndDatetime;
}

void Scenario::setVersionEndDatetime(const QDateTime& _datetime)
{
	if (m_versionEndDatetime != _datetime) {
		m_versionEndDatetime = _datetime;

		changesNotStored();
	}
}

QString Scenario::versionComment() const
{
	return m_versionComment;
}

void Scenario::setVersionComment(const QString& _comment)
{
	if (m_versionComment != _comment) {
		m_versionComment = _comment;

		changesNotStored();
	}
}

QString Scenario::uuid() const
{
	return m_uuid;
}

void Scenario::setUuid(const QString& _uuid)
{
	if (m_uuid != _uuid) {
		m_uuid = _uuid;

		changesNotStored();
	}
}

bool Scenario::isSynced() const
{
	return m_isSynced;
}

void Scenario::setIsSynced(bool _isSynced)
{
	if (m_isSynced != _isSynced) {
		m_isSynced = _isSynced;

		changesNotStored();
	}
}

// ****

namespace {
	const int COLUMN_COUNT = 3;
}

ScenariosTable::ScenariosTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int ScenariosTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant ScenariosTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		Scenario* scenario = dynamic_cast<Scenario*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Name: {
				resultData = scenario->name();
				break;
			}
			case Synopsis: {
				resultData = scenario->synopsis();
				break;
			}
			case Text: {
				resultData = scenario->text();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

ScenariosTable::Column ScenariosTable::sectionToColumn(int _section) const
{
	Column column = Undefined;

	switch (_section) {
		case 0: {
			column = Name;
			break;
		}
		case 1: {
			column = Synopsis;
			break;
		}
		case 2: {
			column = Text;
			break;
		}
		default: {
			break;
		}
	}

	return column;
}

#include "Scenario.h"

using namespace Domain;


Scenario::Scenario(const Identifier& _id, const QString& _name,
	const QString& _synopsis, const QString& _text) :
	DomainObject(_id),
	m_name(_name),
	m_synopsis(_synopsis),
	m_text(_text),
	m_isDraft(false)
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

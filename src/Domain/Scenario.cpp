#include "Scenario.h"

using namespace Domain;


Scenario::Scenario(
		const Identifier& _id,
		const QString& _name,
		const QString& _synopsis,
		const QString& _text
		) :
	DomainObject(_id),
	m_name(_name),
	m_synopsis(_synopsis),
	m_text(_text)
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

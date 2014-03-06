#include "Character.h"

using namespace Domain;


Character::Character(const Identifier& _id, const QString& _name) :
	DomainObject(_id),
	m_name(_name)
{
}

QString Character::name() const
{
	return m_name;
}

void Character::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;
	}
}

// ****

namespace {
	const int COLUMN_COUNT = 1;
}

CharactersTable::CharactersTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int CharactersTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant CharactersTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		Character* character = dynamic_cast<Character*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Name: {
				resultData = character->name();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

CharactersTable::Column CharactersTable::sectionToColumn(int _section) const
{
	Column column = Undefined;

	switch (_section) {
		case 0: {
			column = Name;
			break;
		}

		default: {
			break;
		}
	}

	return column;
}

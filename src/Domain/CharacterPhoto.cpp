#include "CharacterPhoto.h"

#include "Character.h"

using namespace Domain;


CharacterPhoto::CharacterPhoto(
		const Identifier& _id,
		Character* _character,
		const QPixmap& _photo,
		int _sortOrder
		) :
	DomainObject(_id),
	m_character(_character),
	m_photo(_photo),
	m_sortOrder(_sortOrder)
{
}

Character* CharacterPhoto::character() const
{
	return m_character;
}

void CharacterPhoto::setCharacter(Character* _character)
{
	if (m_character != _character) {
		m_character = _character;
	}
}

QPixmap CharacterPhoto::photo() const
{
	return m_photo;
}

void CharacterPhoto::setPhoto(const QPixmap& _photo)
{
	m_photo = _photo;
}

int CharacterPhoto::sortOrder() const
{
	return m_sortOrder;
}

void CharacterPhoto::setSortOrder(int _sortOrder)
{
	if (m_sortOrder != _sortOrder) {
		m_sortOrder = _sortOrder;
	}
}

// ****

namespace {
	const int COLUMN_COUNT = 2;
}

CharacterPhotosTable::CharacterPhotosTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{

}

void CharacterPhotosTable::setCharacter(Domain::Character* _character)
{
	foreach(DomainObject* domainObject, domainObjects()) {
		CharacterPhoto* photo = dynamic_cast<CharacterPhoto*>(domainObject);
		photo->setCharacter(_character);
	}
}

int CharacterPhotosTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant CharacterPhotosTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject* domainObject = domainObjects().value(_index.row());
		CharacterPhoto* photo = dynamic_cast<CharacterPhoto*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Photo: {
				resultData = photo->photo();
				break;
			}

			case SortOrder: {
				resultData = photo->sortOrder();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

CharacterPhotosTable::Column CharacterPhotosTable::sectionToColumn(const int _section) const
{
	Column column = Undefined;

	switch (_section) {
		case 0: {
			column = Photo;
			break;
		}

		case 1: {
			column = SortOrder;
			break;
		}

		default: {
			break;
		}
	}

	return column;
}

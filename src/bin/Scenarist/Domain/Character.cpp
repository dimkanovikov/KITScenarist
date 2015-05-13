#include "Character.h"

#include "CharacterPhoto.h"

using namespace Domain;


Character::Character(
		const Identifier& _id,
		const QString& _name,
		const QString& _realName,
		const QString& _description,
		CharacterPhotosTable* _photos
		) :
	DomainObject(_id),
	m_name(_name),
	m_realName(_realName),
	m_description(_description),
	m_photos(_photos)
{
	m_photos->setCharacter(this);
}

QString Character::name() const
{
	return m_name;
}

void Character::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;

		changesNotStored();
	}
}

QString Character::realName() const
{
	return m_realName;
}

void Character::setRealName(const QString& _realName)
{
	if (m_realName != _realName) {
		m_realName = _realName;

		changesNotStored();
	}
}

QString Character::description() const
{
	return m_description;
}

void Character::setDescription(const QString& _description)
{
	if (m_description != _description) {
		m_description = _description;

		changesNotStored();
	}
}

CharacterPhotosTable* Character::photosTable() const
{
	return m_photos;
}

QList<QPixmap> Character::photos() const
{
	QList<QPixmap> photos;
	foreach (DomainObject* domainObject, m_photos->toList()) {
		CharacterPhoto* photo = dynamic_cast<CharacterPhoto*>(domainObject);
		photos.insert(photo->sortOrder(), photo->photo());
	}
	return photos;
}

void Character::setPhotosTable(CharacterPhotosTable* _photos)
{
	if (m_photos != _photos) {
		m_photos->clear();

		foreach (DomainObject* photo, _photos->toList()) {
			m_photos->append(photo);
		}
	}
}

void Character::setPhotos(const QList<QPixmap>& _photos)
{
	m_photos->clear();

	for (int index = 0; index < _photos.count(); ++index) {
		CharacterPhoto* newPhoto = new CharacterPhoto(Identifier(), this, _photos.value(index), index);
		m_photos->append(newPhoto);
	}

	changesNotStored();
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

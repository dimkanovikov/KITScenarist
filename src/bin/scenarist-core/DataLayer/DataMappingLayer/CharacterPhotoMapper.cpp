#include "CharacterPhotoMapper.h"

#include <Domain/Character.h>
#include <Domain/CharacterPhoto.h>

#include <3rd_party/Helpers/ImageHelper.h>

#include <QBuffer>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, fk_character_id, photo, sort_order ";
	const QString TABLE_NAME = " characters_photo ";
}

CharacterPhoto* CharacterPhotoMapper::find(const Identifier& _id)
{
	return dynamic_cast<CharacterPhoto*>(abstractFind(_id));
}

CharacterPhotosTable* CharacterPhotoMapper::findAll()
{
	return qobject_cast<CharacterPhotosTable*>(abstractFindAll());
}

CharacterPhotosTable* CharacterPhotoMapper::findAllForCharacter(const Identifier& _characterIdentifier)
{
	//
	// Фильтр по связанным локациям
	//
	QString filter = QString(" WHERE fk_character_id = %1 ").arg(_characterIdentifier.value());

	return qobject_cast<CharacterPhotosTable*>(abstractFindAll(filter));
}

void CharacterPhotoMapper::insert(CharacterPhoto* _character)
{
	abstractInsert(_character);
}

void CharacterPhotoMapper::update(CharacterPhoto* _character)
{
	abstractUpdate(_character);
}

void CharacterPhotoMapper::remove(CharacterPhoto* _character)
{
	abstractDelete(_character);
}

QString CharacterPhotoMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString CharacterPhotoMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString CharacterPhotoMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, fk_character_id, photo, sort_order) "
					" VALUES(?, ?, ?, ?) "
					);

	CharacterPhoto* photo = dynamic_cast<CharacterPhoto*>(_subject);
	_insertValues.clear();
	_insertValues.append(photo->id().value());
	_insertValues.append(photo->character()->id().value());
	_insertValues.append(ImageHelper::bytesFromImage(photo->photo()));
	_insertValues.append(photo->sortOrder());

	return insertStatement;
}

QString CharacterPhotoMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET fk_character_id = ?, "
					" photo = ?, "
					" sort_order = ? "
					" WHERE id = ? "
					);

	CharacterPhoto* photo = dynamic_cast<CharacterPhoto*>(_subject);
	_updateValues.clear();
	_updateValues.append(photo->character()->id().value());
	_updateValues.append(ImageHelper::bytesFromImage(photo->photo()));
	_updateValues.append(photo->sortOrder());
	_updateValues.append(photo->id().value());

	return updateStatement;
}

QString CharacterPhotoMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* CharacterPhotoMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	//
	// Персонажи не загружаются чтобы не вызывать циклической инициилизации данных,
	// связывание фотографий с персонажами осуществляется посредством метода findAllForCharacter
	//
	Character* character = 0;
	const QPixmap photo = ImageHelper::imageFromBytes(_record.value("photo").toByteArray());
	const int sortOrder = _record.value("sort_order").toInt();

	return new CharacterPhoto(_id, character, photo, sortOrder);
}

void CharacterPhotoMapper::doLoad(DomainObject* _domainObject, const QSqlRecord& _record)
{
	if (CharacterPhoto* characterPhoto = dynamic_cast<CharacterPhoto*>(_domainObject)) {
		const QPixmap photo = ImageHelper::imageFromBytes(_record.value("photo").toByteArray());
		characterPhoto->setPhoto(photo);

		const int sortOrder = _record.value("sort_order").toInt();
		characterPhoto->setSortOrder(sortOrder);
	}
}

DomainObjectsItemModel* CharacterPhotoMapper::modelInstance()
{
	return new CharacterPhotosTable;
}

CharacterPhotoMapper::CharacterPhotoMapper()
{
}

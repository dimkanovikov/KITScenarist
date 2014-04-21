#include "CharacterPhotoMapper.h"

#include <Domain/Character.h>
#include <Domain/CharacterPhoto.h>

#include <QBuffer>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, fk_character_id, photo, sort_order ";
	const QString TABLE_NAME = " characters_photo ";

	const char* PHOTO_FILE_FORMAT = "PNG";
	const int PHOTO_FILE_QUALITY = 100;

	/**
	 * @brief Сохранение фотографии в массив байт
	 */
	static QByteArray bytesFromPhoto(const QPixmap& _photo) {
		QByteArray photoData;
		QBuffer photoBuffer(&photoData);
		photoBuffer.open(QIODevice::WriteOnly);
		_photo.save(&photoBuffer, PHOTO_FILE_FORMAT, PHOTO_FILE_QUALITY);
		return photoData;
	}

	/**
	 * @brief Загрузить фото из массива байт
	 */
	static QPixmap photoFromBytes(const QByteArray& _bytes) {
		QPixmap photo;
		photo.loadFromData(_bytes);
		return photo;
	}
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
	_insertValues.append(bytesFromPhoto(photo->photo()));
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
	_updateValues.append(bytesFromPhoto(photo->photo()));
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
	// Локации не загружаются чтобы не вызывать циклической инициилизации данных,
	// связывание фотографий с локациями осуществляется посредством метода findAllForCharacter
	//
	Character* character = 0;
	QPixmap photo = photoFromBytes(_record.value("photo").toByteArray());
	int sortOrder = _record.value("sort_order").toInt();

	return new CharacterPhoto(_id, character, photo, sortOrder);
}

DomainObjectsItemModel* CharacterPhotoMapper::modelInstance()
{
	return new CharacterPhotosTable;
}

CharacterPhotoMapper::CharacterPhotoMapper()
{
}

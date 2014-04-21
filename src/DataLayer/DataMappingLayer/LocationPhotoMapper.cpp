#include "LocationPhotoMapper.h"

#include <Domain/Location.h>
#include <Domain/LocationPhoto.h>

#include <QBuffer>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, fk_location_id, photo, sort_order ";
	const QString TABLE_NAME = " locations_photo ";

	const char* PHOTO_FILE_FORMAT = "JPG";
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

LocationPhoto* LocationPhotoMapper::find(const Identifier& _id)
{
	return dynamic_cast<LocationPhoto*>(abstractFind(_id));
}

LocationPhotosTable* LocationPhotoMapper::findAll()
{
	return qobject_cast<LocationPhotosTable*>(abstractFindAll());
}

LocationPhotosTable* LocationPhotoMapper::findAllForLocation(const Identifier& _locationIdentifier)
{
	//
	// Фильтр по связанным локациям
	//
	QString filter = QString(" WHERE fk_location_id = %1 ").arg(_locationIdentifier.value());

	return qobject_cast<LocationPhotosTable*>(abstractFindAll(filter));
}

void LocationPhotoMapper::insert(LocationPhoto* _location)
{
	abstractInsert(_location);
}

void LocationPhotoMapper::update(LocationPhoto* _location)
{
	abstractUpdate(_location);
}

void LocationPhotoMapper::remove(LocationPhoto* _location)
{
	abstractDelete(_location);
}

QString LocationPhotoMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString LocationPhotoMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString LocationPhotoMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, fk_location_id, photo, sort_order) "
					" VALUES(?, ?, ?, ?) "
					);

	LocationPhoto* photo = dynamic_cast<LocationPhoto*>(_subject);
	_insertValues.clear();
	_insertValues.append(photo->id().value());
	_insertValues.append(photo->location()->id().value());
	_insertValues.append(bytesFromPhoto(photo->photo()));
	_insertValues.append(photo->sortOrder());

	return insertStatement;
}

QString LocationPhotoMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET fk_location_id = ?, "
					" photo = ?, "
					" sort_order = ? "
					" WHERE id = ? "
					);

	LocationPhoto* photo = dynamic_cast<LocationPhoto*>(_subject);
	_updateValues.clear();
	_updateValues.append(photo->location()->id().value());
	_updateValues.append(bytesFromPhoto(photo->photo()));
	_updateValues.append(photo->sortOrder());
	_updateValues.append(photo->id().value());

	return updateStatement;
}

QString LocationPhotoMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* LocationPhotoMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	//
	// Локации не загружаются чтобы не вызывать циклической инициилизации данных,
	// связывание фотографий с локациями осуществляется посредством метода findAllForLocation
	//
	Location* location = 0;
	QPixmap photo = photoFromBytes(_record.value("photo").toByteArray());
	int sortOrder = _record.value("sort_order").toInt();

	return new LocationPhoto(_id, location, photo, sortOrder);
}

DomainObjectsItemModel* LocationPhotoMapper::modelInstance()
{
	return new LocationPhotosTable;
}

LocationPhotoMapper::LocationPhotoMapper()
{
}

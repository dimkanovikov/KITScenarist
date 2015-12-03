#include "ResearchMapper.h"

#include <Domain/Research.h>

#include <3rd_party/Helpers/ImageHelper.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, parent_id, type, name, description, url, image, sort_order ";
	const QString TABLE_NAME = " research ";
}

Research* ResearchMapper::find(const Identifier& _id)
{
	return dynamic_cast<Research*>(abstractFind(_id));
}

ResearchTable* ResearchMapper::findAll()
{
	//
	// Настраиваем порядок сортировки, т.к. он важен при построении дерева разработки
	//
	const QString sortQuery = " ORDER BY parent_id, sort_order";

	return qobject_cast<ResearchTable*>(abstractFindAll(sortQuery));
}

void ResearchMapper::insert(Research* _research)
{
	abstractInsert(_research);
}

void ResearchMapper::update(Research* _research)
{
	abstractUpdate(_research);
}

void ResearchMapper::remove(Research* _research)
{
	abstractDelete(_research);
}

QString ResearchMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString ResearchMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString ResearchMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (" + COLUMNS + ") "
					" VALUES(?, ?, ?, ?, ?, ?, ?, ?) "
					);

	Research* research = dynamic_cast<Research*>(_subject );
	_insertValues.clear();
	_insertValues.append(research->id().value());
	_insertValues.append((research->parent() == 0 || !research->parent()->id().isValid()) ? QVariant() : research->parent()->id().value());
	_insertValues.append(research->type());
	_insertValues.append(research->name());
	_insertValues.append(research->description());
	_insertValues.append(research->url());
	_insertValues.append(ImageHelper::bytesFromImage(research->image()));
	_insertValues.append(research->sortOrder());

	return insertStatement;
}

QString ResearchMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET parent_id = ?, "
					" type = ?, "
					" name = ?, "
					" description = ?, "
					" url = ?, "
					" image = ?, "
					" sort_order =? "
					" WHERE id = ? "
					);

	Research* research = dynamic_cast<Research*>(_subject);
	_updateValues.clear();
	_updateValues.append((research->parent() == 0 || !research->parent()->id().isValid()) ? QVariant() : research->parent()->id().value());
	_updateValues.append(research->type());
	_updateValues.append(research->name());
	_updateValues.append(research->description());
	_updateValues.append(research->url());
	_updateValues.append(ImageHelper::bytesFromImage(research->image()));
	_updateValues.append(research->sortOrder());
	_updateValues.append(research->id().value());

	return updateStatement;
}

QString ResearchMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* ResearchMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	Research* parent = 0;
	if (!_record.value("parent_id").isNull()) {
		parent = find(Identifier(_record.value("parent_id").toInt()));
	}
	const Research::Type type = (Research::Type)_record.value("type").toInt();
	const QString name = _record.value("name").toString();
	const QString description = _record.value("description").toString();
	const QString url = _record.value("url").toString();
	const QPixmap image = ImageHelper::imageFromBytes(_record.value("image").toByteArray());
	const int sortOrder = _record.value("sort_order").toInt();

	return new Research(_id, parent, type, sortOrder, name, description, url, image);
}

void ResearchMapper::doLoad(DomainObject* _domainObject, const QSqlRecord& _record)
{
	if (Research* research = dynamic_cast<Research*>(_domainObject)) {
		Research* parent = 0;
		if (!_record.value("parent_id").isNull()) {
			parent = find(Identifier(_record.value("parent_id").toInt()));
		}
		research->setParent(parent);

		const Research::Type type = (Research::Type)_record.value("type").toInt();
		research->setType(type);

		const QString name = _record.value("name").toString();
		research->setName(name);

		const QString description = _record.value("description").toString();
		research->setDescription(description);

		const QString url = _record.value("url").toString();
		research->setUrl(url);

		const QPixmap image = ImageHelper::imageFromBytes(_record.value("image").toByteArray());
		research->setImage(image);

		const int sortOrder = _record.value("sort_order").toInt();
		research->setSortOrder(sortOrder);
	}
}

DomainObjectsItemModel* ResearchMapper::modelInstance()
{
	return new ResearchTable;
}

ResearchMapper::ResearchMapper()
{
}

#include "DomainObject.h"

using namespace Domain;


bool DomainObject::isValid(const DomainObject* _object)
{
	bool isValid = (_object != 0);
	return isValid;
}


DomainObject::DomainObject() :
	m_id(Identifier()),
	m_isChangesStored(false)
{
}

DomainObject::DomainObject(Identifier _id) :
	m_id(_id),
	m_isChangesStored(_id.isValid() ? true : false)
{
}

DomainObject::~DomainObject()
{
}

Identifier DomainObject::id() const
{
	return m_id;
}

void DomainObject::setId(const Identifier& _id)
{
	if (!_id.isValid()
		|| m_id != _id) {
		m_id = _id;
	}
}

bool DomainObject::isChangesStored() const
{
	return m_isChangesStored;
}

void DomainObject::changesStored()
{
	m_isChangesStored = true;
}

void DomainObject::changesNotStored()
{
	m_isChangesStored = false;
}

// ****

DomainObjectsItemModel::DomainObjectsItemModel(QObject* _parent) :
	QAbstractItemModel(_parent)
{

}

QModelIndex DomainObjectsItemModel::index(int _row, int _column, const QModelIndex &_parent) const
{
	QModelIndex resultIndex;
	if (_row < 0
		|| _row > domainObjects().count()
		|| _column < 0
		|| _column > columnCount(_parent)) {
		resultIndex = QModelIndex();
	} else {
		DomainObject* indexItem = domainObjects().value(_row);
		resultIndex = createIndex(_row, _column, indexItem);
	}
	return resultIndex;
}

QModelIndex DomainObjectsItemModel::parent(const QModelIndex&) const
{
	return QModelIndex();
}

int DomainObjectsItemModel::rowCount(const QModelIndex&) const
{
	return m_domainObjects.count();
}

int DomainObjectsItemModel::columnCount(const QModelIndex&) const
{
	return 0;
}

QVariant DomainObjectsItemModel::data(const QModelIndex&, int) const
{
	return QVariant();
}

DomainObject *DomainObjectsItemModel::itemForIndex(const QModelIndex &index) const
{
	return domainObjects().value(index.row());
}

QModelIndex DomainObjectsItemModel::indexForItem(DomainObject* _item) const
{
	return index(domainObjects().indexOf(_item), 0, QModelIndex());
}

QList<DomainObject*> DomainObjectsItemModel::toList() const
{
	return domainObjects();
}

int DomainObjectsItemModel::size() const
{
	return rowCount(QModelIndex());
}

bool DomainObjectsItemModel::contains(DomainObject* domainObject) const
{
	//
	//
	//
	bool contains = false;
	foreach (DomainObject* object, domainObjects()) {
		if (object->id() == domainObject->id()) {
			contains = true;
			break;
		}
	}

	return contains;
}

void DomainObjectsItemModel::clear()
{
	for (int index = m_domainObjects.count()-1; index >= 0; --index) {
		remove(m_domainObjects.value(index));
	}
}

void DomainObjectsItemModel::append(DomainObject* domainObject)
{
	emit beginInsertRows(QModelIndex(), size(), size());
	m_domainObjects.append( domainObject );
	emit endInsertRows();
}

void DomainObjectsItemModel::remove(DomainObject* domainObject)
{
	const int index = m_domainObjects.indexOf(domainObject);
	beginRemoveRows(QModelIndex(), index, index);
	m_domainObjects.removeOne( domainObject );
	endRemoveRows();
}

void DomainObjectsItemModel::itemChanged(DomainObject* _domainObject)
{
	const QModelIndex itemIndex = indexForItem(_domainObject);
	emit dataChanged(itemIndex, itemIndex);
}

QList<DomainObject*> DomainObjectsItemModel::domainObjects() const
{
	return m_domainObjects;
}

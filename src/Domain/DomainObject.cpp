#include "DomainObject.h"

using namespace Domain;


bool DomainObject::isValid(const DomainObject* _object)
{
	bool isValid = (_object != 0);
    return isValid;
}


DomainObject::DomainObject() :
	m_id(Identifier())
{
}

DomainObject::DomainObject(Identifier _id) :
	m_id(_id)
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

int DomainObjectsItemModel::count() const
{
    return domainObjects().count();
}

bool DomainObjectsItemModel::contains(DomainObject* domainObject) const
{
	return domainObjects().contains(domainObject);
}

void DomainObjectsItemModel::append(DomainObject* domainObject)
{
	emit beginInsertRows(QModelIndex(), count(), count());
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

QList<DomainObject*> DomainObjectsItemModel::domainObjects() const
{
    return m_domainObjects;
}

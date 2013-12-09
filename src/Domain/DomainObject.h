#ifndef DOMAINOBJECT_H
#define DOMAINOBJECT_H

#include "Identifier.h"

#include <QObject>
#include <QVariant>
#include <QAbstractItemModel>


namespace Domain
{
	enum ObjectState {
		New = 0,
		AboutArchived = 1,
		Archived = 2,
		AboutStored = 3,
		Stored = 4,
		AboutDeleted = 5,
		Deleted = 6
	};

	class DomainObject
	{
	public:
		static bool isValid(const DomainObject* _object);

	public:
		DomainObject();
		DomainObject(Identifier _id);
		virtual ~DomainObject();

	public:
		Identifier id() const;
		void setId(const Identifier& _id);

	private:
		Identifier m_id;
	};

	//******

	class DomainObjectsItemModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		explicit DomainObjectsItemModel(QObject* _parent = 0);

	public:
		virtual QModelIndex index(int _row, int _column, const QModelIndex& _parent) const;
		virtual QModelIndex parent(const QModelIndex &) const;
		virtual int rowCount(const QModelIndex&) const;
		virtual int columnCount(const QModelIndex&) const;
		virtual QVariant data(const QModelIndex&, int) const;
		virtual DomainObject* itemForIndex(const QModelIndex&) const;
		QList<DomainObject*> toList() const;
		int count() const;
		bool contains(DomainObject*) const;

	public:
		virtual void append(DomainObject*);
		void remove(DomainObject*);

	protected:
		QList<DomainObject*> domainObjects() const;

	private:
		QList<DomainObject*> m_domainObjects;
	};
}

#endif // DOMAINOBJECT_H

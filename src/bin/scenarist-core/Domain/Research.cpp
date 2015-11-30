#include "Research.h"

using namespace Domain;


Research::Research(const Identifier& _id, Research* _parent, Research::Type _type,
	const QString& _name, const QString& _description, const QString& _url, int _sortOrder) :
	DomainObject(_id),
	m_parent(_parent),
	m_type(_type),
	m_name(_name),
	m_description(_description),
	m_url(_url),
	m_sortOrder(_sortOrder)
{
}

Research* Research::parent() const
{
	return m_parent;
}

void Research::setParent(Research* _parent)
{
	if (m_parent != _parent) {
		m_parent = _parent;

		changesNotStored();
	}
}

Research::Type Research::type() const
{
	return m_type;
}

void Research::setType(Research::Type _type)
{
	if (m_type != _type) {
		m_type = _type;

		changesNotStored();
	}
}

QString Research::name() const
{
	return m_name;
}

void Research::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;

		changesNotStored();
	}
}

QString Research::description() const
{
	return m_description;
}

void Research::setDescription(const QString& _description)
{
	if (m_description != _description) {
		m_description = _description;

		changesNotStored();
	}
}

QString Research::url() const
{
	return m_url;
}

void Research::setUrl(const QString& _url)
{
	if (m_url != _url) {
		m_url = _url;

		changesNotStored();
	}
}

int Research::sortOrder() const
{
	return m_sortOrder;
}

void Research::setSortOrder(int _sortOrder)
{
	if (m_sortOrder != _sortOrder) {
		m_sortOrder = _sortOrder;

		changesNotStored();
	}
}

// ****


namespace {
	const int COLUMN_COUNT = 1;
}

ResearchTable::ResearchTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int ResearchTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant ResearchTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		Research* research = dynamic_cast<Research*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Name: {
				resultData = research->name();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

ResearchTable::Column ResearchTable::sectionToColumn(int _section) const
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

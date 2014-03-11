#ifndef LOCATION_H
#define LOCATION_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	/**
	 * @brief Класс локации
	 */
	class Location : public DomainObject
	{
	public:
		Location(const Identifier& _id, const QString& _name);

		/**
		 * @brief Получить название локации
		 */
		QString name() const;

	private:
		/**
		 * @brief Название локации
		 */
		QString m_name;
	};

	// ****

	class LocationsTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit LocationsTable(QObject* _parent = 0);

	public:
		enum Column {
			Undefined,
			Id,
			Name
		};

	public:
		int columnCount(const QModelIndex&) const;
		QVariant data(const QModelIndex& _index, int _role) const;

	private:
		Column sectionToColumn(int _section) const;
	};
}

#endif // LOCATION_H

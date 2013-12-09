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
		Location(const Identifier& _id, Location* _parentLocation, const QString& _name);

		/**
		 * @brief Получить родительскую локацию
		 */
		Location* parentLocation() const;

		/**
		 * @brief Получить название локации
		 */
		QString name() const;

	private:

		/**
		 * @brief Родительская локация
		 */
		Location* m_parentLocation;
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
			ParentLocation,
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

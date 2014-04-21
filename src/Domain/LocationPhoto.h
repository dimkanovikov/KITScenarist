#ifndef LOCATIONPHOTO_H
#define LOCATIONPHOTO_H

#include "DomainObject.h"

#include <QPixmap>


namespace Domain
{
	class Location;

	/**
	 * @brief Класс фотографии локации
	 */
	class LocationPhoto : public DomainObject
	{
	public:
		LocationPhoto(
				const Identifier& _id,
				Location* _location,
				const QPixmap& _photo,
				int _sortOrder
				);

		Location* location() const;
		void setLocation(Location* _location);

		QPixmap photo() const;
		void setPhoto(const QPixmap& _photo);

		int sortOrder() const;
		void setSortOrder(int _sortOrder);

	private:
		/**
		 * @brief Локация к которой относится фотографии
		 */
		Location* m_location;

		/**
		 * @brief Собственно фотография
		 */
		QPixmap m_photo;

		/**
		 * @brief Порядок следования фотографии
		 */
		int m_sortOrder;
	};

	// ****

	class LocationPhotosTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit LocationPhotosTable(QObject* _parent = 0 );

	public:
		enum Column {
			Undefined,
			Id,
			Location,
			Photo,
			SortOrder
		};

	public:
		void setLocation(Domain::Location* _location);

	public:
		int columnCount(const QModelIndex&) const;
		QVariant data(const QModelIndex& _index, int _role) const;

	private:
		Column sectionToColumn(const int _section) const;
	};
}

#endif // LOCATIONPHOTO_H

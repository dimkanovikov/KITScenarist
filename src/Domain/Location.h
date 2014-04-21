#ifndef LOCATION_H
#define LOCATION_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	class LocationPhotosTable;


	/**
	 * @brief Класс локации
	 */
	class Location : public DomainObject
	{
	public:
		Location(const Identifier& _id, const QString& _name, const QString& _description, LocationPhotosTable* _photos);

		/**
		 * @brief Получить название локации
		 */
		QString name() const;

		/**
		 * @brief Установить название
		 */
		void setName(const QString& _name);

		/**
		 * @brief Получить описание локации
		 */
		QString description() const;

		/**
		 * @brief Установить описание локации
		 */
		void setDescription(const QString& _description);

		/**
		 * @brief Получить фотографии локации
		 */
		LocationPhotosTable* photosTable() const;

		/**
		 * @brief Установить фотографии локации
		 */
		void setPhotos(const QList<QPixmap>& _photos);

		/**
		 * @brief Получить фотографии локации
		 */
		QList<QPixmap> photos() const;

	private:
		/**
		 * @brief Название локации
		 */
		QString m_name;

		/**
		 * @brief Описание
		 * @note Html-форматированный текст
		 */
		QString m_description;

		/**
		 * @brief Фотографии
		 */
		LocationPhotosTable* m_photos;
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

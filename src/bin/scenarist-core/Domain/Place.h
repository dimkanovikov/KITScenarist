#ifndef PLACE_H
#define PLACE_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	/**
	 * @brief Класс места съёмок
	 */
	class Place : public DomainObject
	{
	public:
		Place(const Identifier& _id, const QString& _name);

		/**
		 * @brief Получить название места
		 */
		QString name() const;

		/**
		 * @brief Установить название места
		 */
		void setName(const QString& _name);

	private:
		/**
		 * @brief Название места
		 */
		QString m_name;
	};

	// ****

	class PlacesTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit PlacesTable(QObject* _parent = 0);

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

#endif // PLACE_H

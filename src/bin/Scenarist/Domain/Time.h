#ifndef TIME_H
#define TIME_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	/**
	 * @brief Класс времени съёмок
	 */
	class Time : public DomainObject
	{
	public:
		Time(const Identifier& _id, const QString& _name);

		/**
		 * @brief Получить название времени
		 */
		QString name() const;

		/**
		 * @brief Установить название времени
		 */
		void setName(const QString& _name);

	private:
		/**
		 * @brief Название времени
		 */
		QString m_name;
	};

	// ****

	class TimesTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit TimesTable(QObject* _parent = 0);

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

#endif // TIME_H

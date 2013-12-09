#ifndef SCENARIODAY_H
#define SCENARIODAY_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	/**
	 * @brief Класс сценарного дня
	 */
	class ScenarioDay : public DomainObject
	{
	public:
		ScenarioDay(const Identifier& _id, const QString& _name);

		/**
		 * @brief Получить название сценарного дня
		 */
		QString name() const;

	private:
		/**
		 * @brief Название сценарного дня
		 */
		QString m_name;
	};

	// ****

	class ScenarioDaysTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit ScenarioDaysTable(QObject* _parent = 0);

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

#endif // SCENARIODAY_H

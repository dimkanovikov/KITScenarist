#ifndef SCENARIODATA_H
#define SCENARIODATA_H

#include "DomainObject.h"

class QString;


namespace Domain
{
	/**
	 * @brief Класс данных сценария
	 */
	class ScenarioData : public DomainObject
	{
	public:
		/**
		 * @brief Ключи для доступа к данным сценария
		 */
		/** @{ */
		static const QString NAME_KEY;
		static const QString LOGLINE_KEY;
		static const QString ADDITIONAL_INFO_KEY;
		static const QString GENRE_KEY;
		static const QString AUTHOR_KEY;
		static const QString CONTACTS_KEY;
		static const QString YEAR_KEY;
		static const QString SYNOPSIS_KEY;
		/** @} */

	public:
		ScenarioData(const Identifier& _id, const QString& _name, const QString& _value);

		/**
		 * @brief Название
		 */
		QString name() const;

		/**
		 * @brief Значение
		 */
		/** @{ */
		QString value() const;
		void setValue(const QString& _value);
		/** @} */

	private:
		/**
		 * @brief Название
		 */
		QString m_name;

		/**
		 * @brief Значение
		 */
		QString m_value;
	};

	// ****

	/**
	 * @brief Таблица данных сценария
	 */
	class ScenarioDataTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit ScenarioDataTable(QObject* _parent = 0);

	public:
		enum Column {
			Undefined,
			Id,
			Name,
			Value
		};

	public:
		int columnCount(const QModelIndex&) const;
		QVariant data(const QModelIndex& _index, int _role) const;

		/**
		 * @brief Получить данные по имени
		 */
		ScenarioData* scenarioData(const QString& _name) const;

	private:
		Column sectionToColumn(int _section) const;
	};
}

#endif // SCENARIODATA_H

#ifndef SCENARIO_H
#define SCENARIO_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	/**
	 * @brief Класс сценария
	 */
	class Scenario : public DomainObject
	{
	public:
		Scenario(const Identifier& _id,
				 const QString& _name,
				 const QString& _synopsis,
				 const QString& _text);

		/**
		 * @brief Получить название сценария
		 */
		QString name() const;

		/**
		 * @brief Установить название сценария
		 */
		void setName(const QString& _name);

		/**
		 * @brief Получить синопсис сценария
		 */
		QString synopsis() const;

		/**
		 * @brief Установить синопсис сценария
		 */
		void setSynopsis(const QString& _synopsis);

		/**
		 * @brief Получить текст сценария
		 */
		QString text() const;

		/**
		 * @brief Установить текст сценария
		 */
		void setText(const QString& _text);

	private:
		/**
		 * @brief Название
		 */
		QString m_name;

		/**
		 * @brief Синопсис
		 */
		QString m_synopsis;

		/**
		 * @brief Текст сценария
		 */
		QString m_text;
	};

	// ****

	class ScenariosTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit ScenariosTable(QObject* _parent = 0);

	public:
		enum Column {
			Undefined,
			Id,
			Name,
			Synopsis,
			Text
		};

	public:
		int columnCount(const QModelIndex&) const;
		QVariant data(const QModelIndex& _index, int _role) const;

	private:
		Column sectionToColumn(int _section) const;
	};
}

#endif // SCENARIO_H

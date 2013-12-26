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
		Scenario(const Identifier& _id, const QString& _text);

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

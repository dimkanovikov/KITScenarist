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
		Scenario(const Identifier& _id, const QString& _text, bool _isDraft);

		/**
		 * @brief Текст сценария
		 */
		/** @{ */
		QString text() const;
		void setText(const QString& _text);
		/** @} */

		/**
		 * @brief Является ли сценарий черновиком
		 */
		/** @{ */
		bool isDraft() const;
		void setIsDraft(bool _isDraft);

	private:
		/**
		 * @brief Текст сценария
		 */
		QString m_text;

		/**
		 * @brief Является ли сценарий черновиком
		 */
		bool m_isDraft;
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

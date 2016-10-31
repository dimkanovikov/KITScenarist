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
		Scenario(const Identifier& _id, const QString& _scheme, const QString& _text, bool _isDraft);

		/**
		 * @brief Схема сценария
		 */
		/** @{ */
		QString scheme() const;
		void setScheme(const QString _scheme);
		/** @} */

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
		 * @brief Схема сценария
		 */
		QString m_scheme;

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
			Scheme,
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

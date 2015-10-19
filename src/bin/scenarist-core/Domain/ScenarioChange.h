#ifndef SCENARIOCHANGE_H
#define SCENARIOCHANGE_H

#include "DomainObject.h"

#include <QDateTime>
#include <QUuid>


namespace Domain
{
	/**
	 * @brief Класс атома изменения сценария
	 */
	class ScenarioChange : public DomainObject
	{
	public:
		ScenarioChange(const Identifier& _id, const QUuid& _uuid, const QDateTime& _datetime,
			const QString& _user, const QString& _undoPatch, const QString& _redoPatch, bool _isDraft);

		/**
		 * @brief Уникальный айди
		 */
		/** @{ */
		QUuid uuid() const;
		void setUuid(const QUuid& _uuid);
		/** @} */

		/**
		 * @brief Дата и время изменения
		 */
		/** @{ */
		QDateTime datetime() const;
		void setDatetime(const QDateTime& _datetime);
		/** @} */

		/**
		 * @brief Пользователь совершивший изменение
		 */
		/** @{ */
		QString user() const;
		void setUser(const QString& _user);
		/** @} */

		/**
		 * @brief Патч для отмены изменения
		 */
		/** @{ */
		QString undoPatch() const;
		void setUndoPatch(const QString& _patch);
		/** @} */

		/**
		 * @brief Патч для повтора/наложения изменения
		 */
		/** @{ */
		QString redoPatch() const;
		void setRedoPatch(const QString& _patch);
		/** @} */

		/**
		 * @brief Изменение чистовика (0) или черновика (1)
		 */
		/** @{ **/
		bool isDraft() const;
		void setIsDraft(bool _isDraft);
		/** @} **/

	private:
		/**
		 * @brief Уникальный айди
		 */
		QUuid m_uuid;

		/**
		 * @brief Дата и время изменения
		 */
		QDateTime m_datetime;

		/**
		 * @brief Пользователь совершивший изменение
		 */
		QString m_user;

		/**
		 * @brief Патч для отмены изменения
		 */
		QString m_undoPatch;

		/**
		 * @brief Патч для повтора/наложения изменения
		 */
		QString m_redoPatch;

		/**
		 * @brief Изменение чистовика (0) или черновика (1)
		 */
		bool m_isDraft;
	};

	// ****

	class ScenarioChangesTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit ScenarioChangesTable(QObject* _parent = 0);

	public:
		enum Column {
			Undefined,
			Id,
			Uuid
		};

	public:
		int columnCount(const QModelIndex&) const;
		QVariant data(const QModelIndex& _index, int _role) const;

	private:
		Column sectionToColumn(int _section) const;
	};
}

#endif // SCENARIOCHANGE_H

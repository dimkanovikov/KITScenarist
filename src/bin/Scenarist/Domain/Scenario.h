#ifndef SCENARIO_H
#define SCENARIO_H

#include "DomainObject.h"

#include <QDateTime>
#include <QString>


namespace Domain
{
	/**
	 * @brief Класс сценария
	 */
	class Scenario : public DomainObject
	{
	public:
		Scenario(const Identifier& _id, const QString& _name, const QString& _synopsis,
			const QString& _text, const QDateTime& _versionStartDatetime,
			const QDateTime& _versionEndDatetime, const QString& _versionComment,
			const QString& _uuid, bool _isSynced = false);

		/**
		 * @brief Название сценария
		 */
		/** @{ */
		QString name() const;
		void setName(const QString& _name);
		/** @} */

		/**
		 * @brief Дополнительная информация
		 */
		/** @{ */
		QString additionalInfo() const;
		void setAdditionalInfo(const QString& _additionalInfo);
		/** @} */

		/**
		 * @brief Жанр
		 */
		/** @{ */
		QString genre() const;
		void setGenre(const QString& _genre);
		/** @} */

		/**
		 * @brief Автор
		 */
		/** @{ */
		QString author() const;
		void setAuthor(const QString _author);
		/** @} */

		/**
		 * @brief Контактная информация
		 */
		/** @{ */
		QString contacts() const;
		void setContacts(const QString& _contacts);
		/** @} */

		/**
		 * @brief Год
		 */
		/** @{ */
		QString year() const;
		void setYear(const QString& _year);
		/** @} */

		/**
		 * @brief Синопсис сценария
		 */
		/** @{ */
		QString synopsis() const;
		void setSynopsis(const QString& _synopsis);
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
		/** @} */

		/**
		 * @brief Временные метки, начала и конца сессии работы над сценарием
		 */
		/** @{ */
		QDateTime versionStartDatetime() const;
		void setVersionStartDatetime(const QDateTime& _datetime);
		QDateTime versionEndDatetime() const;
		void setVersionEndDatetime(const QDateTime& _datetime);
		/** @} */

		/**
		 * @brief Комментарий к фиксируемой версии сценария
		 */
		/** @{ */
		QString versionComment() const;
		void setVersionComment(const QString& _comment);
		/** @} */

		/**
		 * @brief Uuid версии сценария
		 */
		/** @{ */
		QString uuid() const;
		void setUuid(const QString& _uuid);
		/** @} */

		/**
		 * @brief Синхронизирован ли сценарий с сервером
		 */
		/** @{ */
		bool isSynced() const;
		void setIsSynced(bool _isSynced);
		/** @} */

	private:
		/**
		 * @brief Название
		 */
		QString m_name;

		/**
		 * @brief Дополнительная информация
		 */
		QString m_additionalInfo;

		/**
		 * @brief Жанр
		 */
		QString m_genre;

		/**
		 * @brief Автор
		 */
		QString m_author;

		/**
		 * @brief Контакты
		 */
		QString m_contacts;

		/**
		 * @brief Год
		 */
		QString m_year;

		/**
		 * @brief Синопсис
		 */
		QString m_synopsis;

		/**
		 * @brief Текст сценария
		 */
		QString m_text;

		/**
		 * @brief Является ли сценарий черновиком
		 */
		bool m_isDraft;

		/**
		 * @brief Временные метки, начала и конца сессии работы над сценарием
		 */
		/** @{ */
		QDateTime m_versionStartDatetime;
		QDateTime m_versionEndDatetime;
		/** @} */

		/**
		 * @brief Комментарий к фиксируемой версии сценария
		 */
		QString m_versionComment;

		/**
		 * @brief Uuid версии сценария
		 */
		QString m_uuid;

		/**
		 * @brief Синхроинизирован ли сценарий с сервером
		 */
		bool m_isSynced;
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

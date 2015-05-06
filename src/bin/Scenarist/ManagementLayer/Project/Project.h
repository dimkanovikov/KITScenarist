#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QDateTime>


namespace ManagementLayer
{
	/**
	 * @brief Проект сценария
	 */
	class Project
	{
	public:
		/**
		 * @brief Тип проекта
		 */
		enum Type {
			Invalid,
			Local,
			Remote
		};

		/**
		 * @brief Роль пользователя в проекте
		 */
		enum Role {
			Owner,
			Redactor,
			Commentator
		};

		/**
		 * @brief Получить строку из роли
		 */
		static QString roleToString(Role _role);

		/**
		 * @brief Получить значение роли из строки
		 */
		static Role roleFromString(const QString& _role);

	public:
		Project();
		Project(Type _type, const QString& _name, const QString& _path,
			const QDateTime& _lastEditDatetime, int _id = 0, const QString& _owner = QString::null,
			Role _role = Owner);

		/**
		 * @brief Тип проекта
		 */
		Type type() const;

		/**
		 * @brief Это проект из облака?
		 */
		bool isRemote() const;

		/**
		 * @brief Отображаемое название проекта
		 */
		QString displayName() const;

		/**
		 * @brief Название проекта
		 */
		/** @{ */
		QString name() const;
		void setName(const QString& _name);
		/** @} */

		/**
		 * @brief Отображаемый путь к проекту
		 */
		QString displayPath() const;

		/**
		 * @brief Путь к проекту
		 */
		QString path() const;

		/**
		 * @brief Дата и время последнего изменения проекта
		 */
		/** @{ */
		QDateTime lastEditDatetime() const;
		void setLastEditDatetime(const QDateTime& _datetime);
		/** @} */

		/**
		 * @brief Идентификатор проекта
		 */
		int id() const;

	private:
		/**
		 * @brief Тип проекта
		 */
		Type m_type;

		/**
		 * @brief Название проекта
		 */
		QString m_name;

		/**
		 * @brief Путь к файлу проекта
		 */
		QString m_path;

		/**
		 * @brief Дата и время последнего изменения проекта
		 */
		QDateTime m_lastEditDatetime;

		/**
		 * @brief Идентификатор проекта (для проектов из облака)
		 */
		int m_id;

		/**
		 * @brief Логин владельца проекта (для проектов из облака)
		 */
		QString m_owner;

		/**
		 * @brief Роль пользователя в проекте (для проектов из облака)
		 */
		Role m_role;
	};

	/**
	 * @brief Сравнить два проекта
	 */
	bool operator==(const Project& _lhs, const Project& _rhs);
}

#endif // PROJECT_H

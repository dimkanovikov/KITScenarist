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
		Project(Type _type, const QString& _name, const QString& _path,
			const QDateTime& _lastEditDatetime, int _id = 0, const QString& _owner = QString::null,
			Role _role = Owner);

		/**
		 * @brief Название проекта
		 */
		QString name() const;

		/**
		 * @brief Путь к проекту
		 */
		QString path() const;

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
}

#endif // PROJECT_H

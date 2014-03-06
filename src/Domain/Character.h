#ifndef CHARACTER_H
#define CHARACTER_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	/**
	 * @brief Класс персонажа сценария
	 */
	class Character : public DomainObject
	{
	public:
		Character(const Identifier& _id, const QString& _name);

		/**
		 * @brief Получить имя персонажа
		 */
		QString name() const;

		/**
		 * @brief Установить имя
		 */
		void setName(const QString& _name);

	private:
		/*
		 * Параметры:
		 *
		 * История (должны быть стандартные ключи): наполняемый справочник ключ-значение [из базы данных]
		 * Фото: список фотографий (одна главная)
		 * Возраст (от и до): число
		 * Описание: текст
		 * Реквизит: : наполняемый список [из базы данных]
		 * Группа персонажей (хорошие, плохие): наполняемый список [из базы данных]
		 *
		 */


		QString m_name;
	};

	// ****

	class CharactersTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit CharactersTable(QObject* _parent = 0);

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

#endif // CHARACTER_H

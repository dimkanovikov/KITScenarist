#ifndef CHARACTER_H
#define CHARACTER_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	class CharacterPhotosTable;


	/**
	 * @brief Класс персонажа сценария
	 */
	class Character : public DomainObject
	{
	public:
		Character(const Identifier& _id,
				  const QString& _name,
				  const QString& _realName,
				  const QString& _description,
				  CharacterPhotosTable* _photos);

		/**
		 * @brief Получить имя персонажа
		 */
		QString name() const;

		/**
		 * @brief Установить имя
		 */
		void setName(const QString& _name);

		/**
		 * @brief Получить настоящее имя
		 */
		QString realName() const;

		/**
		 * @brief Установить настоящее имя
		 */
		void setRealName(const QString& _realName);

		/**
		 * @brief Получить описание локации
		 */
		QString description() const;

		/**
		 * @brief Установить описание локации
		 */
		void setDescription(const QString& _description);

		/**
		 * @brief Получить фотографии локации
		 */
		CharacterPhotosTable* photosTable() const;

		/**
		 * @brief Установить фотографии локации
		 */
		void setPhotos(const QList<QPixmap>& _photos);

		/**
		 * @brief Получить фотографии локации
		 */
		QList<QPixmap> photos() const;

	private:
		/*
		 * Параметры:
		 *
		 * История (должны быть стандартные ключи): наполняемый справочник ключ-значение [из базы данных]
		 * Возраст (от и до): число
		 * Реквизит: : наполняемый список [из базы данных]
		 * Группа персонажей (хорошие, плохие): наполняемый список [из базы данных]
		 *
		 */

		/**
		 * @brief Название героя
		 */
		QString m_name;

		/**
		 * @brief Настоящее имя героя (ФИО)
		 */
		QString m_realName;

		/**
		 * @brief Описание
		 * @note Html-форматированный текст
		 */
		QString m_description;

		/**
		 * @brief Фотографии
		 */
		CharacterPhotosTable* m_photos;
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

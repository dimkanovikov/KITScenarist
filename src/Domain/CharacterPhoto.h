#ifndef CHARACTERPHOTO_H
#define CHARACTERPHOTO_H

#include "DomainObject.h"

#include <QPixmap>


namespace Domain
{
	class Character;

	/**
	 * @brief Класс фотографии локации
	 */
	class CharacterPhoto : public DomainObject
	{
	public:
		CharacterPhoto(
				const Identifier& _id,
				Character* _character,
				const QPixmap& _photo,
				int _sortOrder
				);

		Character* character() const;
		void setCharacter(Character* _character);

		QPixmap photo() const;
		void setPhoto(const QPixmap& _photo);

		int sortOrder() const;
		void setSortOrder(int _sortOrder);

	private:
		/**
		 * @brief Локация к которой относится фотографии
		 */
		Character* m_character;

		/**
		 * @brief Собственно фотография
		 */
		QPixmap m_photo;

		/**
		 * @brief Порядок следования фотографии
		 */
		int m_sortOrder;
	};

	// ****

	class CharacterPhotosTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit CharacterPhotosTable(QObject* _parent = 0 );

	public:
		enum Column {
			Undefined,
			Id,
			Character,
			Photo,
			SortOrder
		};

	public:
		void setCharacter(Domain::Character* _character);

	public:
		int columnCount(const QModelIndex&) const;
		QVariant data(const QModelIndex& _index, int _role) const;

	private:
		Column sectionToColumn(const int _section) const;
	};
}

#endif // CHARACTERPHOTO_H

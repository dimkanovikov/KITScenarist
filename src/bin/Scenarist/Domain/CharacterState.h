#ifndef CHARACTERSTATE_H
#define CHARACTERSTATE_H

#include "DomainObject.h"

#include <QString>


namespace Domain
{
	/**
	 * @brief Класс состояния персонажа
	 */
	class CharacterState : public DomainObject
	{
	public:
		CharacterState(const Identifier& _id, const QString& _name);

		/**
		 * @brief Получить название места
		 */
		QString name() const;

	private:
		/**
		 * @brief Название места
		 */
		QString m_name;
	};

	// ****

	class CharacterStatesTable : public DomainObjectsItemModel
	{
		Q_OBJECT

	public:
		explicit CharacterStatesTable(QObject* _parent = 0);

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

#endif // CHARACTERSTATE_H

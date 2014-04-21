#ifndef CHARACTERSDATAEDITMANAGER_H
#define CHARACTERSDATAEDITMANAGER_H

#include <QObject>

namespace Domain {
	class Character;
}

namespace UserInterface {
	class CharactersDataEdit;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий данными персонажа
	 */
	class CharactersDataEditManager : public QObject
	{
		Q_OBJECT

	public:
		explicit CharactersDataEditManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Подготовить редактор к работе
		 */
		void clean();

		/**
		 * @brief Редактировать персонажа
		 */
		void editCharacter(Domain::Character* _character);

	signals:
		/**
		 * @brief Было изменено имя персонажа
		 */
		void characterNameChanged(const QString& _oldName, const QString& _newName);

	private slots:
		/**
		 * @brief Сохранить изменения
		 */
		void aboutSave();

		/**
		 * @brief Отменить изменения
		 */
		void aboutDontSave();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Редактор данных
		 */
		UserInterface::CharactersDataEdit* m_editor;

		/**
		 * @brief Персонаж для изменения
		 */
		Domain::Character* m_character;
	};
}

#endif // CHARACTERSDATAEDITMANAGER_H

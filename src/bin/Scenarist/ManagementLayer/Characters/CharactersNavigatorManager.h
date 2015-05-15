#ifndef CHARACTERSNAVIGATORMANAGER_H
#define CHARACTERSNAVIGATORMANAGER_H

#include <QObject>

namespace UserInterface {
	class CharactersNavigator;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий списком персонажей
	 */
	class CharactersNavigatorManager : public QObject
	{
		Q_OBJECT

	public:
		explicit CharactersNavigatorManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить список персонажей
		 */
		void loadCharacters();

		/**
		 * @brief Выделить персонажа
		 */
		void chooseCharacter(const QString& _name);

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Добавить персонажа
		 */
		void addCharacter(const QString& _name);

		/**
		 * @brief Изменить персонажа
		 */
		void editCharacter(const QString& _name);

		/**
		 * @brief Удалить персонажа
		 */
		void removeCharacters(const QStringList& _names);

		/**
		 * @brief Обновить список персонажей
		 */
		void refreshCharacters();

	private slots:
		/**
		 * @brief Предобработчик добавления персонажа
		 */
		void aboutAddCharacter();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения для формы
		 */
		void initConnections();

	private:
		/**
		 * @brief Представление
		 */
		UserInterface::CharactersNavigator* m_navigator;
	};
}

#endif // CHARACTERSNAVIGATORMANAGER_H

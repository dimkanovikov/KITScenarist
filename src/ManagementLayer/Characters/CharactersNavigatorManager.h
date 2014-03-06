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

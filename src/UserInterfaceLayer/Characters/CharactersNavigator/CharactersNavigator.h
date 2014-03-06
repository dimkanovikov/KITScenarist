#ifndef CHARACTERSNAVIGATOR_H
#define CHARACTERSNAVIGATOR_H

#include <QWidget>

class QToolButton;
class QListView;
class QAbstractItemModel;


namespace UserInterface
{
	class CharactersNavigator : public QWidget
	{
		Q_OBJECT

	public:
		explicit CharactersNavigator(QWidget *parent = 0);

		/**
		 * @brief Установить список персонажей
		 */
		void setModel(QAbstractItemModel* _model);

	signals:
		/**
		 * @brief Добавить персонажа
		 */
		void addCharacter();

		/**
		 * @brief Изменить персонажа
		 */
		void editCharacter(const QString& _name);

		/**
		 * @brief Удалить персонажа
		 */
		void removeCharacter(const QString& _name);

		/**
		 * @brief Обновить список персонажей
		 */
		void refreshCharacters();

	private slots:
		/**
		 * @brief Обработчик события выбора персонажа в списке
		 */
		void aboutEditCharacter();

		/**
		 * @brief Обработчик нажатия кнопки удаления персонажа
		 */
		void aboutRemoveCharacter();

	private:
		/**
		 * @brief Получить имя выделенного персонажа
		 */
		QString selectedUserName() const;

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
		 * @brief Кнопка добавления персонажа
		 */
		QToolButton* m_addCharacter;

		/**
		 * @brief Кнопка удаления персонажа
		 */
		QToolButton* m_removeCharacter;

		/**
		 * @brief Кнопка обновления списка персонажей
		 */
		QToolButton* m_refreshCharacters;

		/**
		 * @brief Список персонажей
		 */
		QListView* m_navigator;
	};
}

#endif // CHARACTERSNAVIGATOR_H

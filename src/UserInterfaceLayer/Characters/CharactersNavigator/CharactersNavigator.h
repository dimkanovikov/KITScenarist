#ifndef CHARACTERSNAVIGATOR_H
#define CHARACTERSNAVIGATOR_H

#include <QWidget>

class QLabel;
class QToolButton;
class QListView;
class QAbstractItemModel;
class QSortFilterProxyModel;


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

		/**
		 * @brief Выделить первого персонажа
		 */
		void selectFirstCharacter();

		/**
		 * @brief Выделить персонажа
		 */
		void selectCharacter(const QString& _name);

		/**
		 * @brief Выделить персонажа
		 */
		void selectCharacter(const QModelIndex& _index);

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

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Заголовок навигатора
		 */
		QLabel* m_title;

		/**
		 * @brief Кнопка добавления персонажа
		 */
		QToolButton* m_addCharacter;

		/**
		 * @brief Кнопка редактирования персонажа
		 */
		QToolButton* m_editCharacter;

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

		/**
		 * @brief Прокси модель для сортировки персонажей
		 */
		QSortFilterProxyModel* m_navigatorProxyModel;
	};
}

#endif // CHARACTERSNAVIGATOR_H

#ifndef CHARACTERSNAVIGATOR_H
#define CHARACTERSNAVIGATOR_H

#include <QWidget>

class FlatButton;
class QLabel;
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
		void removeCharacters(const QStringList& _names);

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
		void aboutRemoveCharacters();

	private:
		/**
		 * @brief Получить имена выделенных персонажей
		 */
		QStringList selectedCharactersNames() const;

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
		FlatButton* m_addCharacter;

		/**
		 * @brief Кнопка удаления персонажа
		 */
		FlatButton* m_removeCharacter;

		/**
		 * @brief Объединить выбранных персонажей в одного
		 */
		FlatButton* m_mergeCharacters;

		/**
		 * @brief Кнопка обновления списка персонажей
		 */
		FlatButton* m_refreshCharacters;

		/**
		 * @brief Окончание панели инструментов
		 */
		QLabel* m_endTitle;

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

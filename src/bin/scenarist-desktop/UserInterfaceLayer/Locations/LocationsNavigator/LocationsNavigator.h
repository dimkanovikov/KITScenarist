#ifndef LOCATIONSNAVIGATOR_H
#define LOCATIONSNAVIGATOR_H

#include <QWidget>

class FlatButton;
class QLabel;
class QListView;
class QAbstractItemModel;
class QSortFilterProxyModel;


namespace UserInterface
{
	class LocationsNavigator : public QWidget
	{
		Q_OBJECT

	public:
		explicit LocationsNavigator(QWidget *parent = 0);

		/**
		 * @brief Установить список локаций
		 */
		void setModel(QAbstractItemModel* _model);

		/**
		 * @brief Выделить первую локацию
		 */
		void selectFirstLocation();

		/**
		 * @brief Выделить локацию
		 */
		void selectLocation(const QString& _name);

		/**
		 * @brief Выделить локацию
		 */
		void selectLocation(const QModelIndex& _index);

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Добавить локацию
		 */
		void addLocation();

		/**
		 * @brief Изменить локацию
		 */
		void editLocation(const QString& _name);

		/**
		 * @brief Удалить локацию
		 */
		void removeLocations(const QStringList& _names);

		/**
		 * @brief Обновить список локаций
		 */
		void refreshLocations();

	private slots:
		/**
		 * @brief Обработчик события выбора локации в списке
		 */
		void aboutEditLocation();

		/**
		 * @brief Обработчик нажатия кнопки удаления локации
		 */
		void aboutRemoveLocation();

	private:
		/**
		 * @brief Получить названия выделенных локаций
		 */
		QStringList selectedLocationsNames() const;

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
		 * @brief Кнопка добавления локации
		 */
		FlatButton* m_addLocation;

		/**
		 * @brief Кнопка удаления локации
		 */
		FlatButton* m_removeLocation;

		/**
		 * @brief Кнопка обновления списка локаций
		 */
        FlatButton* m_refreshLocations;

		/**
		 * @brief Список локаций
		 */
		QListView* m_navigator;

		/**
		 * @brief Прокси модель для сортировки локаций
		 */
		QSortFilterProxyModel* m_navigatorProxyModel;
	};
}

#endif // LOCATIONSNAVIGATOR_H

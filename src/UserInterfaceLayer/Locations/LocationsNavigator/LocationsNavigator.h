#ifndef LOCATIONSNAVIGATOR_H
#define LOCATIONSNAVIGATOR_H

#include <QWidget>

class QToolButton;
class QListView;
class QAbstractItemModel;


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
		void removeLocation(const QString& _name);

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
		 * @brief Получить имя выделенной локации
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
		 * @brief Кнопка добавления локации
		 */
		QToolButton* m_addLocation;

		/**
		 * @brief Кнопка редактирования локации
		 */
		QToolButton* m_editLocation;

		/**
		 * @brief Кнопка удаления локации
		 */
		QToolButton* m_removeLocation;

		/**
		 * @brief Кнопка обновления списка локаций
		 */
		QToolButton* m_refreshLocations;

		/**
		 * @brief Список локаций
		 */
		QListView* m_navigator;
	};
}

#endif // LOCATIONSNAVIGATOR_H

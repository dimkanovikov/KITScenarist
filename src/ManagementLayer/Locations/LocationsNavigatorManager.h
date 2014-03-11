#ifndef LOCATIONSNAVIGATORMANAGER_H
#define LOCATIONSNAVIGATORMANAGER_H

#include <QObject>

namespace UserInterface {
	class LocationsNavigator;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий списком локаций
	 */
	class LocationsNavigatorManager : public QObject
	{
		Q_OBJECT

	public:
		explicit LocationsNavigatorManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить список локаций
		 */
		void loadLocations();

		/**
		 * @brief Выделить локацию
		 */
		void chooseLocation(const QString& _name);

	signals:
		/**
		 * @brief Добавить локацию
		 */
		void addLocation(const QString& _name);

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
		 * @brief Предобработчик добавления локации
		 */
		void aboutAddLocation();

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
		UserInterface::LocationsNavigator* m_navigator;
	};
}

#endif // LOCATIONSNAVIGATORMANAGER_H

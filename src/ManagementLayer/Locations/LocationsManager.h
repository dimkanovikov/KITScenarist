#ifndef LOCATIONSMANAGER_H
#define LOCATIONSMANAGER_H

#include <QObject>

namespace ManagementLayer
{
	class LocationsNavigatorManager;
	class LocationsDataEditManager;


	/**
	 * @brief Управляющий локациями
	 */
	class LocationsManager : public QObject
	{
		Q_OBJECT

	public:
		explicit LocationsManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить данные текущего проекта
		 */
		void loadCurrentProject();

	signals:
		/**
		 * @brief Было изменено название локации
		 */
		void locationNameChanged(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Обновить список локаций
		 */
		void refreshLocations();

	private slots:
		/**
		 * @brief Добавить локацию
		 */
		void aboutAddLocation(const QString& _name);

		/**
		 * @brief Изменить локацию
		 */
		void aboutEditLocation(const QString& _name);

		/**
		 * @brief Удалить локацию
		 */
		void aboutRemoveLocation(const QString& _name);

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
		 * @brief Представление
		 */
		QWidget* m_view;

		/**
		 * @brief Управляющий списком локаций
		 */
		LocationsNavigatorManager* m_navigatorManager;

		/**
		 * @brief Управляющий параметрами локации
		 */
		LocationsDataEditManager* m_dataEditManager;
	};
}

#endif // LOCATIONSMANAGER_H

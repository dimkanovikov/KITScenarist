#ifndef LOCATIONSDATAEDITMANAGER_H
#define LOCATIONSDATAEDITMANAGER_H

#include <QObject>

namespace Domain {
	class Location;
}

namespace UserInterface {
	class LocationsDataEdit;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий данными локации
	 */
	class LocationsDataEditManager : public QObject
	{
		Q_OBJECT

	public:
		explicit LocationsDataEditManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Подготовить редактор к работе
		 */
		void clean();

		/**
		 * @brief Редактировать локацию
		 */
		void editLocation(Domain::Location* _location);

	signals:
		/**
		 * @brief Была изменена локация
		 */
		void locationChanged();

		/**
		 * @brief Было изменено название локации
		 */
		void locationNameChanged(const QString& _oldName, const QString& _newName);

	private slots:
		/**
		 * @brief Сохранить изменения
		 */
		void aboutSave();

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
		UserInterface::LocationsDataEdit* m_editor;

		/**
		 * @brief Локация для изменения
		 */
		Domain::Location* m_location;
	};
}

#endif // LOCATIONSDATAEDITMANAGER_H

#ifndef SETTINGSSTORAGE_H
#define SETTINGSSTORAGE_H

#include <QString>


namespace StorageLayer
{
	/**
	 * @brief Класс для доступа к настройкам сценария
	 */
	class SettingsStorage
	{
	public:
		enum Settingsplace {
			ApplicationSettings,
			ScenarioSettings
		};

	public:
		/**
		 * @brief Сохранить значение с заданным ключём
		 */
		void setValue(const QString& _key, const QString& _value, Settingsplace _settingsPlace);

		/**
		 * @brief Получить значение по ключу
		 */
		QString value(const QString& _key, Settingsplace _settingsPlace);

	private:
		SettingsStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SETTINGSSTORAGE_H

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
		/**
		 * @brief Сохранить значение с заданным ключём
		 */
		void setValue(const QString& _key, const QString& _value);

		/**
		 * @brief Получить значение по ключу
		 */
		QString value(const QString& _key);

	private:
		SettingsStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SETTINGSSTORAGE_H

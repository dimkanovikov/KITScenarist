#ifndef SETTINGSSTORAGE_H
#define SETTINGSSTORAGE_H

#include <QSettings>
#include <QString>
#include <QMap>
#include <QVariant>


namespace DataStorageLayer
{
	/**
	 * @brief Класс для доступа к настройкам сценария
	 */
	class SettingsStorage
	{
	public:
		enum SettingsPlace {
			ApplicationSettings,
			ScenarioSettings
		};

	public:
		/**
		 * @brief Сохранить значение с заданным ключём
		 */
		/** @{ */
		void setVariantValue(const QString& _key, const QVariant& _value, SettingsPlace _settingsPlace);
		void setValue(const QString& _key, const QString& _value, SettingsPlace _settingsPlace);
		void setValue(const QString& _key, const QStringList& _value, SettingsPlace _settingsPlace);
		/** @} */

		/**
		 * @brief Сохранить карту параметров
		 */
		void setValues(const QMap<QString, QString>& _values, const QString& _valuesGroup, SettingsPlace _settingsPlace);

		/**
		 * @brief Получить значение по ключу
		 */
		QVariant variantValue(const QString& _key, SettingsPlace _settingsPlace);
		QString value(const QString& _key, SettingsPlace _settingsPlace, const QString& _defaultValue = QString::null);

		/**
		 * @brief Получить группу значений
		 */
		QMap<QString, QString> values(const QString& _valuesGroup, SettingsPlace _settingsPlace);

		/**
		 * @brief Сбросить настройки
		 */
		void resetValues(SettingsPlace _settingsPlace);

		/**
		 * @brief Сохранить и загрузить положения окон, слайдеров, заголовков таблиц и т.п.
		 */
		/** @{ */
		void saveApplicationStateAndGeometry(QWidget* _widget);
		void loadApplicationStateAndGeometry(QWidget* _widget);
		/** @} */

	private:
		SettingsStorage();

		/**
		 * @brief Настройки приложения
		 */
		QSettings m_appSettings;

		/**
		 * @brief Значения параметров по умолчанию
		 */
		QMap<QString, QString> m_defaultValues;

		/**
		 * @brief Кэшированные значения параметров
		 */
		/** @{ */
		QMap<QString, QVariant> m_cachedValuesApp;
		QMap<QString, QVariant> m_cachedValuesDb;
		/** @} */

		/**
		 * @brief Загрузить параметр из кэша
		 */
		QVariant getCachedValue(const QString& _key, SettingsPlace _settingsPlace, bool& _ok);

		/**
		 * @brief Сохранить параметр в кэше
		 */
		void cacheValue(const QString& _key, const QVariant& _value, SettingsPlace _settingsPlace);

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SETTINGSSTORAGE_H

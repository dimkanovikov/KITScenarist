#ifndef SETTINGSMAPPER_H
#define SETTINGSMAPPER_H

#include <QString>


namespace DataMappingLayer
{
	class SettingsMapper
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
		SettingsMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // SETTINGSMAPPER_H

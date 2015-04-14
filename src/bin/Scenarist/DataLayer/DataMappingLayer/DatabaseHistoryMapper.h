#ifndef DATABASEHISTORYMAPPER_H
#define DATABASEHISTORYMAPPER_H

#include <QMap>


namespace DataMappingLayer
{
	class DatabaseHistoryMapper
	{
	public:
		/**
		 * @brief Получить таблицу изменений с указанной даты и времени
		 */
		QList<QMap<QString, QString> > history(const QString& _fromDatetime);

	private:
		DatabaseHistoryMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // DATABASEHISTORYMAPPER_H

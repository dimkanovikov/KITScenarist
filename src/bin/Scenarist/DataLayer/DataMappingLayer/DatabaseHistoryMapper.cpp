#include "DatabaseHistoryMapper.h"

#include <DataLayer/Database/Database.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

using DataMappingLayer::DatabaseHistoryMapper;

namespace {
	const QString ID_KEY = "id";
	const QString QUERY_KEY = "query";
	const QString QUERY_VALUES_KEY = "query_values";
	const QString DATETIME_KEY = "datetime";
}


QList<QMap<QString, QString> > DatabaseHistoryMapper::history(const QString& _fromDatetime)
{
	QSqlQuery q_loader = DatabaseLayer::Database::query();
	q_loader.exec(
		QString("SELECT %1, %2, %3, %4 FROM _database_history WHERE datetime > '%5'")
		.arg(ID_KEY, QUERY_KEY, QUERY_VALUES_KEY, DATETIME_KEY, _fromDatetime)
		);

	QList<QMap<QString, QString> > databaseHistory;
	while (q_loader.next()) {
		QMap<QString, QString> historyRecord;
		historyRecord.insert(ID_KEY, q_loader.value(ID_KEY).toString());
		historyRecord.insert(QUERY_KEY, q_loader.value(QUERY_KEY).toString());
		historyRecord.insert(QUERY_VALUES_KEY, q_loader.value(QUERY_VALUES_KEY).toString());
		historyRecord.insert(DATETIME_KEY, q_loader.value(DATETIME_KEY).toString());

		databaseHistory.append(historyRecord);
	}

	return databaseHistory;
}

DatabaseHistoryMapper::DatabaseHistoryMapper()
{
}



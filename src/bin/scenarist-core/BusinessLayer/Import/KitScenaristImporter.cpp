#include "KitScenaristImporter.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

using namespace BusinessLogic;

namespace {
	const QString SQL_DRIVER = "QSQLITE";
	const QString CONNECTION_NAME = "import_database";
}


KitScenaristImporter::KitScenaristImporter() :
	AbstractImporter()
{

}

QString KitScenaristImporter::importScenario(const ImportParameters& _importParameters) const
{
	QString result;

	{
		QSqlDatabase database = QSqlDatabase::addDatabase(SQL_DRIVER, CONNECTION_NAME);
		database.setDatabaseName(_importParameters.filePath);
		if (database.open()) {
			QSqlQuery query(database);
			query.exec("SELECT text FROM scenario WHERE is_draft = 0");
			query.next();
			result = query.record().value("text").toString();
		}
	}

	QSqlDatabase::removeDatabase(CONNECTION_NAME);

	return result;
}

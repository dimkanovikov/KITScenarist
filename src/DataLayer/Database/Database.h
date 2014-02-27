#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>


namespace DatabaseLayer
{
	class Database
	{
	public:
		static QSqlDatabase instanse(const QString& _databaseFileName = QString());

		enum State {
			Null,
			Scheme,
			Indexes,
			Enums,
			Data
		};
		Q_DECLARE_FLAGS(States, State)


	private:
		static QString CONNECTION_NAME,
					   SQL_DRIVER,
					   DATABASE_NAME;

		static void open(QSqlDatabase& _database,
				const QString& _connectionName,
				const QString& _databaseName
				);
		static Database::States checkState(QSqlDatabase& _database);
		static void createTables(QSqlDatabase& _database);
		static void createIndexes(QSqlDatabase& _database);
		static void createEnums(QSqlDatabase& _database);
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS( Database::States )
}

#endif // DATABASE_H

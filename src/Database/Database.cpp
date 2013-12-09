#include "Database.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

using namespace DatabaseLayer;


QSqlDatabase Database::instanse()
{
	QSqlDatabase database;

	if ( !QSqlDatabase::contains( CONNECTION_NAME ) ) {
		open( database );
	} else {
		database = QSqlDatabase::database( CONNECTION_NAME );
	}

	return database;
}

QString Database::CONNECTION_NAME = "local_database";
QString Database::SQL_DRIVER      = "QSQLITE";
QString Database::DATABASE_NAME   = "database.db";

void Database::open(QSqlDatabase& _database, const QString& _connectionName, const QString& _databaseName)
{
	_database = QSqlDatabase::addDatabase(SQL_DRIVER, _connectionName);
	_database.setDatabaseName(_databaseName);
	_database.open();

	Database::States states = checkState(_database);

	if (!states.testFlag(Scheme))
		createTables(_database);
	if (!states.testFlag(Indexes))
		createIndexes(_database);
	if (!states.testFlag(Enums))
		createEnums(_database);
}

// Проверка состояния базы данных
// например:
// - БД отсутствует
// - БД пуста
// - БД имеет старую версию
// - БД имеет последнюю версию
// - и т.д.
Database::States Database::checkState(QSqlDatabase& _database)
{
	QSqlQuery q_checker(_database);
	Database::States states;

	// Схема данных
	if (q_checker.exec("SELECT COUNT(*) as size FROM sqlite_master WHERE type = 'table' ") &&
		q_checker.next() &&
		q_checker.record().value("size").toInt()
		)
		states = states | Database::Scheme;
	// Индексы
	if (q_checker.exec("SELECT COUNT(*) as size FROM sqlite_master WHERE type = 'index' ") &&
		q_checker.next() &&
		q_checker.record().value("size").toInt()
		)
		states = states | Database::Indexes;

	return states;
}

void Database::createTables(QSqlDatabase& _database)
{
	QSqlQuery q_creator(_database);
	_database.transaction();

	// Таблица "Место"
	q_creator.exec("CREATE TABLE places "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
				   "name TEXT UNIQUE NOT NULL "
				   "); "
				   );

	// Таблица "Локация"
	q_creator.exec("CREATE TABLE locations "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
				   "parent_id INTEGER DEFAULT(NULL), "
				   "name TEXT UNIQUE NOT NULL "
				   "); "
				   );

	// Таблица "Сценарний день"
	q_creator.exec("CREATE TABLE scenary_days "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
				   "name TEXT UNIQUE NOT NULL "
				   "); "
				   );

	// Таблица "Время"
	q_creator.exec("CREATE TABLE times "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
				   "name TEXT UNIQUE NOT NULL "
				   "); "
				   );

	// Таблица "Персонажи"
	q_creator.exec("CREATE TABLE characters "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
				   "name TEXT UNIQUE NOT NULL "
				   "); "
				   );

	_database.commit();
}

void Database::createIndexes(QSqlDatabase& _database)
{
	Q_UNUSED(_database);
}

void Database::createEnums(QSqlDatabase& _database)
{
	QSqlQuery q_creator(_database);
	_database.transaction();

	// Справочник мест
	{
		q_creator.exec("INSERT INTO places (id, name) VALUES (null, 'INT');");
		q_creator.exec("INSERT INTO places (id, name) VALUES (null, 'EXT');");
		q_creator.exec("INSERT INTO places (id, name) VALUES (null, 'ИНТ');");
		q_creator.exec("INSERT INTO places (id, name) VALUES (null, 'НАТ');");
		q_creator.exec("INSERT INTO places (id, name) VALUES (null, 'ПАВ');");
	}

	// Справочник времени
	{
		q_creator.exec("INSERT INTO times (id, name) VALUES (null, 'DAY');");
		q_creator.exec("INSERT INTO times (id, name) VALUES (null, 'NIGHT');");
		q_creator.exec("INSERT INTO times (id, name) VALUES (null, 'ДЕНЬ');");
		q_creator.exec("INSERT INTO times (id, name) VALUES (null, 'НОЧЬ');");
	}

	_database.commit();
}

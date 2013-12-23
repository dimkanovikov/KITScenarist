#include "DatabaseHelper.h"

#include "Database.h"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include <QDebug>
#include <QSqlError>

using namespace DatabaseLayer;


namespace {
	const QString CONNECTION_NAME = "file_database";
	const QString SQL_DRIVER      = "QSQLITE";
	const QString ATTACH_NAME	  = "fileDB";
}

void DatabaseHelper::saveDatabaseToFile(const QString& _databaseFileName)
{
	//
	// Удалить файл, если существует
	//
	if (QFile::exists(_databaseFileName)) {
		QFile::remove(_databaseFileName);
	}

	//
	// Открыть БД в файле
	//
	{
		QSqlDatabase fileDatabase = QSqlDatabase::addDatabase(SQL_DRIVER, CONNECTION_NAME);
		fileDatabase.setDatabaseName(_databaseFileName);
		fileDatabase.open();
	}

	//
	// Скопировать схему
	//
	{
		QSqlDatabase fileDatabase = QSqlDatabase::database(CONNECTION_NAME);

		QSqlQuery q_schemeFromMemory(Database::instanse());
		QSqlQuery q_schemeToFile(fileDatabase);

		fileDatabase.transaction();
		q_schemeFromMemory.exec("SELECT sql FROM sqlite_master WHERE sql IS NOT NULL AND name != 'sqlite_sequence'");
		while (q_schemeFromMemory.next()) {
			q_schemeToFile.exec(q_schemeFromMemory.value("sql").toString());
		}
		fileDatabase.commit();
	}

	//
	// Скопировать данные
	//
	{
		QSqlDatabase memoryDatabase = Database::instanse();

		QSqlQuery q_dataFromMemory(memoryDatabase);
		QSqlQuery q_dataToFile(memoryDatabase);

		//
		// ... присоединить базу данных из файла
		//
		q_dataToFile.exec(
					QString("ATTACH DATABASE '%1' AS %2")
					.arg(_databaseFileName)
					.arg(ATTACH_NAME)
					);

		//
		// ... скопировать данные
		//
		memoryDatabase.transaction();
		q_dataFromMemory.exec("SELECT name FROM sqlite_master WHERE type = 'table'");
		while (q_dataFromMemory.next()) {
			q_dataToFile.exec(
						QString("INSERT INTO %1.%2 SELECT * FROM main.%2")
						.arg(ATTACH_NAME)
						.arg(q_dataFromMemory.value("name").toString())
						);
		}
		memoryDatabase.commit();

		//
		// ... отсоединить бд из файла
		//
		q_dataToFile.exec(
					QString("DETACH DATABASE '%1'")
					.arg(ATTACH_NAME)
					);
	}


	//
	// Закрыть базу данных из файла
	//
	QSqlDatabase::removeDatabase(CONNECTION_NAME);
}

void DatabaseHelper::loadDatabaseFromFile(const QString* _databaseFileName)
{
	//
	// Очистить данные в памяти
	// Открыть базу данных из файла
	// В зависимости от версии загрузить данные из файла в память
	// Закрыть базу данных из файла
	//
}

#include "DatabaseHelper.h"

#include "Database.h"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QStringList>

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
		q_schemeFromMemory.exec("SELECT sql FROM sqlite_master "
								"WHERE sql IS NOT NULL AND name != 'sqlite_sequence'");
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

void DatabaseHelper::loadDatabaseFromFile(const QString& _databaseFileName)
{
	//
	// Очистить данные в памяти
	//
	clearDatabaseInMemory();

	//
	// Открыть базу данных из файла
	//
	{
		QSqlDatabase fileDatabase = QSqlDatabase::addDatabase(SQL_DRIVER, CONNECTION_NAME);
		fileDatabase.setDatabaseName(_databaseFileName);
		fileDatabase.open();
	}

	//
	// В зависимости от версии загрузить данные из файла в память
	//
	{
		QSqlDatabase fileDatabase = QSqlDatabase::database(CONNECTION_NAME);

		//
		// Получить версию БД в файле
		//
		QSqlQuery q_versionFinder(fileDatabase);
		q_versionFinder.exec("SELECT value FROM system_variables WHERE variable = 'application-version'");
		q_versionFinder.next();

		// ... major.minor.build
		QString version = q_versionFinder.value("value").toString();
		int versionMajor = version.split(".").first().toInt();
		//int versionMinor = version.split(".").value(1).toInt();
		//int versionBuild = version.split(".").last().toInt();

		switch (versionMajor) {
			case 0: {
				loadDatabaseFromFileVersion0(fileDatabase);
				break;
			}

			default: {
				break;
			}
		}
	}

	//
	// Закрыть базу данных из файла
	//
	QSqlDatabase::removeDatabase(CONNECTION_NAME);
}

void DatabaseHelper::clearDatabaseInMemory()
{
	QSqlDatabase memoryDatabase = Database::instanse();

	QSqlQuery q_cleaner(memoryDatabase);

	memoryDatabase.transaction();
//	q_cleaner.exec("DELETE FROM places");
	q_cleaner.exec("DELETE FROM locations");
	q_cleaner.exec("DELETE FROM scenary_days");
//	q_cleaner.exec("DELETE FROM times");
	q_cleaner.exec("DELETE FROM characters");
	q_cleaner.exec("DELETE FROM scenario");
	memoryDatabase.commit();
}

void DatabaseHelper::loadDatabaseFromFileVersion0(const QSqlDatabase& _fileDatabase)
{
	QSqlDatabase memoryDatabase = Database::instanse();

	QSqlQuery q_dataFromFile(_fileDatabase);
	QSqlQuery q_dataToMemory(memoryDatabase);

	// Настройки
	q_dataFromFile.exec("SELECT variable, value FROM system_variables WHERE variable != 'application-version'");
	while (q_dataFromFile.next()) {
		q_dataToMemory.exec(
					QString("INSERT INTO system_variables (variable, value) VALUES('%1', '%2')")
					.arg(q_dataFromFile.value("variable").toString())
					.arg(q_dataFromFile.value("value").toString())
					);
	}

	// Место
	q_dataFromFile.exec("SELECT id, name FROM places");
	while (q_dataFromFile.next()) {
		q_dataToMemory.exec(
					QString("INSERT INTO places (id, name) VALUES(%1, '%2')")
					.arg(q_dataFromFile.value("id").toInt())
					.arg(q_dataFromFile.value("name").toString())
					);
	}

	// Локации
	q_dataFromFile.exec("SELECT id, parent_id, name FROM locations");
	while (q_dataFromFile.next()) {
		q_dataToMemory.exec(
					QString("INSERT INTO locations (id, parent_id, name) VALUES(%1, %2, '%3')")
					.arg(q_dataFromFile.value("id").toInt())
					.arg(q_dataFromFile.value("parent_id").toInt())
					.arg(q_dataFromFile.value("name").toString())
					);
	}

	// Дни сценария
	q_dataFromFile.exec("SELECT id, name FROM scenary_days");
	while (q_dataFromFile.next()) {
		q_dataToMemory.exec(
					QString("INSERT INTO scenary_days (id, name) VALUES(%1, '%2')")
					.arg(q_dataFromFile.value("id").toInt())
					.arg(q_dataFromFile.value("name").toString())
					);
	}

	// Время
	q_dataFromFile.exec("SELECT id, name FROM times");
	while (q_dataFromFile.next()) {
		q_dataToMemory.exec(
					QString("INSERT INTO times (id, name) VALUES(%1, '%2')")
					.arg(q_dataFromFile.value("id").toInt())
					.arg(q_dataFromFile.value("name").toString())
					);
	}

	// Персонажи
	q_dataFromFile.exec("SELECT id, name FROM characters");
	while (q_dataFromFile.next()) {
		q_dataToMemory.exec(
					QString("INSERT INTO characters (id, name) VALUES(%1, '%2')")
					.arg(q_dataFromFile.value("id").toInt())
					.arg(q_dataFromFile.value("name").toString())
					);
	}

	// Текст сценария
	q_dataFromFile.exec("SELECT id, text, is_fixed, fix_date, fix_comment FROM scenario");
	while (q_dataFromFile.next()) {
		q_dataToMemory.prepare("INSERT INTO scenario (id, text, is_fixed, fix_date, fix_comment) "
							   "VALUES(?, ?, ?, ?, ?)");
		q_dataToMemory.addBindValue(q_dataFromFile.value("id"));
		q_dataToMemory.addBindValue(q_dataFromFile.value("text"));
		q_dataToMemory.addBindValue(q_dataFromFile.value("is_fixed"));
		q_dataToMemory.addBindValue(q_dataFromFile.value("fix_date"));
		q_dataToMemory.addBindValue(q_dataFromFile.value("fix_comment"));
		q_dataToMemory.exec();
	}
}

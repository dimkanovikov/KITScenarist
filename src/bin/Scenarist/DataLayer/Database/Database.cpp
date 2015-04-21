#include "Database.h"

#include <QApplication>
#include <QDateTime>
#include <QRegularExpression>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextCodec>
#include <QVariant>
#include <QUuid>

using namespace DatabaseLayer;


void Database::setCurrentFile(const QString& _databaseFileName)
{
	//
	// Если использовалась база данных, то удалим старое соединение
	//
	closeCurrentFile();

	//
	// Установим текущее имя базы данных
	//
	if (DATABASE_NAME != _databaseFileName) {
		DATABASE_NAME = _databaseFileName;
		CONNECTION_NAME = "local_database [" + DATABASE_NAME + "]";
	}

	//
	// Откроем базу данных, или создадим новую
	//
	instanse();
}

void Database::closeCurrentFile()
{
	if (QSqlDatabase::contains(CONNECTION_NAME)) {
		QSqlDatabase::removeDatabase(CONNECTION_NAME);
	}
}

QString Database::currentFile()
{
	return instanse().databaseName();
}

QSqlQuery Database::query()
{
	return QSqlQuery(instanse());
}

void Database::transaction()
{
	//
	// Для первого запроса открываем транзакцию
	//
	if (openedTransactions == 0) {
		instanse().transaction();
	}

	//
	// Увеличиваем счётчик открытых транзакций
	//
	++openedTransactions;
}

void Database::commit()
{
	//
	// Уменьшаем счётчик транзакций
	//
	--openedTransactions;

	//
	// При закрытии корневой транзакции фиксируем изменения в базе данных
	//
	if (openedTransactions == 0) {
		instanse().commit();
	}
}


//********
// Скрытая часть


QString Database::CONNECTION_NAME = "local_database";
QString Database::SQL_DRIVER      = "QSQLITE";
QString Database::DATABASE_NAME   = ":memory:";

int Database::openedTransactions = 0;

QSqlDatabase Database::instanse()
{
	QSqlDatabase database;

	if (!QSqlDatabase::contains(CONNECTION_NAME)) {
		open(database, CONNECTION_NAME, DATABASE_NAME);
	} else {
		database = QSqlDatabase::database(CONNECTION_NAME);
	}

	return database;
}

void Database::open(QSqlDatabase& _database, const QString& _connectionName, const QString& _databaseName)
{
	_database = QSqlDatabase::addDatabase(SQL_DRIVER, _connectionName);
	_database.setDatabaseName(_databaseName);
	_database.open();

	Database::States states = checkState(_database);

	if (!states.testFlag(SchemeFlag))
		createTables(_database);
	if (!states.testFlag(IndexesFlag))
		createIndexes(_database);
	if (!states.testFlag(EnumsFlag))
		createEnums(_database);
	if (states.testFlag(OldVersionFlag))
		updateDatabase(_database);
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
	Database::States states = Database::EmptyFlag;

	//
	// Созданы ли таблицы
	//
	if (q_checker.exec("SELECT COUNT(*) as size FROM sqlite_master WHERE type = 'table' ") &&
		q_checker.next() &&
		q_checker.record().value("size").toInt()) {
		//
		// Все остальные проверки имеют смысл, только если проходит данная проверка
		//
		states = states | Database::SchemeFlag;

		//
		// Созданы ли индексы
		//
		if (q_checker.exec("SELECT COUNT(*) as size FROM sqlite_master WHERE type = 'index' ") &&
			q_checker.next() &&
			q_checker.record().value("size").toInt()) {
			states = states | Database::IndexesFlag;
		}

		//
		// Проверка версии
		//
		if (q_checker.exec("SELECT value as version FROM system_variables WHERE variable = 'application-version' ") &&
			q_checker.next() &&
			q_checker.record().value("version").toString() != qApp->applicationVersion()) {
			states = states | Database::OldVersionFlag;
		}
	}

	return states;
}

void Database::createTables(QSqlDatabase& _database)
{
	QSqlQuery q_creator(_database);
	_database.transaction();

	// Таблица с историей запросов
	q_creator.exec("CREATE TABLE _database_history "
				   "( "
				   "id TEXT PRIMARY KEY, " // uuid
				   "query TEXT NOT NULL, "
				   "query_values TEXT NOT NULL, "
				   "datetime TEXT NOT NULL "
				   "); "
				   );

	// Таблица системных переменных
	q_creator.exec("CREATE TABLE system_variables "
				   "( "
				   "variable TEXT PRIMARY KEY ON CONFLICT REPLACE, "
				   "value TEXT NOT NULL "
				   "); "
				   );

	// Таблица "Место"
	q_creator.exec("CREATE TABLE places "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "name TEXT UNIQUE NOT NULL "
				   "); "
				   );

	// Таблица "Локация"
	q_creator.exec("CREATE TABLE locations "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "name TEXT UNIQUE NOT NULL, "
				   "description TEXT DEFAULT(NULL) "
				   "); "
				   );

	// Таблица "Фотографии локаций"
	q_creator.exec("CREATE TABLE locations_photo "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "fk_location_id INTEGER NOT NULL, "
				   "photo BLOB NOT NULL, "
				   "sort_order INTEGER NOT NULL DEFAULT(0) "
				   ")"
				   );

	// Таблица "Сценарний день"
	q_creator.exec("CREATE TABLE scenary_days "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "name TEXT UNIQUE NOT NULL "
				   "); "
				   );

	// Таблица "Время"
	q_creator.exec("CREATE TABLE times "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "name TEXT UNIQUE NOT NULL "
				   "); "
				   );

	// Таблица "Персонажи"
	q_creator.exec("CREATE TABLE characters "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "name TEXT UNIQUE NOT NULL, "
				   "real_name TEXT DEFAULT(NULL), "
				   "description TEXT DEFAULT(NULL) "
				   "); "
				   );

	// Таблица "Состояния персонажей"
	q_creator.exec("CREATE TABLE character_states "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "name TEXT UNIQUE NOT NULL "
				   "); "
				   );

	// Таблица "Фотографии персонажей"
	q_creator.exec("CREATE TABLE characters_photo "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "fk_character_id INTEGER NOT NULL, "
				   "photo BLOB NOT NULL, "
				   "sort_order INTEGER NOT NULL DEFAULT(0) "
				   ")"
				   );

	// Таблица "Текст сценария"
	q_creator.exec("CREATE TABLE scenario "
				   "( "
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "name TEXT DEFAULT(NULL), "
				   "additional_info TEXT DEFAULT(NULL), "
				   "genre TEXT DEFAULT(NULL), "
				   "author TEXT DEFAULT(NULL), "
				   "contacts TEXT DEFAULT(NULL), "
				   "year TEXT DEFAULT(NULL), "
				   "synopsis TEXT DEFAULT(NULL), "
				   "text TEXT NOT NULL, "
				   "is_draft INTEGER NOT NULL DEFAULT(0), "
				   "version_start_datetime TEXT NOT NULL, "
				   "version_end_datetime TEXT NOT NULL, "
				   "version_comment TEXT DEFAULT(NULL), "
				   "uuid TEXT NOT NULL, "
				   "is_synced INTEGER DEFAULT(0) "
				   ")"
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

	// Пустой сценарий и черновик
	{
		const QString dateTime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");
		q_creator.exec(
			QString("INSERT INTO scenario (id, text, version_start_datetime, version_end_datetime) "
					"VALUES(null, '', '%1', '%1')")
					.arg(dateTime)
					);
		q_creator.exec(
			QString("INSERT INTO scenario (id, text, is_draft, version_start_datetime, version_end_datetime) "
					"VALUES(null, '', 1, '%1', '%1')")
					.arg(dateTime)
					);
	}

	// Версия программы
	{
		q_creator.exec(
					QString("INSERT INTO system_variables VALUES ('application-version', '%1')")
					.arg(qApp->applicationVersion())
					);
	}

	// Справочник мест
	{
#ifdef USE_RUSSIAN_DATABASE_ENUMS
		q_creator.exec(
					QString("INSERT INTO places (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("ИНТ"))
					);
		q_creator.exec(
					QString("INSERT INTO places (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("НАТ"))
					);
#else
		q_creator.exec("INSERT INTO places (id, name) VALUES (null, 'INT');");
		q_creator.exec("INSERT INTO places (id, name) VALUES (null, 'EXT');");
#endif
	}

	// Справочник времени
	{
#ifdef USE_RUSSIAN_DATABASE_ENUMS
		q_creator.exec(
					QString("INSERT INTO times (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("ДЕНЬ"))
					);
		q_creator.exec(
					QString("INSERT INTO times (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("НОЧЬ"))
					);
		q_creator.exec(
					QString("INSERT INTO times (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("УТРО"))
					);
		q_creator.exec(
					QString("INSERT INTO times (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("ВЕЧЕР"))
					);
#else
		q_creator.exec("INSERT INTO times (id, name) VALUES (null, 'DAY');");
		q_creator.exec("INSERT INTO times (id, name) VALUES (null, 'NIGHT');");
		q_creator.exec("INSERT INTO times (id, name) VALUES (null, 'MORNING');");
		q_creator.exec("INSERT INTO times (id, name) VALUES (null, 'EVENING');");
#endif
	}

	// Справочник состояний персонажей
	{
#ifdef USE_RUSSIAN_DATABASE_ENUMS
		q_creator.exec(
					QString("INSERT INTO character_states (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("ВПЗ"))
					);
		q_creator.exec(
					QString("INSERT INTO character_states (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("ЗК"))
					);
		q_creator.exec(
					QString("INSERT INTO character_states (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("ПРОД"))
					);
#else
		q_creator.exec("INSERT INTO character_states (id, name) VALUES (null, 'V.O.');");
		q_creator.exec("INSERT INTO character_states (id, name) VALUES (null, 'O.S.');");
		q_creator.exec("INSERT INTO character_states (id, name) VALUES (null, 'O.C.');");
		q_creator.exec("INSERT INTO character_states (id, name) VALUES (null, 'SUBTITLE');");
#endif
	}

	_database.commit();
}

void Database::updateDatabase(QSqlDatabase& _database)
{
	QSqlQuery q_checker(_database);

	//
	// Определим версию базы данных
	//
	q_checker.exec("SELECT value as version FROM system_variables WHERE variable = 'application-version' ");
	q_checker.next();
	QString databaseVersion = q_checker.record().value("version").toString();
	int versionMajor = databaseVersion.split(".").value(0, "0").toInt();
	int versionMinor = databaseVersion.split(".").value(1, "0").toInt();
	int versionBuild = databaseVersion.split(".").value(2, "1").toInt();

	//
	// Вызываются необходимые процедуры обновления БД в зависимости от её версии
	//
	// 0.X.X
	//
	if (versionMajor <= 0) {
		//
		// 0.0.X
		//
		if (versionMinor <= 0) {
			if (versionBuild <= 1) {
				updateDatabaseTo_0_0_2(_database);
			}
			if (versionBuild <= 3) {
				updateDatabaseTo_0_0_4(_database);
			}
			if (versionBuild <= 4) {
				updateDatabaseTo_0_0_5(_database);
			}
		}
		//
		// 0.1.X
		//
		if (versionMinor <= 1) {
			if (versionBuild <= 0) {
				updateDatabaseTo_0_1_0(_database);
			}
		}
		//
		// 0.2.X
		//
		if (versionMinor <= 2) {
			if (versionBuild <= 7) {
				updateDatabaseTo_0_2_8(_database);
			}
		}
		//
		// 0.3.X
		//
		if (versionMinor <= 3) {
			if (versionBuild <= 2) {
				updateDatabaseTo_0_3_3(_database);
			}
		}
		//
		// 0.4.X
		//
		if (versionMinor <= 4) {
			if (versionBuild <= 4) {
				updateDatabaseTo_0_4_5(_database);
			}
			if (versionBuild <= 9) {
				updateDatabaseTo_0_5_0(_database);
			}
		}
	}

	//
	// Обновляется версия программы
	//
	q_checker.exec(
				QString("INSERT INTO system_variables VALUES ('application-version', '%1')")
				.arg(qApp->applicationVersion())
				);
}

void Database::updateDatabaseTo_0_0_2(QSqlDatabase& _database)
{
	QSqlQuery q_updater(_database);

	_database.transaction();

	{
#ifdef USE_RUSSIAN_DATABASE_ENUMS
		q_updater.exec(
					QString("INSERT INTO times (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("УТРО"))
					);
		q_updater.exec(
					QString("INSERT INTO times (id, name) VALUES (null, '%1');")
					.arg(QString::fromUtf8("ВЕЧЕР"))
					);
#else
		q_updater.exec("INSERT INTO times (id, name) VALUES (null, 'MORNING');");
		q_updater.exec("INSERT INTO times (id, name) VALUES (null, 'EVENING');");
#endif
	}

	_database.commit();
}

void Database::updateDatabaseTo_0_0_4(QSqlDatabase& _database)
{
	QSqlQuery q_updater(_database);

	_database.transaction();

	{
#ifdef USE_RUSSIAN_DATABASE_ENUMS
		q_updater.exec(
					QString("UPDATE times SET id = 1 WHERE name = '%1';")
					.arg(QString::fromUtf8("УТРО"))
					);
		q_updater.exec(
					QString("UPDATE times SET id = 2 WHERE name = '%1';")
					.arg(QString::fromUtf8("ДЕНЬ"))
					);
		q_updater.exec(
					QString("UPDATE times SET id = 3 WHERE name = '%1';")
					.arg(QString::fromUtf8("ВЕЧЕР"))
					);
		q_updater.exec(
					QString("UPDATE times SET id = 4 WHERE name = '%1';")
					.arg(QString::fromUtf8("НОЧЬ"))
					);
#else
		q_updater.exec("UPDATE times SET id = 1 WHERE name = 'MORNING';");
		q_updater.exec("UPDATE times SET id = 2 WHERE name = 'DAY';");
		q_updater.exec("UPDATE times SET id = 3 WHERE name = 'EVENING';");
		q_updater.exec("UPDATE times SET id = 4 WHERE name = 'NIGHT';");
#endif
	}

	_database.commit();
}

void Database::updateDatabaseTo_0_0_5(QSqlDatabase& _database)
{
	QSqlQuery q_updater(_database);

	_database.transaction();

	{
		//
		// Обновление таблицы локаций
		//
		q_updater.exec("ALTER TABLE locations ADD COLUMN description TEXT DEFAULT(NULL)");

		// Таблица "Фотографии локаций"
		q_updater.exec("CREATE TABLE locations_photo "
					   "( "
					   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
					   "fk_location_id INTEGER NOT NULL, "
					   "photo BLOB NOT NULL, "
					   "sort_order INTEGER NOT NULL DEFAULT(0) "
					   ")"
					   );

		//
		// Обновление таблицы персонажей
		//
		q_updater.exec("ALTER TABLE characters ADD COLUMN real_name TEXT DEFAULT(NULL)");
		q_updater.exec("ALTER TABLE characters ADD COLUMN description TEXT DEFAULT(NULL)");

		// Таблица "Фотографии персонажей"
		q_updater.exec("CREATE TABLE characters_photo "
					   "( "
					   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
					   "fk_character_id INTEGER NOT NULL, "
					   "photo BLOB NOT NULL, "
					   "sort_order INTEGER NOT NULL DEFAULT(0) "
					   ")"
					   );
	}

	_database.commit();
}

void Database::updateDatabaseTo_0_1_0(QSqlDatabase& _database)
{
	QSqlQuery q_updater(_database);

	_database.transaction();

	{
		//
		// Обновление таблицы сценария
		//
		q_updater.exec("ALTER TABLE scenario ADD COLUMN name TEXT DEFAULT(NULL)");
		q_updater.exec("ALTER TABLE scenario ADD COLUMN synopsis TEXT DEFAULT(NULL)");
	}

	_database.commit();
}

void Database::updateDatabaseTo_0_2_8(QSqlDatabase& _database)
{
	//
	// Заменить при помощи регулярок все
	//       font-family:'*';
	//		 font-size:*;
	//
	QSqlQuery q_updater(_database);

	_database.transaction();

	{
		const QRegularExpression rx_fontFamilyCleaner("font-family:([^;]*);");
		const QRegularExpression rx_fontSizeCleaner("font-size:([^;]*);");

		//
		// Персонажи
		//
		// ... очистим данные
		//
		q_updater.exec("SELECT id, description FROM characters");
		QMap<int, QString> charactersDescriptions;
		while (q_updater.next()) {
			const int id = q_updater.record().value("id").toInt();
			QString description = q_updater.record().value("description").toString();
			description = description.remove(rx_fontFamilyCleaner);
			description = description.remove(rx_fontSizeCleaner);
			charactersDescriptions.insert(id, description);
		}
		//
		// ... обновим данные
		//
		q_updater.prepare("UPDATE characters SET description = ? WHERE id = ?");
		foreach (int id, charactersDescriptions.keys()) {
			q_updater.addBindValue(charactersDescriptions.value(id));
			q_updater.addBindValue(id);
			q_updater.exec();
		}

		//
		// Локации
		//
		// ... очистим данные
		//
		q_updater.exec("SELECT id, description FROM locations");
		QMap<int, QString> locationsDescriptions;
		while (q_updater.next()) {
			const int id = q_updater.record().value("id").toInt();
			QString description = q_updater.record().value("description").toString();
			description = description.remove(rx_fontFamilyCleaner);
			description = description.remove(rx_fontSizeCleaner);
			locationsDescriptions.insert(id, description);
		}
		//
		// ... обновим данные
		//
		q_updater.prepare("UPDATE locations SET description = ? WHERE id = ?");
		foreach (int id, locationsDescriptions.keys()) {
			q_updater.addBindValue(locationsDescriptions.value(id));
			q_updater.addBindValue(id);
			q_updater.exec();
		}

		//
		// Синопсис сценария
		//
		// ... очистим данные
		//
		q_updater.exec("SELECT id, synopsis FROM scenario");
		QMap<int, QString> scenarioSynopsis;
		while (q_updater.next()) {
			const int id = q_updater.record().value("id").toInt();
			QString synopsis = q_updater.record().value("synopsis").toString();
			synopsis = synopsis.remove(rx_fontFamilyCleaner);
			synopsis = synopsis.remove(rx_fontSizeCleaner);
			scenarioSynopsis.insert(id, synopsis);
		}
		//
		// ... обновим данные
		//
		q_updater.prepare("UPDATE scenario SET synopsis = ? WHERE id = ?");
		foreach (int id, scenarioSynopsis.keys()) {
			q_updater.addBindValue(scenarioSynopsis.value(id));
			q_updater.addBindValue(id);
			q_updater.exec();
		}
	}

	_database.commit();
}

void Database::updateDatabaseTo_0_3_3(QSqlDatabase& _database)
{
	QSqlQuery q_updater(_database);

	_database.transaction();

	{
		//
		// Создать таблицу состояний
		//
		q_updater.exec("CREATE TABLE character_states "
					   "( "
					   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
					   "name TEXT UNIQUE NOT NULL "
					   "); "
					   );

		//
		// Добавление полей в таблицу сценария
		//
		q_updater.exec("ALTER TABLE scenario ADD COLUMN additional_info TEXT DEFAULT(NULL)");
		q_updater.exec("ALTER TABLE scenario ADD COLUMN genre TEXT DEFAULT(NULL)");
		q_updater.exec("ALTER TABLE scenario ADD COLUMN author TEXT DEFAULT(NULL)");
		q_updater.exec("ALTER TABLE scenario ADD COLUMN contacts TEXT DEFAULT(NULL)");
		q_updater.exec("ALTER TABLE scenario ADD COLUMN year TEXT DEFAULT(NULL)");
	}

	_database.commit();
}

void Database::updateDatabaseTo_0_4_5(QSqlDatabase& _database)
{
	QSqlQuery q_updater(_database);

	_database.transaction();

	{
		//
		// Добавление поля в таблицу сценария
		//
		q_updater.exec("ALTER TABLE scenario ADD COLUMN is_draft INTEGER NOT NULL DEFAULT(0)");

		//
		// Добавление самого черновика
		//
		q_updater.exec("INSERT INTO scenario (id, text, is_draft) VALUES(null, '', 1)");
	}

	_database.commit();
}

void Database::updateDatabaseTo_0_5_0(QSqlDatabase& _database)
{
	QSqlQuery q_updater(_database);

	_database.transaction();

	{
		//
		// Добавляем новые столбцы
		//
		const QString defaultDateTime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");
		q_updater.exec(
			QString("ALTER TABLE scenario ADD COLUMN version_start_datetime TEXT NOT NULL DEFAULT('%1')")
					.arg(defaultDateTime)
					);
		q_updater.exec(
			QString("ALTER TABLE scenario ADD COLUMN version_end_datetime TEXT NOT NULL DEFAULT('%1')")
					.arg(defaultDateTime)
					);
		q_updater.exec("ALTER TABLE scenario ADD COLUMN version_comment TEXT DEFAULT(NULL)");
		q_updater.exec(
			QString("ALTER TABLE scenario ADD COLUMN uuid TEXT NOT NULL DEFAULT('%1')")
					.arg(QUuid::createUuid().toString())
					);
		q_updater.exec("ALTER TABLE scenario ADD COLUMN is_synced INTEGER DEFAULT(0)");

		//
		// Создаём таблицу для хранения всех запросов
		//
		q_updater.exec("CREATE TABLE _database_history "
					   "( "
					   "id TEXT PRIMARY KEY, " // uuid
					   "query TEXT NOT NULL, "
					   "query_values TEXT NOT NULL, "
					   "datetime TEXT NOT NULL "
					   "); "
					   );
	}

	_database.commit();
}

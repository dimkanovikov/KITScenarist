#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>


namespace DatabaseLayer
{
	class Database
	{
	public:
		/**
		 * @brief Можно ли открыть заданный файл
		 */
		static bool canOpenFile(const QString& _databaseFileName, bool _isLocal);

		/**
		 * @brief Текст ошибки открытия последнего загружаемого файла
		 */
		static QString openFileError();

		/**
		 * @brief Установить текущий файл базы данных
		 */
		static void setCurrentFile(const QString& _databaseFileName);

		/**
		 * @brief Закрыть текущее соединение с базой данных
		 */
		static void closeCurrentFile();

		/**
		 * @brief Получить имя текущей базы данных
		 */
		static QString currentFile();

		/**
		 * @brief Получить объект для выполнения запросов в БД
		 */
		static QSqlQuery query();

		/**
		 * @brief Запустить транзакцию, если ещё не запущена
		 */
		static void transaction();

		/**
		 * @brief Зафиксировать транзакцию, если она была запущена
		 */
		static void commit();

		/**
		 * @brief Состояния базы данных
		 */
		enum State {
			//! Новая база данных
			EmptyFlag,
			//! Схема создана
			SchemeFlag,
			//! Индексы созданы
			IndexesFlag,
			//! Справочники созданы
			EnumsFlag,
			//! Старая версия
			OldVersionFlag
		};
		Q_DECLARE_FLAGS(States, State)

	private:
		static QString CONNECTION_NAME,
					   SQL_DRIVER,
					   DATABASE_NAME;

		/**
		 * @brief Текст ошибки открытия последнего загружаемого файла
		 */
		static QString s_openFileError;

		/**
		 * @brief Счётчик открытых транзакций
		 */
		static int s_openedTransactions;

		/**
		 * @brief Получить объект текущей базы данных
		 */
		static QSqlDatabase instanse();

		/**
		 * @brief Открыть соединение с базой данных
		 */
		static void open(QSqlDatabase& _database,
				const QString& _connectionName,
				const QString& _databaseName
				);
		static Database::States checkState(QSqlDatabase& _database);
		static void createTables(QSqlDatabase& _database);
		static void createIndexes(QSqlDatabase& _database);
		static void createEnums(QSqlDatabase& _database);

		static void updateDatabase(QSqlDatabase& _database);
		/**
		 * @brief Обновить базу данных до версии 0.0.2
		 *
		 * Пересоздаются справочные данные:
		 * - добавлены новые данные в таблицу времён
		 */
		static void updateDatabaseTo_0_0_2(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.0.5
		 *
		 * Добавление информации о персонажах/локациях
		 * - дополнительныем поля
		 * - таблицы хранения фотографий
		 */
		static void updateDatabaseTo_0_0_5(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.1.0
		 *
		 * Добавление информации о сценарии
		 * - название
		 * - синопсис
		 */
		static void updateDatabaseTo_0_1_0(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.2.8
		 *
		 * Убирается информация о шрифте для
		 * - синопсиса проекта
		 * - информации о персонажах
		 * - информации о локациях
		 */
		static void updateDatabaseTo_0_2_8(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.3.3
		 *
		 * - добавляется таблица для состояний персонажа (ГЗК и т.п.)
		 * - добавляются поля для расширенной информации по сценарию
		 */
		static void updateDatabaseTo_0_3_3(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.4.5
		 *
		 * - добавление поля is_draft для сценария
		 * - добавление самого черновика для проекта сценария
		 */
		static void updateDatabaseTo_0_4_5(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.5.0
		 *
		 * - создаётся таблица для хранения всех операций над БД
		 * - таблица для хранения данных сценария
		 * - данные о сценарие переносятся из таблицы со сценарием в таблицу с данными
		 */
		static void updateDatabaseTo_0_5_0(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.5.6
		 *
		 * - блоки с именем time_and_place заменяются на scene_heading
		 */
		static void updateDatabaseTo_0_5_6(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.5.8
		 *
		 * - добавляется таблица research
		 */
		static void updateDatabaseTo_0_5_8(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.5.9
		 *
		 * - внутрь сценария выносятся блоки описания сцен, для поддержания режима поэпизодного плана
		 */
		static void updateDatabaseTo_0_5_9(QSqlDatabase& _database);

		/**
		 * @brief Обновить базу данных до версии 0.7.0
		 *
		 * - в таблицу scenario добавляется поле для хранения схемы
		 */
		static void updateDatabaseTo_0_7_0(QSqlDatabase& _database);
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS(Database::States)
}

#endif // DATABASE_H

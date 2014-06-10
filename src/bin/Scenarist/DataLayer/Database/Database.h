#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>


namespace DatabaseLayer
{
	class Database
	{
	public:
		/**
		 * @brief Установить текущий файл базы данных
		 */
		static void setCurrentFile(const QString& _databaseFileName);

		/**
		 * @brief Получить имя текущей базы данных
		 */
		static QString currentFile();

		/**
		 * @brief Получить объект текущей базы данных
		 */
		static QSqlDatabase instanse();

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
		 * @brief Обновить базу данных до версии 0.0.4
		 *
		 * Упорядочивание таблицы времён
		 */
		static void updateDatabaseTo_0_0_4(QSqlDatabase& _database);

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
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS( Database::States )
}

#endif // DATABASE_H

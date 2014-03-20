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
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS( Database::States )
}

#endif // DATABASE_H

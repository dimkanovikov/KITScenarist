#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

class QString;
class QSqlDatabase;


namespace DatabaseLayer
{
	/**
	 * @brief Класс для считывания-сохранения БД из файла в память и наоборот
	 */
	class DatabaseHelper
	{
	public:
		/**
		 * @brief Сохранить данные из БД в памяти в файл
		 *
		 * @note Происходит полное копирование всех данных
		 */
		static void saveDatabaseToFile(const QString& _databaseFileName);

		/**
		 * @brief Загрузить данные из БД в файле в память
		 */
		static void loadDatabaseFromFile(const QString& _databaseFileName);

	private:
		/**
		 * @brief Очистить данные из БД в памяти
		 */
		static void clearDatabaseInMemory();

		static void loadDatabaseFromFileVersion0(const QSqlDatabase& _fileDatabase);
	};
}

#endif // DATABASEHELPER_H

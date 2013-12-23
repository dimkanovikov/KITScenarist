#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

class QString;


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
		 */
		static void saveDatabaseToFile(const QString& _databaseFileName);

		/**
		 * @brief Загрузить данные из БД в файле в память
		 */
		static void loadDatabaseFromFile(const QString* _databaseFileName);
	};
}

#endif // DATABASEHELPER_H

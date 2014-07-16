#ifndef ABSTRACTEXPORTER_H
#define ABSTRACTEXPORTER_H

#include <QString>

namespace BusinessLogic
{
	class ScenarioDocument;


	/**
	 * @brief Параметры экспорта
	 */
	class ExportParameters
	{
	public:
		ExportParameters() :
			printTilte(false),
			printPagesNumbers(false),
			printScenesNubers(false)
		{}

		/**
		 * @brief Путь к файлу
		 */
		QString filePath;

		/**
		 * @brief Печатать титульную страницу
		 */
		bool printTilte;

		/**
		 * @brief Печатать номера страниц
		 */
		bool printPagesNumbers;

		/**
		 * @brief Печатать номера сцен
		 */
		bool printScenesNubers;

		/**
		 * @brief Приставка сцен
		 */
		QString scenesPrefix;
	};


	/**
	 * @brief Базовый класс экспортера
	 */
	class AbstractExporter
	{
	public:
		/**
		 * @brief Экспорт заданного документа в файл
		 */
		virtual void exportTo(ScenarioDocument* _scenario, const ExportParameters& _exportParameters) const = 0;
	};
}

#endif // ABSTRACTEXPORTER_H

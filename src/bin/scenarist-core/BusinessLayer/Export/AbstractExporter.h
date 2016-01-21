#ifndef ABSTRACTEXPORTER_H
#define ABSTRACTEXPORTER_H

#include <QString>

class QTextDocument;

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
			outline(false),
			printTilte(false),
			printPagesNumbers(false),
			printScenesNumbers(false),
			saveReviewMarks(true)
		{}

		/**
		 * @brief Режим текста: true - поэпизодник, false - сценарий
		 */
		bool outline;

		/**
		 * @brief Путь к файлу
		 */
		QString filePath;

		/**
		 * @brief Название стиля экспорта
		 */
		QString style;

		/**
		 * @brief Печатать титульную страницу
		 */
		bool printTilte;

		/**
		 * @brief Информация с титульного листа
		 */
		/** @{ */
		QString scenarioName;
		QString scenarioAdditionalInfo;
		QString scenarioGenre;
		QString scenarioAuthor;
		QString scenarioContacts;
		QString scenarioYear;
		/** @} */

		/**
		 * @brief Печатать номера страниц
		 */
		bool printPagesNumbers;

		/**
		 * @brief Печатать номера сцен
		 */
		bool printScenesNumbers;

		/**
		 * @brief Приставка сцен
		 */
		QString scenesPrefix;

		/**
		 * @brief Сохранять редакторские пометки
		 */
		bool saveReviewMarks;
	};


	/**
	 * @brief Базовый класс экспортера
	 */
	class AbstractExporter
	{
	public:
		/**
		 * @brief Сформировать из сценария документ, готовый для экспорта
		 * @note Вызывающий получает владение над новым сформированным документом
		 */
		static QTextDocument* prepareDocument(const ScenarioDocument* _scenario,
			const ExportParameters& _exportParameters);

	public:
		virtual ~AbstractExporter() {}

		/**
		 * @brief Экспорт заданного документа в файл
		 */
		virtual void exportTo(ScenarioDocument* _scenario, const ExportParameters& _exportParameters) const = 0;
	};
}

#endif // ABSTRACTEXPORTER_H

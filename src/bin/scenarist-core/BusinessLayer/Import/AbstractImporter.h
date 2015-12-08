#ifndef ABSTRACTIMPORTER_H
#define ABSTRACTIMPORTER_H

#include <QString>


namespace BusinessLogic
{
	/**
	 * @brief Параметры импорта
	 */
	class ImportParameters
	{
	public:
		ImportParameters() :
			outline(false),
			removeScenesNumbers(true),
			insertionMode(ReplaceDocument),
			findCharactersAndLocations(true)
		{}

		/**
		 * @brief Режим текста: true - поэпизодник, false - сценарий
		 */
		bool outline;

		/**
		 * @brief Путь к импортируемому файлу
		 */
		QString filePath;

		/**
		 * @brief Необходимо ли удалять номера сцен
		 */
		bool removeScenesNumbers;

		/**
		 * @brief Типы способа вставки импортируемого текста в сценарий
		 */
		enum InsertionMode {
			ReplaceDocument,
			ToCursorPosition,
			ToDocumentEnd
		};

		/**
		 * @brief Способ вставки импортируемого текста в сценарий
		 */
		InsertionMode insertionMode;

		/**
		 * @brief Искать ли персонажей и локации
		 */
		bool findCharactersAndLocations;

		/**
		 * @brief Сохранять редакторские заметки
		 */
		bool saveReviewMarks;
	};

	/**
	 * @brief Абстракция класса импортирующего сценарий
	 */
	class AbstractImporter
	{
	public:
		virtual ~AbstractImporter() {}

		/**
		 * @brief Импорт сценария
		 * @return Сценарий в xml-формате
		 */
		virtual QString importScenario(const ImportParameters& _importParameters) const = 0;
	};
}

#endif // ABSTRACTIMPORTER_H

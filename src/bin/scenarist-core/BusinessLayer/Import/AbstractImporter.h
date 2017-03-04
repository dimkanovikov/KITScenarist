#ifndef ABSTRACTIMPORTER_H
#define ABSTRACTIMPORTER_H

#include <QApplication>
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

		/**
		 * @brief Список видов файлов, которые могут быть импортированы
		 */
		static QString filters() {
			QString filters;
            filters.append(QApplication::translate("BusinessLogic::AbstractImporter", "All Supported Files") + QLatin1String(" (*.kitsp *.fdx *.trelby *.docx *.doc *.odt *.fountain)"));
			filters.append(";;");
			filters.append(QApplication::translate("BusinessLogic::AbstractImporter","KIT Scenarist Project") + QLatin1String(" (*.kitsp)"));
			filters.append(";;");
			filters.append(QApplication::translate("BusinessLogic::AbstractImporter","Final Draft screenplay") + QLatin1String(" (*.fdx)"));
			filters.append(";;");
			filters.append(QApplication::translate("BusinessLogic::AbstractImporter","Trelby screenplay") + QLatin1String(" (*.trelby)"));
			filters.append(";;");
			filters.append(QApplication::translate("BusinessLogic::AbstractImporter","Office Open XML") + QLatin1String(" (*.docx *.doc)"));
			filters.append(";;");
			filters.append(QApplication::translate("BusinessLogic::AbstractImporter","OpenDocument Text") + QLatin1String(" (*.odt)"));
            filters.append(";;");
            filters.append(QApplication::translate("BusinessLogic::AbstractImporter","Fountain Text") + QLatin1String(" (*.fountain)"));

			return filters;
		}
	};
}

#endif // ABSTRACTIMPORTER_H

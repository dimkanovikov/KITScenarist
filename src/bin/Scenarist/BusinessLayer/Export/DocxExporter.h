#ifndef DOCXEXPORTER_H
#define DOCXEXPORTER_H

#include "AbstractExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <QString>

class QtZipWriter;


namespace BusinessLogic
{
	/**
	 * @brief Экспортер в DOCX
	 */
	class DocxExporter : public AbstractExporter
	{
	public:
		DocxExporter();

		/**
		 * @brief Экспорт заданного документа в указанный файл
		 */
		void exportTo(ScenarioDocument* _scenario, const ExportParameters& _exportParameters) const;

	private:
		/**
		 * @brief Записать все статичные данные в файл
		 */
		void writeStaticData(QtZipWriter* _zip, const ExportParameters& _exportParameters) const;

		/**
		 * @brief Записать стили
		 */
		void writeStyles(QtZipWriter* _zip) const;

		/**
		 * @brief Записать верхний колонтитул
		 */
		void writeHeader(QtZipWriter* _zip, const ExportParameters& _exportParameters) const;

		/**
		 * @brief Записать нижний колонтитул
		 */
		void writeFooter(QtZipWriter* _zip, const ExportParameters& _exportParameters) const;

		/**
		 * @brief Записать документ
		 */
		void writeDocument(QtZipWriter* _zip, ScenarioDocument* _scenario,
			const ExportParameters& _exportParameters) const;
	};
}

#endif // DOCXEXPORTER_H

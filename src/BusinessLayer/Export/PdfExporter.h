#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include "AbstractExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>

#include <QTextCharFormat>
#include <QTextBlockFormat>

namespace BusinessLogic
{
	/**
	 * @brief Экспортер в PDF
	 */
	class PdfExporter : public AbstractExporter
	{
	public:
		PdfExporter();

		/**
		 * @brief Экспорт заданного документа в указанный файл
		 */
		void exportTo(QTextDocument* _document, const QString& _toFile) const;

	private:
		/**
		 * @brief Подготовить документ к печати
		 *
		 * @note Вызывающий получает владение над новым сформированным документом
		 */
		QTextDocument* prepareDocument(QTextDocument* _document) const;

		/**
		 * @brief Получить стиль оформления символов для заданного типа
		 */
		QTextCharFormat charFormatForType(ScenarioTextBlockStyle::Type _type) const;

		/**
		 * @brief Получить стиль оформления абзаца для заданного типа
		 */
		QTextBlockFormat blockFormatForType(ScenarioTextBlockStyle::Type _type) const;
	};
}

#endif // PDFEXPORTER_H

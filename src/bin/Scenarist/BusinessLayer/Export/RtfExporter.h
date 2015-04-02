#ifndef RTFEXPORTER_H
#define RTFEXPORTER_H

#include "AbstractExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <QString>


namespace BusinessLogic
{
	/**
	 * @brief Экспортер в RTF
	 */
	class RtfExporter : public AbstractExporter
	{
	public:
		RtfExporter();

		/**
		 * @brief Экспорт заданного документа в указанный файл
		 */
		void exportTo(ScenarioDocument* _scenario, const ExportParameters& _exportParameters) const;

	private:
		/**
		 * @brief Преамбула RTF-файла
		 */
		QString header(const ExportParameters& _exportParameters) const;

		/**
		 * @brief RTF стиль текущего блока
		 */
		/** @{ */
		QString style(QTextCursor& _documentCursor) const;
		QString style(ScenarioBlockStyle::Type _type) const;
		QString style(const QTextBlockFormat& _blockFormat, const QTextCharFormat& _charFormat) const;
		/** @} */

		/**
		 * @brief Перевести UTF-строку в коды для RTF-формата
		 */
		QString stringToUtfCode(const QString& _text) const;
	};
}

#endif // RTFEXPORTER_H

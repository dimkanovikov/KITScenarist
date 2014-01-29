#ifndef RTFEXPORTER_H
#define RTFEXPORTER_H

#include "AbstractExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>

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
		void exportTo(QTextDocument* _document, const QString& _toFile) const;

	private:
		/**
		 * @brief Преамбула RTF-файла
		 */
		QString header() const;

		QString style(ScenarioTextBlockStyle::Type _type) const;

		QString stringToUtfCode(const QString& _text) const;
	};
}

#endif // RTFEXPORTER_H

#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>

#include "AbstractExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <QTextCharFormat>
#include <QTextBlockFormat>

class QPrinter;


namespace BusinessLogic
{
	/**
	 * @brief Экспортер в PDF
	 */
	class PdfExporter : public QObject, public AbstractExporter
	{
		Q_OBJECT

	public:
		explicit PdfExporter(QObject* _parent = 0);

		/**
		 * @brief Экспорт заданного документа в указанный файл
		 */
		void exportTo(ScenarioDocument* _scenario, const ExportParameters& _exportParameters) const;

		/**
		 * @brief Предварительный просмотр и печать
		 */
		void printPreview(ScenarioDocument* _scenario, const ExportParameters& _exportParameters);

    signals:
        /**
         * @brief Документ был напечатан
         */
        void printed();

	private slots:
		/**
		 * @brief Печатать
		 */
		void aboutPrint(QPrinter* _printer);

	private:
		/**
		 * @brief Подготовить принтер к печати
		 *
		 * @note Вызывающий получает владение над новым сформированным документом
		 */
		QPrinter* preparePrinter(const QString& _forFile = QString::null) const;

	private:
		/**
		 * @brief Документ для печати
		 */
		QTextDocument* m_documentForPrint;

        /**
         * @brief Последняя позиция прокрутки предпросмотра сценария
         */
        static QPair<ScenarioDocument*, int> m_lastScenarioPreviewScrollPosition;
	};
}

#endif // PDFEXPORTER_H

#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <QTextCharFormat>
#include <QTextBlockFormat>

class QPrinter;


namespace BusinessLogic
{
	/**
	 * @brief Экспортер в PDF
	 */
	class PdfExporter : public QObject
	{
		Q_OBJECT

	public:
		explicit PdfExporter(QObject* _parent = 0);

		/**
		 * @brief Экспорт заданного документа в указанный файл
		 */
		void exportTo(QTextDocument* _document, const QString& _toFile) const;

		/**
		 * @brief Предварительный просмотр и печать
		 */
		void printPreview(QTextDocument* _document);

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

		/**
		 * @brief Подготовить документ к печати
		 *
		 * @note Вызывающий получает владение над новым сформированным документом
		 */
		QTextDocument* prepareDocument(QTextDocument* _document) const;

		/**
		 * @brief Получить стиль оформления символов для заданного типа
		 */
		QTextCharFormat charFormatForType(ScenarioBlockStyle::Type _type) const;

		/**
		 * @brief Получить стиль оформления абзаца для заданного типа
		 */
		QTextBlockFormat blockFormatForType(ScenarioBlockStyle::Type _type) const;

	private:
		/**
		 * @brief Документ для печати
		 */
		QTextDocument* m_documentForPrint;
	};
}

#endif // PDFEXPORTER_H

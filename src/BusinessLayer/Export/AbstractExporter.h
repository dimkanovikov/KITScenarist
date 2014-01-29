#ifndef ABSTRACTEXPORTER_H
#define ABSTRACTEXPORTER_H

class QTextDocument;
class QString;


namespace BusinessLogic
{
	/**
	 * @brief Базовый класс экспортера
	 */
	class AbstractExporter
	{
	public:
		/**
		 * @brief Экспорт заданного документа в файл
		 */
		virtual void exportTo(QTextDocument* _document, const QString& _toFile) const = 0;
	};
}

#endif // ABSTRACTEXPORTER_H

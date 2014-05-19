#ifndef SCENARIOTEXTDOCUMENT_H
#define SCENARIOTEXTDOCUMENT_H

#include <QTextDocument>


namespace BusinessLogic
{
	class ScenarioXml;


	/**
	 * @brief Расширение класса текстового документа для предоставления интерфейса для обработки mime
	 */
	class ScenarioTextDocument : public QTextDocument
	{
		Q_OBJECT

	public:
		explicit ScenarioTextDocument(QObject *parent, ScenarioXml* _xmlHandler);

		/**
		 * @brief Получить майм представление данных в указанном диапазоне
		 */
		QString mimeFromSelection(int _startPosition, int _endPosition) const;

		/**
		 * @brief Вставить данные в указанную позицию документа
		 */
		void insertFromMime(int _insertPosition, const QString& _mimeData);

	signals:

	private:
		/**
		 * @brief Обработчик xml
		 */
		ScenarioXml* m_xmlHandler;
	};
}


#endif // SCENARIOTEXTDOCUMENT_H

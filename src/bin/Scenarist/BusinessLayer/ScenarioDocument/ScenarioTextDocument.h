#ifndef SCENARIOTEXTDOCUMENT_H
#define SCENARIOTEXTDOCUMENT_H

#include <QTextDocument>
#include <QTextCursor>

namespace Domain {
	class ScenarioChange;
}

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
		 * @brief Загрузить сценарий
		 */
		void load(const QString& _scenarioXml);

		/**
		 * @brief Получить майм представление данных в указанном диапазоне
		 */
		QString mimeFromSelection(int _startPosition, int _endPosition) const;

		/**
		 * @brief Вставить данные в указанную позицию документа
		 */
		void insertFromMime(int _insertPosition, const QString& _mimeData);

		/**
		 * @brief Применить патч
		 */
		void applyPatch(const QString& _patch);

		/**
		 * @brief Сохранить изменения текста
		 */
		Domain::ScenarioChange* saveChanges();

		/**
		 * @brief Собственные реализации отмены/повтора последнего действия
		 */
		/** @{ */
		void undoReimpl();
		void redoReimpl();
		/** @} */

		/**
		 * @brief Собственные реализации проверки доступности отмены/повтора последнего действия
		 */
		/** @{ */
		bool isUndoAvailableReimpl() const;
		bool isRedoAvailableReimpl() const;
		/** @} */

		/**
		 * @brief Установить курсор в заданную позицию
		 */
		void setCursorPosition(QTextCursor& _cursor, int _position,
			QTextCursor::MoveMode _moveMode = QTextCursor::MoveAnchor);

	signals:
		/**
		 * @brief Сигналы уведомляющие об этапах применения патчей
		 */
		/** @{ */
		void beforePatchApply();
		void afterPatchApply();
		/** @} */

	private:
		/**
		 * @brief Обработчик xml
		 */
		ScenarioXml* m_xmlHandler;

		/**
		 * @brief Применяется ли патч в данный момент
		 */
		bool m_isPatchApplyProcessed;

		/**
		 * @brief Текст сценария с сохранёнными изменениями
		 */
		QString m_lastScenarioXml;

		/**
		 * @brief MD5-хэш текста сценария с сохранёнными изменениями
		 */
		QByteArray m_lastScenarioXmlHash;

		/**
		 * @brief Стеки для отмены/повтора последнего действия
		 */
		/** @{ */
		QList<Domain::ScenarioChange*> m_undoStack;
		QList<Domain::ScenarioChange*> m_redoStack;
		/** @{ */
	};
}


#endif // SCENARIOTEXTDOCUMENT_H

#ifndef SCENARIOXML_H
#define SCENARIOXML_H

#include <QString>


namespace BusinessLogic
{
	class ScenarioDocument;
	class ScenarioModelItem;


	/**
	 * @brief Фасад для преобразований сценария в/из xml-описания
	 */
	class ScenarioXml
	{
	public:
		/**
		 * @brief Конструктор фасада для работы с xml
		 *
		 * @note Документ должен быть обязательно задан
		 */
		ScenarioXml(ScenarioDocument* _scenario);

		/**
		 * @brief Записать сценарий в xml-строку из заданного диапазона текста
		 */
		QString scenarioToXml(int _startPosition = 0, int _endPosition = 0);

		/**
		 * @brief Записать сценарий в xml-строку из заданного диапазона элементов
		 */
		QString scenarioToXml(ScenarioModelItem* _fromItem, ScenarioModelItem* _toItem);

		/**
		 * @brief Загрузить сценарий из xml в документ
		 */
		void xmlToScenario(int _position, const QString& _xml);

		/**
		 * @brief Загрузить сценарий из xml после заданного элемента для его родителя
		 */
		void xmlToScenario(ScenarioModelItem* _insertParent, ScenarioModelItem* _insertBefore, const QString& _xml, bool _removeLastMime);

	private:
		/**
		 * @brief Удалить последний преобразованный в майм-данные блок текста
		 * @return Количество удалённых символов
		 *
		 * Используетмя при перемещении элементов по дереву
		 */
		int removeLastMime();

	private:
		/**
		 * @brief Документ сценария
		 */
		ScenarioDocument* m_scenario;

		/**
		 * @brief Последние позиции создания майм-данных
		 */
		/** @{ */
		int m_lastMimeFrom;
		int m_lastMimeTo;
		/** @} */
	};
}

#endif // SCENARIOXML_H

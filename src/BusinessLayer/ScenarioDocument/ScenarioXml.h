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
		 * @brief Записать сценарий в xml-строку из заданного диапозона текста
		 */
		QString scenarioToXml(int _startPosition = 0, int _endPosition = 0);

		/**
		 * @brief Загрузить сценарий из xml в документ
		 */
		void xmlToScenario(int _position, const QString& _xml);

		/**
		 * @brief Загрузить сценарий из xml после заданного элемента для его родителя
		 */
		void xmlToScenario(ScenarioModelItem* _insertParent, ScenarioModelItem* _insertBefore, const QString& _xml);

	private:
		ScenarioDocument* m_scenario;
	};
}

#endif // SCENARIOXML_H

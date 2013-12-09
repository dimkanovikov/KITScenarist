#ifndef SCENARIOOBJECT_H
#define SCENARIOOBJECT_H


namespace Scenario
{
	/**
	 * @brief Базовый класс элементов сценария
	 *
	 * @note Должен описывать интерфейс базовых действий состовляющих сценария
	 *		 Хронометраж
	 *		 Отображение в текстовом редакторе
	 */
	class ScenarioObject
	{
	public:
		ScenarioObject(ScenarioObject* _parent = 0);
		virtual ~ScenarioObject();

	private:
		/**
		 * @brief Родительский элемент объекта
		 */
		ScenarioObject* m_parent;
	};
}

#endif // SCENARIOOBJECT_H

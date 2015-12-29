#ifndef SCENARIOTEXTCORRECTOR_H
#define SCENARIOTEXTCORRECTOR_H

class QTextDocument;


namespace BusinessLogic
{
	/**
	 * @brief Класс корректирующий текст сценария
	 */
	class ScenarioTextCorrector
	{
	public:
		ScenarioTextCorrector();

		/**
		 * @brief Скорректировать документ на разрывах страниц
		 */
		void correctScenarioText(QTextDocument* _document, int _startPosition);
	};
}

#endif // SCENARIOTEXTCORRECTOR_H

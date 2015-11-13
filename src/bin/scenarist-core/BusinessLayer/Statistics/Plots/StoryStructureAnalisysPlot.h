#ifndef STORYSTRUCTUREANALISYSPLOT_H
#define STORYSTRUCTUREANALISYSPLOT_H

#include "AbstractPlot.h"


namespace BusinessLogic
{
	/**
	 * @brief График структурного анализа истории
	 */
	class StoryStructureAnalisysPlot : public AbstractPlot
	{
	public:
		StoryStructureAnalisysPlot() {}

		/**
		 * @brief Получить название графика
		 */
		QString plotName(const StatisticsParameters& _parameters) const;

		/**
		 * @brief Сформировать график по заданному сценарию с установленными параметрами
		 */
		Plot makePlot(QTextDocument* _scenario,
			const StatisticsParameters& _parameters) const;

	private:
		/**
		 * @brief Данные о сцене
		 */
		class SceneData {
		public:
			SceneData() : page(0), number(0), chron(0), actionChron(0), dialoguesChron(0),
				charactersCount(0), dialoguesCount(0)
			{}

			/**
			 * @brief Название
			 */
			QString name;

			/**
			 * @brief Страница, на которой начинается
			 */
			int page;

			/**
			 * @brief Номер
			 */
			int number;

			/**
			 * @brief Хронометраж
			 */
			int chron;

			/**
			 * @brief Хронометраж действий
			 */
			int actionChron;

			/**
			 * @brief Хронометраж реплик
			 */
			int dialoguesChron;

			/**
			 * @brief Количество персонажей
			 */
			int charactersCount;

			/**
			 * @brief Количество реплик
			 */
			int dialoguesCount;
		};
	};
}

#endif // STORYSTRUCTUREANALISYSPLOT_H

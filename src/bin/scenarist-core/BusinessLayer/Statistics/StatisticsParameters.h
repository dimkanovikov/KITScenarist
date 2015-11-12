#ifndef STATISTICSPARAMETERS
#define STATISTICSPARAMETERS

#include <QString>


namespace BusinessLogic
{
	/**
	 * @brief Параметры отчёта
	 */
	class StatisticsParameters
	{
	public:
		StatisticsParameters() {}

		/**
		 * @brief Вид отчёта
		 */
		enum Type {
			Report,
			Plot
		} type;

		/**
		 * @brief Тип отчёта
		 */
		enum ReportType {
			SummaryReport,
			SceneReport,
			LocationReport,
			CastReport,
			CharacterReport
		} reportType;

		/**
		 * @brief Тип графика
		 */
		enum PlotType {
			StoryStructureAnalisysPlot
		} plotType;

		/**
		 * @brief Параметры отчёта по статистике сценария
		 */
		/** @{ */
		bool summaryText;
		bool summaryScenes;
		bool summaryLocations;
		bool summaryCharacters;
		/** @} */

		/**
		 * @brief Параметры отчёта по сценам
		 */
		/** @{ */
		bool sceneShowCharacters;
		int sceneSortByColumn;
		/** @} */

		/**
		 * @brief Параметры отчёта по локациям
		 */
		/** @{ */
		bool locationExtendedView;
		int locationSortByColumn;
		/** @} */

		/**
		 * @brief Параметры отчёта по персонажам
		 */
		/** @{ */
		bool castShowSpeakingAndNonspeakingScenes;
		int castSortByColumn;
		/** @} */

		/**
		 * @brief Параметры отчёта по персонажу
		 */
		/** @{ */
		QString characterName;
		/** @} */

		/**
		 * @brief Параметры графика структурного анализа истории
		 */
		/** @{ */
		bool storyStructureAnalisysSceneChron;
		bool storyStructureAnalisysActionChron;
		bool storyStructureAnalisysDialoguesChron;
		bool storyStructureAnalisysCharactersCount;
		bool storyStructureAnalisysDialoguesCount;
		/** @} */
	};
}

#endif // STATISTICSPARAMETERS


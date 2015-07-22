#ifndef ABSTRACTREPORT
#define ABSTRACTREPORT

#include <QStringList>


namespace BusinessLogic
{
	/**
	 * @brief Параметры отчёта
	 */
	class ReportParameters
	{
	public:
		ReportParameters() {}

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
			Statistics,
			Scene,
			Location,
			Cast,
			Character,
			Script
		} reportType;

		/**
		 * @brief Параметры отчёта по статистике сценария
		 */
		/** @{ */
		bool statisticsSummaryText;
		bool statisticsScenes;
		bool statisticsLocations;
		bool statisticsCharacters;
		/** @} */

		/**
		 * @brief Параметры отчёта по сценам
		 */
		int sceneSortByColumn;

		/**
		 * @brief Параметры отчёта по локациям
		 */
		int locationSortByColumn;

		/**
		 * @brief Параметры отчёта по персонажам
		 */
		/** @{ */
		bool castNumberOfScenes;
		bool castNumberOdNonspekingScenes;
		bool castTotalSceneAppearance;
		bool castTotalDialogues;
		int castSortByColumn;
		/** @} */

		/**
		 * @brief Параметры отчёта по персонажу
		 */
		/** @{ */
		QString characterName;
		bool characterIncludeSceneHeadings;
		bool characterIncludeDialogs;
		bool characterIncludeArcBeats;
		/** @} */

		/**
		 * @brief Параметры отчёта по тексту сценария
		 */
		/** @{ */
		QStringList scriptElements;
		bool scriptUseCourierNew;
		/** @} */
	};
}

#endif // ABSTRACTREPORT


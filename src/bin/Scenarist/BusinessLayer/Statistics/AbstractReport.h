#ifndef ABSTRACTREPORT
#define ABSTRACTREPORT

#include <QStringList>

class QTextDocument;


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
			StatisticsReport,
			SceneReport,
			LocationReport,
			CastReport,
			CharacterReport
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
	};

	/**
	 * @brief Базовый класс для отчёта
	 */
	class AbstractReport
	{
	public:
		virtual ~AbstractReport() {}

		/**
		 * @brief Получить название отчёта
		 */
		virtual QString reportName(const ReportParameters& _parameters) const = 0;

		/**
		 * @brief Сформировать отчёт по заданному сценарию с установленными параметрами
		 */
		virtual QString makeReport(QTextDocument* _scenario,
			const ReportParameters& _parameters) const = 0;
	};
}

#endif // ABSTRACTREPORT


#ifndef LOCATIONREPORT_H
#define LOCATIONREPORT_H

#include "AbstractReport.h"

#include <QList>

namespace BusinessLogic
{
	/**
	 * @brief Отчёт по локациям
	 */
	class LocationReport : public AbstractReport
	{
	public:
		LocationReport() {}

		/**
		 * @brief Название
		 */
		QString reportName(const StatisticsParameters&) const;

		/**
		 * @brief Подготовить отчёт
		 */
		QString makeReport(QTextDocument *_scenario, const StatisticsParameters &_parameters) const;

	private:
		/**
		 * @brief Данные о локации, группе сцен или сцене
		 */
		class ReportData {
		public:
			ReportData() : page(0), number(0), chron(0) {}

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
			 * @brief Вложенные элементы
			 */
			QList<ReportData*> childs;

			/**
			 * @brief Вспомогательные функции для сортировки списка с данными
			 */
			/** @{ */
			static bool sortAlphabetical(ReportData* lhs, ReportData* rhs) {
				return lhs->name < rhs->name;
			}
			static bool sortFromMostToLeast(ReportData* lhs, ReportData* rhs) {
				return lhs->childs.size() > rhs->childs.size();
			}
			static bool sortFromLeastToMost(ReportData* lhs, ReportData* rhs) {
				return !sortFromMostToLeast(lhs, rhs);
			}
			static bool sortFromLongestToShortest(ReportData* lhs, ReportData* rhs) {
				return lhs->chron > rhs->chron;
			}
			static bool sortFromShortestToLongest(ReportData* lhs, ReportData* rhs) {
				return !sortFromLongestToShortest(lhs, rhs);
			}
			/** @} */
		};
	};
}

#endif // LOCATIONREPORT_H

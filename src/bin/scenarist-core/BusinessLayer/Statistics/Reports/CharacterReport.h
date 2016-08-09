#ifndef CHARACTERREPORT_H
#define CHARACTERREPORT_H

#include "AbstractReport.h"

#include <QVariant>


namespace BusinessLogic
{
	/**
	 * @brief Отчёт по персонажу
	 */
	class CharacterReport : public AbstractReport
	{
	public:
		CharacterReport() {}

		/**
		 * @brief Название
		 */
		QString reportName(const StatisticsParameters &_parameters) const;

		/**
		 * @brief Подготовить отчёт
		 */
		QString makeReport(QTextDocument *_scenario, const StatisticsParameters &_parameters) const;

	private:
		/**
		 * @brief Данные о репликах персонажа в сцене
		 */
		class ReportData {
		public:
			ReportData() : page(0), number(0) {}

			/**
			 * @brief Сцена
			 */
			QString scene;

			/**
			 * @brief Страница, на которой начинается
			 */
			int page;

			/**
			 * @brief Номер
			 */
			int number;

			/**
             * @brief Реплики персонажа <персонаж, текст, позиция>
			 */
            QList<QVariantList> dialogues;
		};
	};
}

#endif // CHARACTERREPORT_H

#ifndef CHARACTERREPORT_H
#define CHARACTERREPORT_H

#include "AbstractReport.h"


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
		QString reportName(const ReportParameters &_parameters) const;

		/**
		 * @brief Подготовить отчёт
		 */
		QString makeReport(QTextDocument *_scenario, const ReportParameters &_parameters) const;

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
			 * @brief Реплики персонажа
			 */
			QStringList dialogues;
		};
	};
}

#endif // CHARACTERREPORT_H

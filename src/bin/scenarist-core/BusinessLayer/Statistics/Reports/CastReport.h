#ifndef CASTREPORT_H
#define CASTREPORT_H

#include "AbstractReport.h"


namespace BusinessLogic
{
	/**
	 * @brief Отчёт по сценам
	 */
	class CastReport : public AbstractReport
	{
	public:
		CastReport() {}

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
		 * @brief Данные о персонаже
		 */
		class CharacterData {
		public:
			CharacterData(const QString& _name) :
				name(_name), dialogsCount(0), speakingScenesCount(0), nonspeakingScenesCount(0) {}

			/**
			 * @brief Имя
			 */
			QString name;

			/**
			 * @brief Количество реплик
			 */
			int dialogsCount;

			/**
			 * @brief Количество сцен с репликами
			 */
			int speakingScenesCount;

			/**
			 * @brief Количество сцен без реплик
			 */
			int nonspeakingScenesCount;

			/**
			 * @brief Общее кол-во сцен
			 */
			int scenesCount() const { return speakingScenesCount + nonspeakingScenesCount; }

			/**
			 * @brief Вспомогательные функции для сортировки списка с данными
			 */
			/** @{ */
			static bool sortAlphabetical(CharacterData* lhs, CharacterData* rhs) {
				return lhs->name < rhs->name;
			}
			static bool sortFromMostToLeastScenes(CharacterData* lhs, CharacterData* rhs) {
				return lhs->scenesCount() > rhs->scenesCount();
			}
			static bool sortFromLeastToMostScenes(CharacterData* lhs, CharacterData* rhs) {
				return !sortFromMostToLeastScenes(lhs, rhs);
			}
			static bool sortFromMostToLeastDialogs(CharacterData* lhs, CharacterData* rhs) {
				return lhs->dialogsCount > rhs->dialogsCount;
			}
			static bool sortFromLeastToMostDialogs(CharacterData* lhs, CharacterData* rhs) {
				return !sortFromMostToLeastDialogs(lhs, rhs);
			}
			/** @} */
		};
	};
}

#endif // CASTREPORT_H

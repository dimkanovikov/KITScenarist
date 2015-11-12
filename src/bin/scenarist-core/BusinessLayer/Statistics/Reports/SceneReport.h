#ifndef SCENEREPORT_H
#define SCENEREPORT_H

#include "AbstractReport.h"

#include <QList>


namespace BusinessLogic
{
	/**
	 * @brief Отчёт по сценам
	 */
	class SceneReport : public AbstractReport
	{
	public:
		SceneReport() {}

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
		 * @brief Персонаж сцены
		 */
		class SceneCharacter {
		public:
			SceneCharacter(const QString& _name) :
				name(_name), isFirstOccurence(true), dialogsCount(0) {}

			/**
			 * @brief Имя персонажа
			 */
			QString name;

			/**
			 * @brief Первое появление в сценарии
			 */
			bool isFirstOccurence;

			/**
			 * @brief Количество реплик
			 */
			int dialogsCount;

			/**
			 * @brief Проверить равенство двух персонажей
			 */
			bool operator ==(const SceneCharacter& _rhs) {
				return name == _rhs.name;
			}
		};

		/**
		 * @brief Данные о сцене
		 */
		class SceneData {
		public:
			SceneData() : page(0), number(0), chron(0) {}

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
			 * @brief Персонажи сцены
			 */
			QList<SceneCharacter> characters;

			/**
			 * @brief Получить индекс персонажа. Если персонажа нет в списке возвращается -1
			 */
			int characterIndex(const QString& _characterName) const {
				int index = -1;
				for (int characterIndex = 0; characterIndex < characters.size(); ++characterIndex) {
					if (characters.at(characterIndex).name == _characterName) {
						index = characterIndex;
						break;
					}
				}
				return index;
			}

			/**
			 * @brief Вспомогательные функции для сортировки списка с данными
			 */
			/** @{ */
			static bool sortAlphabetical(SceneData* lhs, SceneData* rhs) {
				return lhs->name < rhs->name;
			}
			static bool sortFromLongestToShortest(SceneData* lhs, SceneData* rhs) {
				return lhs->chron > rhs->chron;
			}
			static bool sortFromShortestToLongest(SceneData* lhs, SceneData* rhs) {
				return !sortFromLongestToShortest(lhs, rhs);
			}
			static bool sortFromMassiveToUnmanned(SceneData* lhs, SceneData* rhs) {
				return lhs->characters.size() > rhs->characters.size();
			}
			static bool sortFromUnmannedToMassive(SceneData* lhs, SceneData* rhs) {
				return !sortFromMassiveToUnmanned(lhs, rhs);
			}
			/** @} */
		};
	};
}

#endif // SCENEREPORT_H

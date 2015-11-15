#ifndef CHARACTERSACTIVITYPLOT_H
#define CHARACTERSACTIVITYPLOT_H

#include "AbstractPlot.h"


namespace BusinessLogic
{
	/**
	 * @brief График структурного анализа истории
	 */
	class CharactersActivityPlot : public AbstractPlot
	{
	public:
		CharactersActivityPlot() {}

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
		 * @brief Персонаж сцены
		 */
		class SceneCharacter {
		public:
			SceneCharacter(const QString& _name) :
				name(_name), isFirstOccurence(true), dialoguesCount(0) {}

			/**
			 * @brief Имя персонажа
			 */
			QString name;

			/**
			 * @brief Первое появление в сценарии
			 */
			bool isFirstOccurence;

			/**
			 * @brief Хронометраж реплик
			 */
			int dialoguesChron;

			/**
			 * @brief Количество реплик
			 */
			int dialoguesCount;

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
			SceneData() : number(0), chron(0) {}

			/**
			 * @brief Номер
			 */
			int number;

			/**
			 * @brief Хронометраж
			 */
			qreal chron;

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
		};
	};
}

#endif // CHARACTERSACTIVITYPLOT_H

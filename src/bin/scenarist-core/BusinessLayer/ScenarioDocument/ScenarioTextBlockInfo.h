#ifndef SCENARIOTEXTBLOCKINFO_H
#define SCENARIOTEXTBLOCKINFO_H

#include <QTextBlockUserData>

namespace BusinessLogic
{
	/**
	 * @brief Класс для хранения информации о сцене
	 */
	class ScenarioTextBlockInfo : public QTextBlockUserData
	{
	public:
		ScenarioTextBlockInfo();

		/**
		 * @brief Получить номер сцены
		 */
		int sceneNumber() const;

		/**
		 * @brief Установить номер сцены
		 */
		void setSceneNumber(int _number);

		/**
		 * @brief Получить цвета сцены
		 */
		QString colors() const;

		/**
		 * @brief Установить цвета сцены
		 */
		void setColors(const QString& _colors);

		/**
		 * @brief Получить описание
		 */
		QString description(bool htmlEscaped = false) const;

		/**
		 * @brief Установить описание
		 */
		void setDescription(const QString& _description, bool htmlEscaped = false);

		/**
		 * @brief Создать дубликат
		 */
		ScenarioTextBlockInfo* clone() const;

	private:
		/**
		 * @brief Номер сцены
		 */
		int m_sceneNumber;

		/**
		 * @brief Цвета сцены
		 */
		QString m_colors;

		/**
		 * @brief Текст описания
		 */
		QString m_description;
	};
}

#endif // SCENARIOTEXTBLOCKINFO_H

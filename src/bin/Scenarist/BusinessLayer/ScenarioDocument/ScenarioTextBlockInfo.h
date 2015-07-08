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
		 * @brief Получить цвет сцены
		 */
		QColor color() const;

		/**
		 * @brief Установить цвет сцены
		 */
		void setColor(const QColor& _color);

		/**
		 * @brief Получить описание
		 */
		QString description(bool htmlEscaped = false) const;

		/**
		 * @brief Установить описание
		 */
		void setDescription(const QString& _description, bool htmlEscaped = false);

	private:
		/**
		 * @brief Номер сцены
		 */
		int m_sceneNumber;

		/**
		 * @brief Цвет сцены
		 */
		QColor m_color;

		/**
		 * @brief Текст описания
		 */
		QString m_description;
	};
}

#endif // SCENARIOTEXTBLOCKINFO_H

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
		ScenarioTextBlockInfo(const QString& _synopsis = QString());

		/**
		 * @brief Получить синопсис
		 */
		QString synopsis() const;

		/**
		 * @brief Установить синопсис
		 */
		void setSynopsis(const QString& _synopsis);

	private:
		/**
		 * @brief Текст синопсиса
		 */
		QString m_synopsis;
	};
}

#endif // SCENARIOTEXTBLOCKINFO_H

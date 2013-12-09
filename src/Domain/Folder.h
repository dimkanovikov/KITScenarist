#ifndef FOLDER_H
#define FOLDER_H

#include "ScenarioObject.h"

#include <QString>
#include <QList>


namespace Scenario
{
	/**
	 * @brief Класс папки сценария
	 */
	class Folder : public ScenarioObject
	{
	public:
		Folder();

	private:
		/**
		 * @brief Название папки
		 */
		QString m_name;

		/**
		 * @brief Описание папки
		 */
		QString m_description;

		/**
		 * @brief Вложенные в папку папки и сцены
		 *
		 * @note В папку могут быть включены только папки и сцены
		 */
		QList<ScenarioObject*> m_children;
	};
}

#endif // FOLDER_H

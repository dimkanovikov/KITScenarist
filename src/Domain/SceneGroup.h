#ifndef SCENEGROUP_H
#define SCENEGROUP_H

#include "ScenarioObject.h"

#include <QString>
#include <QList>


namespace Scenario
{
	/**
	 * @brief Группа сцен
	 */
	class SceneGroup : public ScenarioObject
	{
	public:
		SceneGroup();

	private:
		/*
		 * Параметры:
		 *
		 * тип группы (вставка, воспоминание и т.п.): наполняемый список [из базы данных]
		 *
		 */


		/**
		 * @brief Название группы
		 */
		QString m_name;

		/**
		 * @brief Включённые в группу сцены
		 *
		 * @note В группу могут быть включены только сцены
		 */
		QList<ScenarioObject*> m_scenes;
	};
}

#endif // SCENEGROUP_H

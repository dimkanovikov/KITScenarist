#ifndef SCENECHARACTERSHANDLER_H
#define SCENECHARACTERSHANDLER_H

#include "StandardKeyHandler.h"

class QStringListModel;


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке участники сцени
	 */
	class SceneCharactersHandler : public StandardKeyHandler
	{
	public:
		SceneCharactersHandler(UserInterface::ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */

	private:
		/**
		 * @brief Сохранить персонажей
		 */
		void storeCharacters() const;

		/**
		 * @brief Отфильтрованная модель персонажей
		 */
		QStringListModel* m_filteredCharactersModel;
	};
}

#endif // SCENECHARACTERSHANDLER_H

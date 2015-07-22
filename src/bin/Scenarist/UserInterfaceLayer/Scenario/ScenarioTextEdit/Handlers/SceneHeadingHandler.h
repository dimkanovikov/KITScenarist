#ifndef SCENEHEADINGHANDLER_H
#define SCENEHEADINGHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке время и место
	 */
	class SceneHeadingHandler : public StandardKeyHandler
	{
	public:
		SceneHeadingHandler(UserInterface::ScenarioTextEdit* _editor);

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
		void storeSceneParameters() const;
	};
}

#endif // SCENEHEADINGHANDLER_H

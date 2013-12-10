#ifndef SCENEHEADERHANDLER_H
#define SCENEHEADERHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке время и место
	 */
	class SceneHeaderHandler : public StandardKeyHandler
	{
	public:
		SceneHeaderHandler(ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */
	};
}

#endif // SCENEHEADERHANDLER_H

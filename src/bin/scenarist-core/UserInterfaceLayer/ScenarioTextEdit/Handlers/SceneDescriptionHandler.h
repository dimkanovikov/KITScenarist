#ifndef SCENEDESCRIPTIONHANDLER_H
#define SCENEDESCRIPTIONHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке описание сцены
	 */
	class SceneDescriptionHandler : public StandardKeyHandler
	{
	public:
		SceneDescriptionHandler(UserInterface::ScenarioTextEdit* _editor);

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

#endif // SCENEDESCRIPTIONHANDLER_H

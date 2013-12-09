#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке описание действия
	 */
	class ActionHandler : public StandardKeyHandler
	{
	public:
		ActionHandler(ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter();
		void handleTab();
		/** @} */
	};
}

#endif // ACTIONHANDLER_H

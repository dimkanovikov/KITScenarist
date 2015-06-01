#ifndef NOPRINTABLETEXTHANDLER_H
#define NOPRINTABLETEXTHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке описание действия
	 */
	class NoprintableTextHandler : public StandardKeyHandler
	{
	public:
		NoprintableTextHandler(UserInterface::ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		/** @} */
	};
}

#endif // NOPRINTABLETEXTHANDLER_H

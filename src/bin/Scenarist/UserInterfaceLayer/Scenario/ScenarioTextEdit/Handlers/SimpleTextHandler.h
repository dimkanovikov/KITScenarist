#ifndef SIMPLETEXTHANDLER_H
#define SIMPLETEXTHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке описание действия
	 */
	class SimpleTextHandler : public StandardKeyHandler
	{
	public:
		SimpleTextHandler(UserInterface::ScenarioTextEdit* _editor);

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

#endif // SIMPLETEXTHANDLER_H

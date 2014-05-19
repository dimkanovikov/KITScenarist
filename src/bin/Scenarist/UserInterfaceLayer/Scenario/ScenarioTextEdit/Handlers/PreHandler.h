#ifndef PREHANDLER_H
#define PREHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий предварительную обработку нажатия клавиш в любом блоке
	 */
	class PreHandler : public StandardKeyHandler
	{
	public:
		PreHandler(UserInterface::ScenarioTextEdit* _editor);

	protected:
		/**
	 * @brief Реализация интерфейса AbstractKeyHandler
	 */
		/** @{ */
		void handleShortcut(QKeyEvent* = 0) {}
		void handleEnter(QKeyEvent* = 0) {}
		void handleTab(QKeyEvent* = 0) {}
		void handleBackspace(QKeyEvent* = 0) {}
		void handleEscape(QKeyEvent* = 0) {}
		void handleUp(QKeyEvent* = 0) {}
		void handleDown(QKeyEvent* = 0) {}

		void handleDelete(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */
	};
}

#endif // PREHANDLER_H

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
		PreHandler(ScenarioTextEdit* _editor);

	protected:
		/**
	 * @brief Реализация интерфейса AbstractKeyHandler
	 */
		/** @{ */
		void handleEnter(QKeyEvent* = 0) {}
		void handleTab(QKeyEvent* = 0) {}
		void handleDelete(QKeyEvent* = 0) {}
		void handleBackspace(QKeyEvent* = 0) {}
		void handleOther(QKeyEvent* _event = 0);
		/** @} */
	};
}

#endif // PREHANDLER_H

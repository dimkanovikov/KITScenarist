#ifndef TITLEHEADERHANDLER_H
#define TITLEHEADERHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке заголовка титра
	 */
	class TitleHeaderHandler : public StandardKeyHandler
	{
	public:
		TitleHeaderHandler(ScenarioTextEdit* _editor) :
			StandardKeyHandler(_editor)
		{}

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter(QKeyEvent* = 0) {}
		void handleTab(QKeyEvent* = 0) {}
		void handleDelete(QKeyEvent* = 0) {}
		void handleBackspace(QKeyEvent* = 0) {}
		/** @} */
	};
}

#endif // TITLEHEADERHANDLER_H

#ifndef FOLDERFOOTERHANDLER_H
#define FOLDERFOOTERHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке завершения папки
	 */
	class FolderFooterHandler : public StandardKeyHandler
	{
	public:
		FolderFooterHandler(ScenarioTextEdit* _editor);

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

#endif // FOLDERFOOTERHANDLER_H

#ifndef STANDARDKEYHANDLER_H
#define STANDARDKEYHANDLER_H

#include "AbstractKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Реализация стандартного обработчика
	 */
	class StandardKeyHandler : public AbstractKeyHandler
	{
	public:
		StandardKeyHandler(ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleShortcut(QKeyEvent* _event = 0);
		void handleDelete();
		void handleBackspace();
		void handleEscape();
		void handleUp(QKeyEvent* _event = 0);
		void handleDown(QKeyEvent* _event = 0);
		void handleOther();
		/** @} */
	};
}

#endif // STANDARDKEYHANDLER_H

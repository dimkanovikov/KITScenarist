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
		void handleDelete(QKeyEvent* _event = 0);
		void handleBackspace(QKeyEvent* _event = 0);
		void handleEscape(QKeyEvent* _event = 0);
		void handleUp(QKeyEvent* _event = 0);
		void handleDown(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */

	private:
		/**
		 * @brief Удалить символы
		 */
		void removeCharacters(bool _backward);
	};
}

#endif // STANDARDKEYHANDLER_H

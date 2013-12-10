#ifndef PREPAREHANDLER_H
#define PREPAREHANDLER_H

#include "AbstractKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс подготавливающий к обработке нажатия клавиш
	 */
	class PrepareHandler : public AbstractKeyHandler
	{
	public:
		PrepareHandler(ScenarioTextEdit* _editor);

		/**
		 * @brief Нужно ли отправлять событие в базовый класс
		 */
		bool needSendEventToBaseClass() const;

	protected:
		/**
		 * @brief Необходимые действия при нажатии конкретной клавиши/сочетания
		 */
		/** @{ */
		void handleShortcut(QKeyEvent* _event = 0);
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		void handleDelete(QKeyEvent* _event = 0);
		void handleBackspace(QKeyEvent* _event = 0);
		void handleEscape(QKeyEvent* _event = 0);
		void handleUp(QKeyEvent* _event = 0);
		void handleDown(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */

	private:
		bool m_needSendEventToBaseClass;
	};
}

#endif // PREPAREHANDLER_H

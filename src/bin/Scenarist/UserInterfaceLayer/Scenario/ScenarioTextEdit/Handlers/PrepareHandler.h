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
		PrepareHandler(UserInterface::ScenarioTextEdit* _editor);

		/**
		 * @brief Нужно ли отправлять событие в базовый класс
		 */
		bool needSendEventToBaseClass() const;

		/**
		 * @brief Нужно ли чтобы курсор был обязательно видим пользователю
		 */
		bool needEnsureCursorVisible() const;

		/**
		 * @brief Изменилась ли структура сценария
		 */
		bool structureChanged() const;

	protected:
		/**
		 * @brief Подготовка к обработке
		 */
		void prepareForHandle();

		/**
		 * @brief Необходимые действия при нажатии конкретной клавиши/сочетания
		 */
		/** @{ */
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		void handleDelete(QKeyEvent* _event = 0);
		void handleBackspace(QKeyEvent* _event = 0);
		void handleEscape(QKeyEvent* _event = 0);
		void handleUp(QKeyEvent* _event = 0);
		void handleDown(QKeyEvent* _event = 0);
		void handlePageUp(QKeyEvent* _event = 0);
		void handlePageDown(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */

	private:

	private:
		bool m_needSendEventToBaseClass;
		bool m_needEnsureCursorVisible;
	};
}

#endif // PREPAREHANDLER_H

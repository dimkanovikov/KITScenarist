#ifndef PARENTHETICALHANDLER_H
#define PARENTHETICALHANDLER_H

#include "StandardKeyHandler.h"

#include <QString>


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке ремарки
	 */
	class ParentheticalHandler : public StandardKeyHandler
	{
	public:
		ParentheticalHandler(ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		/** @} */

	private:
		QString m_stylePrefix;
		QString m_stylePostfix;
	};
}

#endif // PARENTHETICALHANDLER_H

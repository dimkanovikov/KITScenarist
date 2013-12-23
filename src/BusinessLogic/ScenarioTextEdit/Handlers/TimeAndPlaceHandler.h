#ifndef TIMEANDPLACEHANDLER_H
#define TIMEANDPLACEHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке время и место
	 */
	class TimeAndPlaceHandler : public StandardKeyHandler
	{
	public:
		TimeAndPlaceHandler(ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */

	private:
		void storeSceneParameters() const;
	};
}

#endif // TIMEANDPLACEHANDLER_H

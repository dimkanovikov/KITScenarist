#ifndef SCENEGROUPFOOTERHANDLER_H
#define SCENEGROUPFOOTERHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке завершения группы сцен
	 */
	class SceneGroupFooterHandler : public StandardKeyHandler
	{
	public:
		SceneGroupFooterHandler(ScenarioTextEdit* _editor);

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

#endif // SCENEGROUPFOOTERHANDLER_H

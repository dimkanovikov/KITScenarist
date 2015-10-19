#ifndef SCENEGROUPHEADERHANDLER_H
#define SCENEGROUPHEADERHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке заголовка группы сцен
	 */
	class SceneGroupHeaderHandler : public StandardKeyHandler
	{
	public:
		SceneGroupHeaderHandler(UserInterface::ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */
	};
}

#endif // SCENEGROUPHEADERHANDLER_H

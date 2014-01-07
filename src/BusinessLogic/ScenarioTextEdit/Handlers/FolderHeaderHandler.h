#ifndef FOLDERHEADERHANDLER_H
#define FOLDERHEADERHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке заголовка группы сцен
	 */
	class FolderHeaderHandler : public StandardKeyHandler
	{
	public:
		FolderHeaderHandler(ScenarioTextEdit* _editor);

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

#endif // FOLDERHEADERHANDLER_H

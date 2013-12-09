#ifndef CHARACTERHANDLER_H
#define CHARACTERHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке персонажа
	 */
	class CharacterHandler : public StandardKeyHandler
	{
	public:
		CharacterHandler(ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter();
		void handleTab();
		void handleOther();
		/** @} */
	};
}

#endif // CHARACTERHANDLER_H

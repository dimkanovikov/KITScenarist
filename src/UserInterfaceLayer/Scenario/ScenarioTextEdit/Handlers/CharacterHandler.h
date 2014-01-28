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
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */

	private:
		void storeCharacter() const;
	};
}

#endif // CHARACTERHANDLER_H

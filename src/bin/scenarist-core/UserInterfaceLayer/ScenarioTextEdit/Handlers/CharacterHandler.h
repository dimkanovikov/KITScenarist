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
		CharacterHandler(UserInterface::ScenarioTextEdit* _editor);

		/**
		 * @brief При входе в блок, пробуем определить персонажа, который будет говорить
		 */
		void prepareForHandle();

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

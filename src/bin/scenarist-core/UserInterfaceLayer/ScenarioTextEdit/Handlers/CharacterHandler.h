#ifndef CHARACTERHANDLER_H
#define CHARACTERHANDLER_H

#include "StandardKeyHandler.h"

class QStringListModel;


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
		void prehandle();

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

	private:
		/**
		 * @brief Модель персонажей текущей сцены
		 */
		QStringListModel* m_sceneCharactersModel;
	};
}

#endif // CHARACTERHANDLER_H

#ifndef STANDARDKEYHANDLER_H
#define STANDARDKEYHANDLER_H

#include "AbstractKeyHandler.h"

class QString;


namespace KeyProcessingLayer
{
	/**
	 * @brief Реализация стандартного обработчика
	 */
	class StandardKeyHandler : public AbstractKeyHandler
	{
	public:
		StandardKeyHandler(ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleShortcut(QKeyEvent* _event = 0);
		void handleDelete(QKeyEvent* _event = 0);
		void handleBackspace(QKeyEvent* _event = 0);
		void handleEscape(QKeyEvent* _event = 0);
		void handleUp(QKeyEvent* _event = 0);
		void handleDown(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent* _event = 0);
		/** @} */

	private:
		/**
		 * @brief Переместить курсор вверх или вниз
		 */
		void moveCursorUpDown(bool _up, bool _isShiftPressed);

		/**
		 * @brief Удалить символы
		 */
		void removeCharacters(bool _backward);

		/**
		 * @brief Посчитать сколько блоков открывающих/закрывающих группу нужно удалить
		 * @param _isFirstGroupHeader
		 * @param _groupHeadersCount
		 * @param _groupFootersCount
		 */
		void findGroupBlocks(int _startPosition,
							 int _endPosition,
							 int& _groupHeadersCount,
							 int& _groupFootersCount);

		/**
		 * @brief Удалить пары стёртых групп
		 * @param _isFirstGroupHeader
		 * @param _groupHeadersCount
		 * @param _groupFootersCount
		 */
		void removeGroupsPairs(int _groupHeadersCount,
							   int _groupFootersCount);

		/**
		 * @brief Преобразовать текст в значение клавиши
		 */
		int keyCharacterToQtKey(const QString& _keyCharacter) const;
	};
}

#endif // STANDARDKEYHANDLER_H

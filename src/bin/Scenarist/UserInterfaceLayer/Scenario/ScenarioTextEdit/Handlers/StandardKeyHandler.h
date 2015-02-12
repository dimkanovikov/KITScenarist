#ifndef STANDARDKEYHANDLER_H
#define STANDARDKEYHANDLER_H

#include "AbstractKeyHandler.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <QList>

class QString;


namespace KeyProcessingLayer
{
	/**
	 * @brief Реализация стандартного обработчика
	 */
	class StandardKeyHandler : public AbstractKeyHandler
	{
	public:
		StandardKeyHandler(UserInterface::ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Получить стиль блока к которому переходить
		 * TODO: вынести из этого класса, чтобы не работать с настройками из интерфейса
		 */
		/** @{ */
		static BusinessLogic::ScenarioBlockStyle::Type jumpForTab(BusinessLogic::ScenarioBlockStyle::Type _blockType);
		static BusinessLogic::ScenarioBlockStyle::Type jumpForEnter(BusinessLogic::ScenarioBlockStyle::Type _blockType);
		static BusinessLogic::ScenarioBlockStyle::Type changeForTab(BusinessLogic::ScenarioBlockStyle::Type _blockType);
		static BusinessLogic::ScenarioBlockStyle::Type changeForEnter(BusinessLogic::ScenarioBlockStyle::Type _blockType);
		/** @} */

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
		void handlePageUp(QKeyEvent* _event = 0);
		void handlePageDown(QKeyEvent* _event = 0);
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
		 * @brief Найти количество групповых элементов для удаления
		 * @param _topCursorPosition
		 * @param _bottomCursorPosition
		 * @return
		 *
		 * 0 - заголовки групп сцен
		 * 1 - окончания групп сцен
		 * 2 - заголовки папок
		 * 3 - окончания папок
		 */
		QList<int> findGroupCountsToDelete(int _topCursorPosition, int _bottomCursorPosition);

		/**
		 * @brief Удалить пары стёртых групп
		 * @param _isFirstGroupHeader
		 * @param _groupHeadersCount
		 * @param _groupFootersCount
		 */
		void removeGroupsPairs(int _cursorPosition,
							   const QList<int>& _groupCountsToDelete);

		/**
		 * @brief Преобразовать текст в значение клавиши
		 */
		int keyCharacterToQtKey(const QString& _keyCharacter) const;
	};
}

#endif // STANDARDKEYHANDLER_H

#ifndef SCENARIOLINEEDIT_H
#define SCENARIOLINEEDIT_H

#include "ScenarioTextEdit.h"

namespace UserInterface
{
	/**
	 * @brief Редактор строки сценария
	 */
	class ScenarioLineEdit : public ScenarioTextEdit
	{
		Q_OBJECT

	public:
		explicit ScenarioLineEdit(QWidget* _parent = 0);

		/**
		 * @brief Сбросить формат текста
		 */
		void clearCharFormat();

	protected:
		/**
		 * @brief Переопределяется для отправки на обработку выше кнопок Enter и Esc
		 */
		void keyPressEvent(QKeyEvent* _event);

	private slots:
		/**
		 * @brief Убрать переносы строк из текста
		 */
		void removeLineBreaks();
	};
}

#endif // SCENARIOLINEEDIT_H

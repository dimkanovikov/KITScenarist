#ifndef SCENARIOFASTFORMATWIDGET_H
#define SCENARIOFASTFORMATWIDGET_H

#include <QFrame>

namespace UserInterface
{
	class ScenarioTextEdit;


	/**
	 * @brief Виджет быстрого форматирования
	 */
	class ScenarioFastFormatWidget : public QFrame
	{
		Q_OBJECT

	public:
		explicit ScenarioFastFormatWidget(QWidget *parent = 0);

		/**
		 * @brief Установить редактор
		 */
		void setEditor(ScenarioTextEdit* _editor);

		/**
		 * @brief Выделить текущий блок под курсором
		 */
		void selectCurrentBlock();

	private slots:
		/**
		 * @brief Перейти к следующему блоку
		 */
		void goToNextBlock();

		/**
		 * @brief Перейти к предыдущему блоку
		 */
		void goToPrevBlock();

		/**
		 * @brief Сменить стиль блока
		 */
		void changeStyle();

	private:
		/**
		 * @brief Редактор сценария для форматирования блоков
		 */
		ScenarioTextEdit* m_editor;
	};
}

#endif // SCENARIOFASTFORMATWIDGET_H

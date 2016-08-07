#ifndef SCENARIOFASTFORMATWIDGET_H
#define SCENARIOFASTFORMATWIDGET_H

#include <QFrame>

class QPushButton;
class QCheckBox;


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

		/**
		 * @brief Переформировать список быстрых форматов в соответствии с текущим стилем
		 */
		void reinitBlockStyles();

    signals:
        /**
         * @brief Фокус смещён к редактору
         */
        void focusMovedToEditor();

	private slots:
		/**
		 * @brief Перейти к следующему блоку
		 */
		void aboutGoToNextBlock();

		/**
		 * @brief Перейти к предыдущему блоку
		 */
		void aboutGoToPrevBlock();

		/**
		 * @brief Сменить стиль блока
		 */
		void aboutChangeStyle();

		/**
		 * @brief Сменилась позиция курсора
		 */
		void aboutCurrentStyleChanged();

		/**
		 * @brief Сменилось значение отлавливания курсора
		 */
		void aboutGrabCursorChanged(bool _catch);

	private:
		/**
		 * @brief Захватить фокус, если необходимо
		 */
		void catchFocusIfNeeded();

	private:
		/**
		 * @brief Редактор сценария для форматирования блоков
		 */
		ScenarioTextEdit* m_editor;

		/**
		 * @brief Список кнопок
		 */
		QList<QPushButton*> m_buttons;

		/**
		 * @brief Переключатель захвата фокуса
		 */
		QCheckBox* m_grabFocus;
	};
}

#endif // SCENARIOFASTFORMATWIDGET_H

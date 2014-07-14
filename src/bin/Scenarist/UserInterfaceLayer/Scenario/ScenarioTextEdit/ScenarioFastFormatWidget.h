#ifndef SCENARIOFASTFORMATWIDGET_H
#define SCENARIOFASTFORMATWIDGET_H

#include <QFrame>

class QToolButton;

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

	private:
		/**
		 * @brief Редактор сценария для форматирования блоков
		 */
		ScenarioTextEdit* m_editor;

		/**
		 * @brief Список кнопок
		 */
		QList<QToolButton*> m_buttons;
	};
}

#endif // SCENARIOFASTFORMATWIDGET_H

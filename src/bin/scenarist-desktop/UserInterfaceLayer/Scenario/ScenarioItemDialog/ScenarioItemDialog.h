#ifndef SCENARIOITEMDIALOG_H
#define SCENARIOITEMDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

class ColoredToolButton;
class QDialogButtonBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class SimpleTextEditorWidget;

namespace UserInterface
{
	/**
	 * @brief Диалог добавления элемента сценария
	 */
	class ScenarioItemDialog : public QLightBoxDialog
	{
		Q_OBJECT

	public:
		explicit ScenarioItemDialog(QWidget* _parent = 0);

		/**
		 * @brief Восстановить значения по умолчанию
		 */
		void clear();

		/**
		 * @brief Получить тип элемента
		 */
		BusinessLogic::ScenarioBlockStyle::Type itemType() const;

		/**
		 * @brief Получить текст
		 */
		QString header() const;

		/**
		 * @brief Получить цвет
		 */
		QColor color() const;

		/**
		 * @brief Получить описание
		 */
        QString description() const;

    protected:
        /**
         * @brief Переопределяем для фокусировки на виджете вводе заголовка при отображении
         */
        QWidget* focusedOnExec() const override;

	private:
		/**
		 * @brief Настроить представление
		 */
        void initView() override;

		/**
		 * @brief Настроить соединения
		 */
        void initConnections() override;

		/**
		 * @brief Настроить внешний вид диалога
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Переключатели для выбора типа элемента
		 */
		/** @{ */
        QRadioButton* m_folder;
		QRadioButton* m_scene;
		/** @} */

		/**
		 * @brief Редактор заголовка элемента
		 */
        QLineEdit* m_header;

		/**
		 * @brief Цвет сцены
		 */
		ColoredToolButton* m_color;

		/**
		 * @brief Описание сцены
		 */
		SimpleTextEditorWidget* m_description;

		/**
		 * @brief Кнопки диалога
		 */
		QDialogButtonBox* m_buttons;

        /**
         * @brief Кнопка сохранения
         */
        QPushButton* m_saveButton = nullptr;
	};
}

#endif // SCENARIOITEMDIALOG_H

#ifndef SCENARIOITEMDIALOG_H
#define SCENARIOITEMDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

class ColoredToolButton;
class QDialogButtonBox;
class QRadioButton;
class SimpleTextEditor;

namespace UserInterface
{
	class ScenarioLineEdit;


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
		void clearText();

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

	private slots:
		/**
		 * @brief Обновить текущий стиль текста в редакторе
		 */
		void aboutUpdateCurrentTextStyle();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Переключатели для выбора типа элемента
		 */
		/** @{ */
		QRadioButton* m_folder;
		QRadioButton* m_scenesGroup;
		QRadioButton* m_scene;
		/** @} */

		/**
		 * @brief Редактор заголовка элемента
		 */
		ScenarioLineEdit* m_header;

		/**
		 * @brief Цвет сцены
		 */
		ColoredToolButton* m_color;

		/**
		 * @brief Описание сцены
		 */
		SimpleTextEditor* m_description;

		/**
		 * @brief Кнопки диалога
		 */
		QDialogButtonBox* m_buttons;
	};
}

#endif // SCENARIOITEMDIALOG_H

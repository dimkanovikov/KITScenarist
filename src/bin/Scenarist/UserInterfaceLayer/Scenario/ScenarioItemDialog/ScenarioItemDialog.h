#ifndef SCENARIOITEMDIALOG_H
#define SCENARIOITEMDIALOG_H

#include <QDialog>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

class QDialogButtonBox;
class QRadioButton;

namespace UserInterface
{
	class ScenarioLineEdit;


	/**
	 * @brief Диалог добавления элемента сценария
	 */
	class ScenarioItemDialog : public QDialog
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
        QString itemHeader() const;

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
		ScenarioLineEdit* m_itemEditor;

		/**
		 * @brief Кнопки диалога
		 */
		QDialogButtonBox* m_buttons;
	};
}

#endif // SCENARIOITEMDIALOG_H

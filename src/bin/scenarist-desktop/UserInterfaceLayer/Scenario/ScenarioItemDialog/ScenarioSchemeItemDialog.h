#ifndef SCENARIOSCHEMEITEMDIALOG_H
#define SCENARIOSCHEMEITEMDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

#include <BusinessLayer/ScenarioDocument/ScenarioModelItem.h>

namespace Ui {
	class ScenarioSchemeItemDialog;
}


namespace UserInterface
{
	/**
	 * @brief Диалог редактирования элементов схемы
	 */
	class ScenarioSchemeItemDialog : public QLightBoxDialog
	{
		Q_OBJECT

	public:
		explicit ScenarioSchemeItemDialog(QWidget* _parent = 0);
		~ScenarioSchemeItemDialog();

		/**
		 * @brief Восстановить значения по умолчанию
		 */
		void clear();

		/**
		 * @brief Получить тип карточки
		 */
		BusinessLogic::ScenarioModelItem::Type cardType() const;

		/**
		 * @brief Получить заголовок карточки
		 */
		QString cardTitle() const;

		/**
		 * @brief Получить описание карточки
		 */
		QString cardDescription() const;

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид диалога
		 */
		void initStyleSheet();

	private:
		Ui::ScenarioSchemeItemDialog* m_ui;
	};
}

#endif // SCENARIOSCHEMEITEMDIALOG_H

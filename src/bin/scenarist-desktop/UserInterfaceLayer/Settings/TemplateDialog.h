#ifndef TEMPLATEDIALOG_H
#define TEMPLATEDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

class QListWidgetItem;

namespace Ui {
	class TemplateDialog;
}


namespace UserInterface
{
	/**
	 * @brief Диалог редактирования шаблона
	 */
	class TemplateDialog : public QLightBoxDialog
	{
		Q_OBJECT

	public:
		explicit TemplateDialog(QWidget *parent = 0);
		~TemplateDialog();

		/**
		 * @brief Установить шаблон в диалог
		 */
		void setScenarioTemplate(const BusinessLogic::ScenarioTemplate& _template, bool _isNew);

		/**
		 * @brief Получить шаблон настроеный в диалоге
		 */
		BusinessLogic::ScenarioTemplate scenarioTemplate();

	private slots:
		/**
		 * @brief Выбран блок из списка
		 */
		void aboutBlockStyleActivated(QListWidgetItem* _item);

		/**
		 * @brief Изменился тип отступов
		 */
		void aboutSpacingTypeChanged();

		/**
		 * @brief Сменился межстрочный интервал
		 */
		void aboutLineSpacingChanged();

	private:
		/**
		 * @brief Настроить представление
		 */
        void initView() override;

		/**
		 * @brief Настроить соединения
		 */
        void initConnections() override;

	private:
		/**
		 * @brief Интерфейс
		 */
		Ui::TemplateDialog *ui;

		/**
		 * @brief Текущий шаблон
		 */
		BusinessLogic::ScenarioTemplate m_template;

		/**
		 * @brief Текущий стиль блока
		 */
		BusinessLogic::ScenarioBlockStyle m_blockStyle;
	};
}

#endif // TEMPLATEDIALOG_H

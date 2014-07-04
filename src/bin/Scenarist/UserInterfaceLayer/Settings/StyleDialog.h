#ifndef STYLEDIALOG_H
#define STYLEDIALOG_H

#include <QDialog>

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

class QListWidgetItem;

namespace Ui {
	class StyleDialog;
}


namespace UserInterface
{
	/**
	 * @brief Диалог редактирования стиля
	 */
	class StyleDialog : public QDialog
	{
		Q_OBJECT

	public:
		explicit StyleDialog(QWidget *parent = 0);
		~StyleDialog();

		/**
		 * @brief Установить стиль в диалог
		 */
		void setScenarioStyle(const BusinessLogic::ScenarioStyle& _style, bool _isNew);

		/**
		 * @brief Получить стиль настроеный в диалоге
		 */
		BusinessLogic::ScenarioStyle scenarioStyle();

	private slots:
		/**
		 * @brief Выбран блок из списка
		 */
		void aboutBlockStyleActivated(QListWidgetItem* _item);

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
		 * @brief Интерфейс
		 */
		Ui::StyleDialog *ui;

		/**
		 * @brief Текущий стиль
		 */
		BusinessLogic::ScenarioStyle m_style;

		/**
		 * @brief Текущий стиль блока
		 */
		BusinessLogic::ScenarioBlockStyle m_blockStyle;
	};
}

#endif // STYLEDIALOG_H

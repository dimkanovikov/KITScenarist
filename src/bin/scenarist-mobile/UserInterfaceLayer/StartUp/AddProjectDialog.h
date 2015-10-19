#ifndef ADDPROJECTDIALOG_H
#define ADDPROJECTDIALOG_H

#include <QWidget>

namespace Ui {
	class AddProjectDialog;
}


namespace UserInterface
{
	/**
	 * @brief Диалог добавления нового проекта
	 */
	class AddProjectDialog : public QWidget
	{
		Q_OBJECT

	public:
		explicit AddProjectDialog(QWidget *parent = 0);
		~AddProjectDialog();

		/**
		 * @brief Получить название проекта
		 */
		QString projectName() const;

	signals:
		/**
		 * @brief Сигналы нажатия кнопок
		 */
		/** @{ */
		void createClicked();
		void cancelClicked();
		/** @} */

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения для формы
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Интерфейс диалога
		 */
		Ui::AddProjectDialog* m_ui;
	};
}

#endif // ADDPROJECTDIALOG_H

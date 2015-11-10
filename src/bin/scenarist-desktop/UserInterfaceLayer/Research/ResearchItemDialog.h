#ifndef RESEARCHITEMDIALOG_H
#define RESEARCHITEMDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
	class ResearchItemDialog;
}


namespace UserInterface
{
	/**
	 * @brief Диалог добаления элемента разработки
	 */
	class ResearchItemDialog : public QLightBoxDialog
	{
		Q_OBJECT

	public:
		explicit ResearchItemDialog(QWidget* _parent = 0);
		~ResearchItemDialog();

		/**
		 * @brief Очистить диалог
		 */
		void clear();

		void setInsertParent(const QString& _parentName = QString::null);

		/**
		 * @brief Тип разработки
		 */
		int researchType() const;

		/**
		 * @brief Название разработки
		 */
		QString researchName() const;

		/**
		 * @brief Нужно ли встраивать в родительский элемент
		 */
		bool insertResearchInParent() const;

	protected:
		/**
		 * @brief Указываем виджет на который нужно установить фокус при отображении диалога
		 */
		QWidget* focusedOnExec() const;

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
		Ui::ResearchItemDialog* m_ui;
	};
}

#endif // RESEARCHITEMDIALOG_H

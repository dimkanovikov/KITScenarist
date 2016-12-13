#ifndef CRASHREPORTDIALOG_H
#define CRASHREPORTDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
	class CrashReportDialog;
}


namespace UserInterface
{
	/**
	 * @brief Диалог отправки отчёта об ошибке
	 */
	class CrashReportDialog : public QLightBoxDialog
	{
		Q_OBJECT

	public:
		explicit CrashReportDialog(QWidget *parent = 0);
		~CrashReportDialog();

	protected:
		/**
		 * @brief При запуске фокусируемся на поле ввода имени пользователя
		 */
		QWidget* focusedOnExec() const;

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения для формы
		 */
		void initConnections();

	private:
		Ui::CrashReportDialog* m_ui;
	};
}

#endif // CRASHREPORTDIALOG_H

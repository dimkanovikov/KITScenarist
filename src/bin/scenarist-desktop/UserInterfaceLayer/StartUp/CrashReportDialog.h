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

		/**
		 * @brief email отправителя
		 */
		QString email() const;

        /**
         * @brief задать email отправителя
         */
        void setEmail(const QString& _email);

		/**
		 * @brief Сопроводительное сообщение
		 */
		QString message() const;

	protected:
		/**
		 * @brief При запуске фокусируемся на поле ввода имени пользователя
		 */
        QWidget* focusedOnExec() const override;

	private:
		/**
		 * @brief Настроить представление
		 */
        void initView() override;

		/**
		 * @brief Настроить соединения для формы
		 */
        void initConnections() override;

	private:
		Ui::CrashReportDialog* m_ui;
	};
}

#endif // CRASHREPORTDIALOG_H

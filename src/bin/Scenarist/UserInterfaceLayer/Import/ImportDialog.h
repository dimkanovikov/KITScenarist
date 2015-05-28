#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
	class ImportDialog;
}

namespace BusinessLogic {
	class ImportParameters;
}


namespace UserInterface
{
	/**
	 * @brief Диалог импорта
	 */
	class ImportDialog : public QLightBoxDialog
	{
		Q_OBJECT

	public:
		explicit ImportDialog(QWidget *parent = 0);
		~ImportDialog();

		/**
		 * @brief Получить настройки импорта
		 */
		BusinessLogic::ImportParameters importParameters() const;

	private slots:
		/**
		 * @brief Выбрать файл
		 */
		void aboutChooseFile();

		/**
		 * @brief При смене имени файла, обновить доступность кнопки экспорта
		 */
		void aboutFileNameChanged();

	private:
		/**
		 * @brief
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		Ui::ImportDialog *ui;
	};
}

#endif // IMPORTDIALOG_H

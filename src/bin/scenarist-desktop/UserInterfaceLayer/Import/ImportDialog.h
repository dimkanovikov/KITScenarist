#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

class QPushButton;

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
        void initView() override;

		/**
		 * @brief Настроить соединения
		 */
        void initConnections() override;

	private:
		Ui::ImportDialog *ui;

		/**
		 * @brief Кнопка импорта
		 */
		QPushButton* m_import;
	};
}

#endif // IMPORTDIALOG_H

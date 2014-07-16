#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

class QAbstractItemModel;

namespace Ui {
	class ExportDialog;
}


namespace UserInterface
{
	/**
	 * @brief Диалог экспорта
	 */
	class ExportDialog : public QDialog
	{
		Q_OBJECT

	public:
		explicit ExportDialog(QWidget* _parent = 0);
		~ExportDialog();

		/**
		 * @brief Установить модель стилей
		 */
		void setStylesModel(QAbstractItemModel* _model);

		/**
		 * @brief Установить текущий стиль
		 */
		void setCurrentStyle(const QString& _styleName);

		/**
		 * @brief Получить имя экспортируемого файла
		 */
		QString exportFilePath() const;

		/**
		 * @brief Печатать титульную страницу
		 */
		bool printTitle() const;

		/**
		 * @brief Печатать номера страниц
		 */
		bool printPagesNumbering() const;

		/**
		 * @brief Печатать номера сцен
		 */
		bool printScenesNumbering() const;

		/**
		 * @brief Приставка сцен
		 */
		QString scenesPrefix() const;

	signals:
		/**
		 * @brief Сменился стиль
		 */
		void currentStyleChanged(const QString& _styleName);

	private slots:
		/**
		 * @brief Сменился формат
		 */
		void aboutFormatChanged();

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
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		Ui::ExportDialog* ui;
	};
}

#endif // EXPORTDIALOG_H

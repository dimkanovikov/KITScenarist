#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

class QAbstractItemModel;

namespace Ui {
	class ExportDialog;
}

namespace BusinessLogic {
	class ExportParameters;
}


namespace UserInterface
{
	/**
	 * @brief Диалог экспорта
	 */
	class ExportDialog : public QLightBoxDialog
	{
		Q_OBJECT

	public:
		explicit ExportDialog(QWidget* _parent = 0);
		~ExportDialog();

		/**
		 * @brief Установить путь экспортируемого файла
		 */
		void setExportFilePath(const QString& _filePath);

		/**
		 * @brief Установить имя экспортируемого файла
		 */
		void setExportFileName(const QString& _fileName);

		/**
		 * @brief Установить модель стилей
		 */
		void setStylesModel(QAbstractItemModel* _model);

		/**
		 * @brief Установить текущий стиль
		 */
		void setCurrentStyle(const QString& _styleName);

		/**
		 * @brief Установить нумеровать ли страницы
		 */
		void setPageNumbering(bool _isChecked);

		/**
		 * @brief Установить нумеровать ли сцены
		 */
		void setScenesNumbering(bool _isChecked);

		/**
		 * @brief Установить приставку сцен
		 */
		void setScenesPrefix(const QString& _prefix);

		/**
		 * @brief Установить необходимость сохранения редакторских пометок
		 */
		void setSaveReviewMarks(bool _save);

		/**
		 * @brief Установить печатать ли титульный лист
		 */
		void setPrintTitle(bool _isChecked);

		/**
		 * @brief Название сценария
		 */
		/** @{ */
		QString scenarioName() const;
		void setScenarioName(const QString& _name);
		/** @} */

		/**
		 * @brief Дополнительная информация
		 */
		/** @{ */
		QString scenarioAdditionalInfo() const;
		void setScenarioAdditionalInfo(const QString& _additionalInfo);
		/** @} */

		/**
		 * @brief Жанр
		 */
		/** @{ */
		QString scenarioGenre() const;
		void setScenarioGenre(const QString& _genre);
		/** @} */

		/**
		 * @brief Автор
		 */
		/** @{ */
		QString scenarioAuthor() const;
		void setScenarioAuthor(const QString _author);
		/** @} */

		/**
		 * @brief Контактная информация
		 */
		/** @{ */
		QString scenarioContacts() const;
		void setScenarioContacts(const QString& _contacts);
		/** @} */

		/**
		 * @brief Год
		 */
		/** @{ */
		QString scenarioYear() const;
		void setScenarioYear(const QString& _year);
		/** @} */

		/**
		 * @brief Получить настройки экспорта
		 */
		BusinessLogic::ExportParameters exportParameters() const;

	signals:
		/**
		 * @brief Сменился стиль
		 */
		void currentStyleChanged(const QString& _styleName);

		/**
		 * @brief Показать предварительный просмотр
		 */
		void printPreview();

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
		 * @brief При смене имени файла, обновить доступность кнопки экспорта,
		 *		  а также проверить не будет ли произведено пересохранение
		 */
		void aboutFileNameChanged();

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
		Ui::ExportDialog* ui;

		/**
		 * @brief Имя файла для экспорта
		 */
		QString m_exportFileName;
	};
}

#endif // EXPORTDIALOG_H

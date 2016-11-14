#ifndef ADDPROJECTDIALOG_H
#define ADDPROJECTDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
	class AddProjectDialog;
}


namespace UserInterface
{
	/**
	 * @brief Класс диалога авторизации пользователя
	 */
	class AddProjectDialog : public QLightBoxDialog
	{
		Q_OBJECT

	public:
		explicit AddProjectDialog(QWidget* _parent = 0);
		~AddProjectDialog();

		/**
		 * @brief Установить возможность добавления проектов из облака
		 */
		void setIsRemoteAvailable(bool _isAvailable, bool _isEnabled = false);

		/**
		 * @brief Создаётся локальный файл (true) или в облаке (false)
		 */
		bool isLocal() const;

		/**
		 * @brief Название создаваемого проекта
		 */
		QString projectName() const;

		/**
		 * @brief Путь создаваемого файла
		 */
		QString projectFilePath() const;

		/**
		 * @brief Нужно ли импортировать проект
		 */
		bool isNeedImport() const;

		/**
		 * @brief Файл из которого нужно экспоритровать проект
		 */
		QString importFilePath() const;

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
		/**
		 * @brief Интерфейс
		 */
		Ui::AddProjectDialog* m_ui;
	};
}

#endif // ADDPROJECTDIALOG_H

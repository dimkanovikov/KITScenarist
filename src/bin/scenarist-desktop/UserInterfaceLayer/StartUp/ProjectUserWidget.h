#ifndef PROJECTUSERWIDGET_H
#define PROJECTUSERWIDGET_H

#include <QWidget>

namespace Ui {
	class ProjectUserWidget;
}


namespace UserInterface
{
	/**
	 * @brief Представление пользователя, которому открыт доступ к проекту
	 */
	class ProjectUserWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit ProjectUserWidget(QWidget *parent = 0);
		~ProjectUserWidget();

		/**
		 * @brief Установить пользователя
		 */
		void setUserInfo(const QString& _email, const QString& _name, const QString& _role);

		/**
		 * @brief Можно ли удалять подписку для пользователя
		 */
		void setDeletable(bool _isDeletable);

	signals:
		/**
		 * @brief Удалить пользователя с заданным адресом электронной почты
		 */
		void removeUserRequested(const QString& _email);

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
		Ui::ProjectUserWidget* m_ui;
	};
}

#endif // PROJECTUSERWIDGET_H

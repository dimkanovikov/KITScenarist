#ifndef RECENTFILEWIDGET_H
#define RECENTFILEWIDGET_H

#include <QFrame>

class QLabel;
class ElidedLabel;

namespace Ui {
	class ProjectFileWidget;
}

namespace UserInterface
{
	class ProjectUserWidget;


	/**
	 * @brief Виджет для отображения файла в списке недавно открытых
	 */
	class ProjectFileWidget : public QFrame
	{
		Q_OBJECT

	public:
		explicit ProjectFileWidget(QWidget *parent = 0);

		void setProjectName(const QString& _projectName);
		void setFilePath(const QString& _filePath);

		/**
		 * @brief Добавить соавтора
		 */
		void addCollaborator(const QString& _email, const QString& _name, const QString& _role);

		/**
		 * @brief Находится ли мышка над элементом
		 */
		void setMouseHover(bool _hover);

	signals:
		/**
		 * @brief На виджете был произведён клик
		 */
		void clicked();

		/**
		 * @brief Удалить пользователя с заданным адресом электронной почты
		 */
		void removeUser(const QString& _email);

	protected:
		/**
		 * @brief Переопределяем, чтобы изменять внешний вид виджета, в моменты входа/выхода
		 *		  курсора мышки в границы виджета
		 */
		/** @{ */
		void enterEvent(QEvent* _event);
		void leaveEvent(QEvent* _event);
		/** @} */

		/**
		 * @brief Переопределяем, чтобы сигналить о нажатии
		 */
		void mousePressEvent(QMouseEvent* _event);

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

		/**
		 * @brief Настроить стиль
		 */
		void initStylesheet();

	private:
		/**
		 * @brief Интерфейс
		 */
		Ui::ProjectFileWidget* m_ui;

		/**
		 * @brief Название проекта
		 */
		QVector<ProjectUserWidget*> m_users;
	};
}

#endif // RECENTFILEWIDGET_H

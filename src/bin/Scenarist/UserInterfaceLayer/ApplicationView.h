#ifndef APPLICATIONVIEW_H
#define APPLICATIONVIEW_H

#include <QWidget>


namespace UserInterface
{
	/**
	 * @brief Главное окно приложения
	 */
	class ApplicationView : public QWidget
	{
		Q_OBJECT
	public:
		explicit ApplicationView(QWidget* _parent = 0);

	signals:
		/**
		 * @brief Пользователь хочет закрыть приложение
		 */
		void wantToClose();

	protected:
		/**
		 * @brief Переопределяется для отлавливания события закрытия приложения
		 */
		void closeEvent(QCloseEvent* _event);
	};
}

#endif // APPLICATIONVIEW_H

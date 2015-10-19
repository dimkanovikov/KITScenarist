#ifndef APPLICATIONVIEW_H
#define APPLICATIONVIEW_H

#include <QWidget>
#include <QMap>

namespace Ui {
	class ApplicationView;
}


namespace UserInterface
{
	/**
	 * @brief Основная форма приложения
	 */
	class ApplicationView : public QWidget
	{
		Q_OBJECT

	public:
		explicit ApplicationView(QWidget *parent = 0);
		~ApplicationView();

		/**
		 * @brief Добавить представление в окно
		 */
		void addView(QWidget* _toolbar, QWidget* _view);

		/**
		 * @brief Установить текущее представление
		 */
		void setCurrentView(int _index);

	signals:
		/**
		 * @brief Нажата кнопка меню
		 */
		void menuClicked();

		/**
		 * @brief Пользователь хочет закрыть приложение
		 */
		void wantToClose();

	protected:
		/**
		 * @brief Переопределяется для отлавливания события закрытия приложения
		 */
		void closeEvent(QCloseEvent* _event);

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
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Интерфейс представления
		 */
		Ui::ApplicationView* m_ui;
	};
}


#endif // APPLICATIONVIEW_H

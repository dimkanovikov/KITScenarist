#ifndef CABINVIEW_H
#define CABINVIEW_H

#include <QWidget>

namespace Ui {
	class CabinView;
}


namespace UserInterface
{
	/**
	 * @brief Страница личного кабинета пользователя
	 */
	class CabinView : public QWidget
	{
		Q_OBJECT

	public:
		explicit CabinView(QWidget *parent = 0);
		~CabinView();

		/**
		 * @brief Получить панель инструментов
		 */
		QWidget* toolbar();

	signals:
		/**
		 * @brief Нажата кнопка разлогиниться
		 */
		void logoutRequested();

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
		 * @brief Форма
		 */
		Ui::CabinView* m_ui;
	};
}

#endif // CABINVIEW_H

#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>

namespace UserInterface
{
	/**
	 * @brief Класс формы со статистикой по сценарию
	 */
	class StatisticsView : public QWidget
	{
		Q_OBJECT

	public:
		explicit StatisticsView(QWidget* _parent = 0);

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения для формы
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();
	};
}

#endif // STATISTICSVIEW_H

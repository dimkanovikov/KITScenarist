#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QtWidgets//QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QIcon>
#include <QAction>

class SideTabBar : public QWidget
{
	Q_OBJECT

public:
	SideTabBar(QWidget *parent = 0);

	void addTab(QAction* action);
	QAction* addTab(const QString& text, const QIcon& icon = QIcon());

	/**
	 * @brief Установить текущую вкладку
	 */
	void setCurrentTab(int _index);

	/**
	 * @brief Получить индекс текущей вкладки
	 */
	int currentTab() const;

	/**
	 * @brief Получить индекс предыдущей активной вкладки
	 */
	int prevCurrentTab() const;

	QList<QAction*> tabs() const;

	/**
	 * @brief Добавить индикатор вниз панели
	 * @note Вызов с пустыми параметрами, равен вызову функции removeIndicator
	 */
	void addIndicator(const QIcon& _icon, const QString& _title = QString::null, const QString& _message = QString::null);

	/**
	 * @brief Убрать индикатор из панели
	 */
	void removeIndicator();

	/**
	 * @brief Установить компактный режим (по умолчанию установлен расширенный режим)
	 */
	void setCompactMode(bool _compact);

signals:
	/**
	 * @brief Изменилась текущая вкладка
	 */
	void currentChanged(int index);

protected:
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* _event);
	void mouseReleaseEvent(QMouseEvent* _event);
	QSize minimumSizeHint() const;

private:
	QAction* tabAt(const QPoint& at);

private:
	QList<QAction*> m_tabs;
	QAction* m_pressedTab;
	QAction* m_checkedTab;

	/**
	 * @brief Флаг компактного режима
	 */
	bool m_compactMode;

	/**
	 * @brief Индекс текущей вкладки
	 */
	int m_currentIndex;

	/**
	 * @brief Индекс предыдущей активной вкладки
	 */
	int m_prevCurrentIndex;

	/**
	 * @brief Индикатор внизу панели
	 */
	QAction* m_indicator;
};

#endif // SIDEBAR_H

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

	/**
	 * @brief Добавить вкладку
	 */
	/** @{ */
	void addTab(QAction* action);
	QAction* addTab(const QString& text, const QIcon& icon = QIcon());
	/** @} */

	/**
	 * @brief Получить вкладку по заданному индексу
	 */
	QAction* tab(int _index) const;

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

	/**
	 * @brief Добавить индикатор вниз панели
	 * @note Вызов с пустыми параметрами удаляет индикатор
	 */
	void addIndicator(const QIcon& _icon, const QString& _title = QString::null, const QString& _message = QString::null);

	/**
	 * @brief Установить дополнительную информацию для отображения в индикаторе
	 */
	void setIndicatorAdditionalInfo(const QString& _info);

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
	void currentChanged(int _index);

protected:
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* _event);
	void mouseReleaseEvent(QMouseEvent* _event);
	QSize minimumSizeHint() const;

private:
	/**
	 * @brief Определить вкладку в заданной координате
	 */
	QAction* tabAt(const QPoint& _pos);

private:
	/**
	 * @brief Список вкладок
	 */
	QList<QAction*> m_tabs;

	/**
	 * @brief Вкладка на которой был выполнен клик мышкой
	 * @note Используется для внутренних нужд
	 */
	QAction* m_pressedTab;

	/**
	 * @brief Текущая активная вкладка
	 */
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

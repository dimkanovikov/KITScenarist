#ifndef PAGESTEXTEDIT_H
#define PAGESTEXTEDIT_H

#include <QTextEdit>

#include "PageMetrics.h"


class PagesTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit PagesTextEdit(QWidget* parent = 0);

	/**
	 * @brief Установить формат страницы
	 */
	void setPageFormat(QPageSize::PageSizeId _pageFormat);

	/**
	 * @brief Настроить поля страницы
	 */
	void setPageMargins(const QMarginsF& _margins);

	/**
	 * @brief Получить режим отображения текста
	 */
	bool usePageMode() const;

	/**
	 * @brief Установить коэффициент масштабирования
	 */
	void setZoomRange(int _zoomRange);

public slots:
	/**
	 * @brief Установить режим отображения текста
	 */
	void setUsePageMode(bool _use);

signals:
	/**
	 * @brief Изменился коэффициент масштабирования
	 */
	void zoomRangeChanged(int);

protected:
	/**
	 * @brief Переопределяется для корректировки документа и прорисовки оформления страниц
	 */
	void paintEvent(QPaintEvent* _event);

	/**
	 * @brief Сбросить масштабирование
	 */
	void resetZoom();

	/**
	 * @brief Переопределяется для реализации увеличения/уменьшения текста
	 */
	void wheelEvent(QWheelEvent* _event);

private:
	/**
	 * @brief Обновить внутреннюю геометрию
	 */
	void updateInnerGeometry();

	/**
	 * @brief Обновить интервал вертикальной прокрутки
	 */
	void updateVerticalScrollRange();

	/**
	 * @brief Нарисовать оформление страниц документа
	 */
	void paintPagesView();

	/**
	 * @brief Собственные реализации методов масштабирования содержимого
	 */
	/** @{ */
	void privateZoomIn(qreal _range);
	void privateZoomOut(qreal _range);
	/** @} */

private:
	/**
	 * @brief Режим отображения текста
	 *
	 * true - постраничный
	 * false - сплошной
	 */
	bool m_usePageMode;

	/**
	 * @brief Метрика страницы редактора
	 */
	PageMetrics m_pageMetrics;

	/**
	 * @brief Текущий коэффициент масштабирования
	 */
	int m_zoomRange;
};

#endif // PAGESTEXTEDIT_H

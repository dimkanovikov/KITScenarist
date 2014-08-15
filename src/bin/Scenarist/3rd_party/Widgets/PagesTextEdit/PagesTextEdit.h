#ifndef PAGESTEXTEDIT_H
#define PAGESTEXTEDIT_H

#include <QTextEdit>

#include "PageMetrics.h"


/**
 * @brief Редактор текста, с возможностью постраничного отображения содержимого
 */
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

	/**
	 * @brief Установить значение необходимости добавления дополнительной прокрутки снизу
	 */
	void setAddSpaceToBottom(bool _addSpace);

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
	 * @note Если позиции равны нулю, то обрабатывается весь документ
	 */
	/** @{ */
	void privateZoomIn(qreal _range, int _startPosition = 0, int _endPosition = 0);
	void privateZoomOut(qreal _range, int _startPosition = 0, int _endPosition = 0);
	/** @} */

private slots:
	/**
	 * @brief Скорректировать обработку изменения текста, для установки масштаба,
	 * если был изменён документ
	 */
	void aboutUpdateZoomRangeHandling();

	/**
	 * @brief Маштабировать изменения документа
	 */
	void aboutUpdateZoomRange(int _position, int _charsRemoved, int _charsAdded);

	/**
	 * @brief Изменился интервал вертикальной прокрутки
	 */
	void aboutVerticalScrollRangeChanged(int _minimum, int _maximum);

private:
	/**
	 * @brief Режим отображения текста
	 *
	 * true - постраничный
	 * false - сплошной
	 */
	bool m_usePageMode;

	/**
	 * @brief Необходимо ли добавлять пространство снизу в обычном режиме
	 */
	bool m_addBottomSpace;

	/**
	 * @brief Метрика страницы редактора
	 */
	PageMetrics m_pageMetrics;

	/**
	 * @brief Флаг обозначающий, что редактор находится в процессе масштабирования
	 */
	bool m_inZoomHandling;

	/**
	 * @brief Текущий коэффициент масштабирования
	 */
	int m_zoomRange;

	/**
	 * @brief Указатель на документ
	 */
	QTextDocument* m_document;
};

#endif // PAGESTEXTEDIT_H

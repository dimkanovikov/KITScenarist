#ifndef PAGESTEXTEDIT_H
#define PAGESTEXTEDIT_H

#include <QScrollBar>


/**
 * @brief Реализация полосы прокрутки, которая не "скачет" из-за постоянных смены максимума
 */
class ScrollBar : public QScrollBar
{
	Q_OBJECT

public:
	ScrollBar(Qt::Orientation _orientation, QWidget* _parent = 0);

	void setMaximumReimpl(int _maximum);

protected:
	void sliderChange(SliderChange _change);

private:
	int m_maximum;
};


// ****


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
	 * @brief Получить номер страницы курсора
	 */
	int cursorPage(const QTextCursor& _cursor);

public slots:
	/**
	 * @brief Установить режим отображения текста
	 */
	void setUsePageMode(bool _use);

	/**
	 * @brief Установить значение необходимости добавления дополнительной прокрутки снизу
	 */
	void setAddSpaceToBottom(bool _addSpace);

	/**
	 * @brief Установить значение необходимости отображения номеров страниц
	 */
	void setShowPageNumbers(bool _show);

	/**
	 * @brief Установить место отображения номеров страниц
	 */
	void setPageNumbersAlignment(Qt::Alignment _align);

	/**
	 * @brief Установить водяной знак
	 */
	void setWatermark(const QString& _watermark);

	/**
	 * @brief Своя реализация проверки виден ли курсор на экране
	 * @param Значение true докручивает, как сверху, так и снизу, а false только снизу
	 */
	void ensureCursorVisibleReimpl(bool _upAndDown = false);

protected:
	/**
	 * @brief Переопределяется для корректировки документа и прорисовки оформления страниц
	 */
	void paintEvent(QPaintEvent* _event);

	/**
	 * @brief Переопределяется, для корректного обновления размеров вьювпорта
	 */
	void resizeEvent(QResizeEvent* _event);

private:
	/**
	 * @brief Обновить отступы вьювпорта
	 */
	void updateViewportMargins();

	/**
	 * @brief Обновить интервал вертикальной прокрутки
	 */
	void updateVerticalScrollRange();

	/**
	 * @brief Нарисовать оформление страниц документа
	 */
	void paintPagesView();

	/**
	 * @brief Нарисовать номера страниц
	 */
	void paintPageNumbers();

	/**
	 * @brief Нарисовать номер страницы с заданными парамтерами расположения
	 */
	void paintPageNumber(QPainter* _painter, const QRectF& _rect, bool _isHeader, int _number);

	/**
	 * @brief Нарисовать водяной знак
	 */
	void paintWatermark();

private slots:
	/**
	 * @brief Изменился интервал вертикальной прокрутки
	 */
	void aboutVerticalScrollRangeChanged(int _minimum, int _maximum);

	/**
	 * @brief Проверка смены документа
	 */
	void aboutDocumentChanged();

	/**
	 * @brief Обновить геометрию документа
	 * @note Стандартная реализация QTextEdit такова, что она всё время сбрасывает установленный
	 *		 размер документа, что приводит к нежелательным последствиям
	 */
	void aboutUpdateDocumentGeometry();

private:
	/**
	 * @brief Ссылка на текущий документ
	 */
	QTextDocument* m_document;

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
	 * @brief Необходимо ли показывать номера страниц
	 */
	bool m_showPageNumbers;

	/**
	 * @brief Где показывать номера страниц
	 */
	Qt::Alignment m_pageNumbersAlignment;

	/**
	 * @brief Метрика страницы редактора
	 */
	PageMetrics m_pageMetrics;

	/**
	 * @brief Водяной знак
	 */
	QString m_watermark;
	QString m_watermarkMulti;
};

#endif // PAGESTEXTEDIT_H

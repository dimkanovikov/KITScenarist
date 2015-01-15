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
	 * @brief Нарисовать номера страниц
	 */
	void paintPageNumbers();

	/**
	 * @brief Нарисовать номер страницы с заданными парамтерами расположения
	 */
	void paintPageNumber(QPainter* _painter, const QRectF& _rect, bool _isHeader, int _number);

private slots:
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
};

#endif // PAGESTEXTEDIT_H

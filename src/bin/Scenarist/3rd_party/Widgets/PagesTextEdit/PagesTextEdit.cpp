#include "PagesTextEdit.h"

#include <QPainter>
#include <QScrollBar>
#include <QTextFrame>


PagesTextEdit::PagesTextEdit(QWidget *parent) :
	QTextEdit(parent),
	m_usePageMode(false),
	m_addBottomSpace(true),
	m_showPageNumbers(true),
	m_pageNumbersAlignment(Qt::AlignTop | Qt::AlignRight)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	//
	// Ручная настройка интервала прокрутки
	//
	connect(verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
		this, SLOT(aboutVerticalScrollRangeChanged(int,int)));
}

void PagesTextEdit::setPageFormat(QPageSize::PageSizeId _pageFormat)
{
	m_pageMetrics.update(_pageFormat);

	//
	// Перерисуем себя
	//
	repaint();
}

void PagesTextEdit::setPageMargins(const QMarginsF& _margins)
{
	m_pageMetrics.update(m_pageMetrics.pageFormat(), _margins);

	//
	// Перерисуем себя
	//
	repaint();
}

bool PagesTextEdit::usePageMode() const
{
	return m_usePageMode;
}

void PagesTextEdit::setUsePageMode(bool _use)
{
	if (m_usePageMode != _use) {
		m_usePageMode = _use;

		//
		// Перерисуем себя
		//
		repaint();
	}
}

void PagesTextEdit::setAddSpaceToBottom(bool _addSpace)
{
	if (m_addBottomSpace != _addSpace) {
		m_addBottomSpace = _addSpace;

		//
		// Перерисуем себя
		//
		repaint();
	}
}

void PagesTextEdit::setShowPageNumbers(bool _show)
{
	if (m_showPageNumbers != _show) {
		m_showPageNumbers = _show;

		//
		// Перерисуем себя
		//
		repaint();
	}
}

void PagesTextEdit::setPageNumbersAlignment(Qt::Alignment _align)
{
	if (m_pageNumbersAlignment != _align) {
		m_pageNumbersAlignment = _align;

		//
		// Перерисуем себя
		//
		repaint();
	}
}

void PagesTextEdit::paintEvent(QPaintEvent* _event)
{
	updateInnerGeometry();

	updateVerticalScrollRange();

	paintPagesView();

	paintPageNumbers();

	QTextEdit::paintEvent(_event);
}

void PagesTextEdit::resizeEvent(QResizeEvent* _event)
{
	updateInnerGeometry();

	updateVerticalScrollRange();

	QTextEdit::resizeEvent(_event);
}

void PagesTextEdit::updateInnerGeometry()
{
	//
	// Формируем параметры отображения
	//
	QSizeF documentSize(width() - verticalScrollBar()->width(), -1);
	QMargins viewportMargins;
	QMarginsF rootFrameMargins = m_pageMetrics.pxPageMargins();

	if (m_usePageMode) {
		//
		// Настроить размер документа
		//

		int pageWidth = m_pageMetrics.pxPageSize().width();
		int pageHeight = m_pageMetrics.pxPageSize().height();
		documentSize = QSizeF(pageWidth, pageHeight);

		//
		// Рассчитываем отступы для viewport
		//
		const int DEFAULT_TOP_MARGIN = 20;
		const int DEFAULT_BOTTOM_MARGIN = 20;
		if (width() > pageWidth) {
			//
			// Нижний оступ может быть больше минимального значения, для случая,
			// когда весь документ и даже больше помещается на экране
			//
			int bottomViewportMargin = DEFAULT_TOP_MARGIN;
			int documentHeight = pageHeight * document()->pageCount();
			if ((height() - documentHeight) > (DEFAULT_TOP_MARGIN + DEFAULT_BOTTOM_MARGIN)) {
				bottomViewportMargin = height() - documentHeight - DEFAULT_TOP_MARGIN;
			}

			viewportMargins =
					QMargins(
						(width() - pageWidth - verticalScrollBar()->width() - 2)/2,
						DEFAULT_TOP_MARGIN,
						(width() - pageWidth - verticalScrollBar()->width() - 2)/2,
						bottomViewportMargin);
		} else {
			viewportMargins = QMargins(0, DEFAULT_TOP_MARGIN, 0, DEFAULT_BOTTOM_MARGIN);
		}
	}

	//
	// Применяем параметры отображения
	//

	if (document()->documentMargin() != 0) {
		document()->setDocumentMargin(0);
	}

	if (document()->pageSize() != documentSize) {
		document()->setPageSize(documentSize);
	}

	setViewportMargins(viewportMargins);

	QTextFrameFormat rootFrameFormat = document()->rootFrame()->frameFormat();
	if (rootFrameFormat.leftMargin() != rootFrameMargins.left()
		|| rootFrameFormat.topMargin() != rootFrameMargins.top()
		|| rootFrameFormat.rightMargin() != rootFrameMargins.right()
		|| rootFrameFormat.bottomMargin() != rootFrameMargins.bottom()) {
		rootFrameFormat.setLeftMargin(rootFrameMargins.left());
		rootFrameFormat.setTopMargin(rootFrameMargins.top());
		rootFrameFormat.setRightMargin(rootFrameMargins.right());
		rootFrameFormat.setBottomMargin(rootFrameMargins.bottom());
		document()->rootFrame()->setFrameFormat(rootFrameFormat);
	}
}

void PagesTextEdit::updateVerticalScrollRange()
{
	//
	// В постраничном режиме показываем страницу целиком
	//
	if (m_usePageMode) {
		int maximumValue = m_pageMetrics.pxPageSize().height() * document()->pageCount() - viewport()->size().height();
		if (verticalScrollBar()->maximum() != maximumValue) {
			verticalScrollBar()->setMaximum(maximumValue);
		}
	}
	//
	// В обычном режиме просто добавляем немного дополнительной прокрутки для удобства
	//
	else {
		const int SCROLL_DELTA = 800;
		int maximumValue =
				document()->size().height() - viewport()->size().height()
				+ (m_addBottomSpace ? SCROLL_DELTA : 0);
		if (verticalScrollBar()->maximum() != maximumValue) {
			verticalScrollBar()->setMaximum(maximumValue);
		}
	}
}

void PagesTextEdit::paintPagesView()
{
	//
	// Оформление рисуется только тогда, когда редактор находится в постраничном режиме
	//
	if (m_usePageMode) {
		//
		// Нарисовать линии разрыва страниц
		//

		qreal pageWidth = m_pageMetrics.pxPageSize().width();
		qreal pageHeight = m_pageMetrics.pxPageSize().height();

		QPainter p(viewport());
		QPen spacePen(palette().window(), 9);
		QPen borderPen(palette().dark(), 1);

		qreal curHeight = pageHeight - (verticalScrollBar()->value() % (int)pageHeight);
		//
		// Необходимо ли рисовать верхнуюю границу следующей страницы
		//
		bool canDrawNextPageLine = verticalScrollBar()->value() != verticalScrollBar()->maximum();
		//
		// Корректируем позицию правой границы
		//
		int x = pageWidth - (width() % 2 == 0 ? 0 : 1);

		//
		// Нарисовать верхнюю границу
		//
		if (curHeight - pageHeight >= 0) {
			p.setPen(borderPen);
			// ... верхняя
			p.drawLine(0, curHeight - pageHeight, pageWidth, curHeight - pageHeight);
		}

		while (curHeight <= height()) {
			//
			// Фон разрыва страниц
			//
			p.setPen(spacePen);
			p.drawLine(0, curHeight-4, width(), curHeight-4);

			//
			// Границы страницы
			//
			p.setPen(borderPen);
			// ... нижняя
			p.drawLine(0, curHeight-8, pageWidth, curHeight-8);
			// ... верхняя следующей страницы
			if (canDrawNextPageLine) {
				p.drawLine(0, curHeight, pageWidth, curHeight);
			}
			// ... левая
			p.drawLine(0, curHeight-pageHeight, 0, curHeight-8);
			// ... правая
			p.drawLine(x, curHeight-pageHeight, x, curHeight-8);

			curHeight += pageHeight;
		}

		//
		// Нарисовать боковые границы страницы, когда страница не влезает в экран
		//
		if (curHeight >= height()) {
			//
			// Границы страницы
			//
			p.setPen(borderPen);
			// ... левая
			p.drawLine(0, curHeight-pageHeight, 0, height());
			// ... правая
			p.drawLine(x, curHeight-pageHeight, x, height());
		}
	}
}

void PagesTextEdit::paintPageNumbers()
{
	//
	// Номера страниц рисуются только тогда, когда редактор находится в постраничном режиме,
	// если заданы поля и включена опция отображения номеров
	//
	if (m_usePageMode && !m_pageMetrics.pxPageMargins().isNull() && m_showPageNumbers) {
		//
		// Нарисовать номера страниц
		//

		QSizeF pageSize(m_pageMetrics.pxPageSize());
		QMarginsF pageMargins(m_pageMetrics.pxPageMargins());

		QPainter p(viewport());
		p.setFont(document()->defaultFont());
		p.setPen(QPen(palette().text(), 1));

		//
		// Текущие высота и ширина которые отображаются на экране
		//
		qreal curHeight = pageSize.height() - (verticalScrollBar()->value() % (int)pageSize.height());

		//
		// Начало поля должно учитывать смещение полосы прокрутки
		//
		qreal leftMarginPosition = pageMargins.left() - horizontalScrollBar()->value();
		//
		// Итоговая ширина поля
		//
		qreal marginWidth = pageSize.width() - pageMargins.left() - pageMargins.right();

		//
		// Необходимо ли рисовать верхнуюю границу следующей страницы
		//
		bool canDrawNextPageTop = verticalScrollBar()->value() != verticalScrollBar()->maximum();
		//
		// Номер первой видимой на экране страницы
		//
		int pageNumber = verticalScrollBar()->value() / pageSize.height() + 1;

		//
		// Верхнее поле первой страницы на экране, когда не видно предыдущей страницы
		//
		if (curHeight - pageMargins.top() >= 0) {
			QRectF topMarginRect(leftMarginPosition, curHeight - pageSize.height(), marginWidth, pageMargins.top());
			paintPageNumber(&p, topMarginRect, true, pageNumber);
		}

		//
		// Для всех видимых страниц
		//
		while (curHeight < height()) {
			//
			// Определить прямоугольник нижнего поля
			//
			QRect bottomMarginRect(leftMarginPosition, curHeight - pageMargins.bottom(), marginWidth, pageMargins.bottom());
			paintPageNumber(&p, bottomMarginRect, false, pageNumber);

			//
			// Переход к следующей странице
			//
			++pageNumber;

			//
			// Определить прямоугольник верхнего поля следующей страницы
			//
			if (canDrawNextPageTop) {
				QRect topMarginRect(leftMarginPosition, curHeight, marginWidth, pageMargins.top());
				paintPageNumber(&p, topMarginRect, true, pageNumber);
			}

			curHeight += pageSize.height();
		}
	}
}

void PagesTextEdit::paintPageNumber(QPainter* _painter, const QRectF& _rect, bool _isHeader, int _number)
{
	//
	// Верхнее поле
	//
	if (_isHeader) {
		//
		// Если нумерация рисуется в верхнем поле
		//
		if (m_pageNumbersAlignment.testFlag(Qt::AlignTop)) {
			_painter->drawText(_rect, Qt::AlignVCenter | (m_pageNumbersAlignment ^ Qt::AlignTop),
				QString::number(_number));
		}
	}
	//
	// Нижнее поле
	//
	else {
		//
		// Если нумерация рисуется в нижнем поле
		//
		if (m_pageNumbersAlignment.testFlag(Qt::AlignBottom)) {
			_painter->drawText(_rect, Qt::AlignVCenter | (m_pageNumbersAlignment ^ Qt::AlignBottom),
				QString::number(_number));
		}
	}
}

void PagesTextEdit::aboutVerticalScrollRangeChanged(int _minimum, int _maximum)
{
	Q_UNUSED(_minimum);

	int scrollValue = verticalScrollBar()->value();

	//
	// Если текущая позиция прокрутки больше максимального значения,
	// значит текстэдит сам обновил интервал, применяем собственную функцию коррекции
	//
	if (scrollValue > _maximum) {
		updateVerticalScrollRange();
	}
}


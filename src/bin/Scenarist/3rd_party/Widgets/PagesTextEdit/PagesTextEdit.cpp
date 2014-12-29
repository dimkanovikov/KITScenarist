#include "PagesTextEdit.h"

#include <QPainter>
#include <QScrollBar>
#include <QTextFrame>


PagesTextEdit::PagesTextEdit(QWidget *parent) :
	QTextEdit(parent),
	m_usePageMode(false),
	m_addBottomSpace(true),
	m_document(0)
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

void PagesTextEdit::paintEvent(QPaintEvent* _event)
{
	updateInnerGeometry();

	updateVerticalScrollRange();

	paintPagesView();

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
		if (width() > pageWidth) {
			viewportMargins =
					QMargins(
						(width() - pageWidth - verticalScrollBar()->width() - 2)/2,
						20,
						(width() - pageWidth - verticalScrollBar()->width() - 2)/2,
						20);
		} else {
			viewportMargins = QMargins(0, 20, 0, 20);
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
		int x = pageWidth - (width() % 2 == 0 ? 1 : 0);

		//
		// Нарисовать верхнюю границу
		//
		if (curHeight - pageHeight >= 0) {
			p.setPen(borderPen);
			// ... верхняя
			p.drawLine(0, curHeight - pageHeight, pageWidth, curHeight - pageHeight);
		}

		while (curHeight < height()) {
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


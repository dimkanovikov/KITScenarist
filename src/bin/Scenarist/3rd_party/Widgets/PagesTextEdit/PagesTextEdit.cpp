#include "PagesTextEdit.h"

#include <QGestureEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QTextFrame>
#include <QTextFrameFormat>


namespace {
	/**
	 * @brief Минимальный размер коэффициэнта масштабирования
	 */
	const int MINIMUM_ZOOM_RANGE = -6;

	/**
	 * @brief Идентификатор свойства масштабирования для блока
	 */
	const int ZOOM_PROPERTY = QTextFormat::UserProperty + 1;

	/**
	 * @brief Определить коэффициент масштабирования
	 */
	static qreal zoomRange(int _range)
	{
		return (qreal)(_range + 10) / 10;
	}

	/**
	 * @brief Перевести пиксели в поинты
	 */
	static qreal pixelsToPoints(const QPaintDevice* _device, qreal _pixels)
	{
		return _pixels * (qreal)72 / (qreal)_device->logicalDpiY();
	}

	/**
	 * @brief Вычислить масштабированное значение
	 */
	static qreal scale(qreal _source, qreal _scaleRange)
	{
		qreal result = _source;
		if (_scaleRange > 0) {
			result = _source * _scaleRange;
		} else if (_scaleRange < 0) {
			result = _source / _scaleRange * -1;
		} else {
			Q_ASSERT_X(0, "scale", "scale to zero range");
		}
		return result;
	}
}


PagesTextEdit::PagesTextEdit(QWidget *parent) :
	QTextEdit(parent),
	m_usePageMode(false),
	m_addBottomSpace(true),
	m_inZoomHandling(false),
	m_zoomRange(0),
	m_gestureZoomInertionBreak(0),
	m_document(0)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	//
	// Отслеживаем масштабирование при помощи жеста
	//
	grabGesture(Qt::PinchGesture);

	//
	// Отслеживаем потенциальное изменение документа
	//
	connect(this, SIGNAL(textChanged()), this, SLOT(aboutUpdateZoomRangeHandling()));
	aboutUpdateZoomRangeHandling();

	connect(verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(aboutVerticalScrollRangeChanged(int,int)));
}

bool PagesTextEdit::usePageMode() const
{
	return m_usePageMode;
}

void PagesTextEdit::setZoomRange(int _zoomRange)
{
	m_inZoomHandling = true;

	//
	// Возможно лишь двукратное уменьшение масштаба
	//
	if (_zoomRange < MINIMUM_ZOOM_RANGE) return;

	//
	// Отменяем предыдущее мастштабирование
	//
	qreal zoomRange = ::zoomRange(m_zoomRange);
	privateZoomOut(zoomRange);

	//
	// Обновляем коэффициент
	//
	m_zoomRange = _zoomRange;
	zoomRange = ::zoomRange(m_zoomRange);

	//
	// Масштабируем с новым коэффициентом
	//
	{
		//
		// ... шрифт
		//
		privateZoomIn(zoomRange);

		//
		// ... документ
		//
		m_pageMetrics.zoomIn(zoomRange);
	}

	//
	// Уведомляем о том, что коэффициент изменился
	//
	emit zoomRangeChanged(m_zoomRange);

	m_inZoomHandling = false;
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

bool PagesTextEdit::event(QEvent* _event)
{
	bool result = true;
	if (_event->type() == QEvent::Gesture) {
		gestureEvent(static_cast<QGestureEvent*>(_event));
	} else {
		result = QTextEdit::event(_event);
	}

	return result;
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

void PagesTextEdit::paintEvent(QPaintEvent* _event)
{
	updateInnerGeometry();

	updateVerticalScrollRange();

	paintPagesView();

	QTextEdit::paintEvent(_event);
}

void PagesTextEdit::wheelEvent(QWheelEvent* _event)
{
	if (_event->modifiers() & Qt::ControlModifier) {
		if (_event->orientation() == Qt::Vertical) {
			//
			// zoomRange > 0 - Текст увеличивается
			// zoomRange < 0 - Текст уменьшается
			//
			int zoomRange = m_zoomRange + (_event->angleDelta().y() / 120);
			setZoomRange(zoomRange);

			_event->accept();
		}
	} else {
		QTextEdit::wheelEvent(_event);
	}
}

void PagesTextEdit::gestureEvent(QGestureEvent* _event)
{
	if (QGesture* gesture = _event->gesture(Qt::PinchGesture)) {
		if (QPinchGesture* pinch = qobject_cast<QPinchGesture *>(gesture)) {
			//
			// При масштабировании за счёт жестов приходится немного притормаживать
			// т.к. события приходят слишком часто и при обработке каждого события
			// пользователю просто невозможно корректно настроить масштаб
			//

			int zoomRange = m_zoomRange;
			if (pinch->scaleFactor() > 1) {
				if (m_gestureZoomInertionBreak < 0) {
					m_gestureZoomInertionBreak = 0;
				} else if (m_gestureZoomInertionBreak >= 8) {
					m_gestureZoomInertionBreak = 0;
					++zoomRange;
				} else {
					++m_gestureZoomInertionBreak;
				}
			} else if (pinch->scaleFactor() < 1) {
				if (m_gestureZoomInertionBreak > 0) {
					m_gestureZoomInertionBreak = 0;
				} else if (m_gestureZoomInertionBreak <= -8) {
					m_gestureZoomInertionBreak = 0;
					--zoomRange;
				} else {
					--m_gestureZoomInertionBreak;
				}
			}
			setZoomRange(zoomRange);

			_event->accept();
		}
	}
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
			verticalScrollBar()->setRange(0, maximumValue);
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
			verticalScrollBar()->setRange(0, maximumValue);
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

void PagesTextEdit::privateZoomIn(qreal _range, int _startPosition, int _endPosition)
{
	//
	// Нужно увеличить в _range раз
	// * шрифт каждого блока
	// * формат каждого блока
	//
	QTextCursor cursor(document());
	cursor.joinPreviousEditBlock();

	//
	// Обработаем позиции
	//
	cursor.setPosition(_startPosition);
	if (_endPosition == 0) {
		_endPosition = document()->characterCount() - 1;
	}

	do {
		//
		// Выделим редактируемый блок текста
		//
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

		//
		// Если масштаб текущего блока ещё не настроен
		//
		QTextBlockFormat blockFormat = cursor.blockFormat();
		if (blockFormat.property(ZOOM_PROPERTY).toReal() != _range) {

			//
			// Обновляем настройки шрифта
			//
			QTextCharFormat blockCharFormat = cursor.charFormat();
			QFont blockFont = blockCharFormat.font();
			blockFont.setPointSizeF(scale(blockFont.pointSizeF(), _range));
			blockCharFormat.setFont(blockFont);
			cursor.setCharFormat(blockCharFormat);
			cursor.setBlockCharFormat(blockCharFormat);

			//
			// Обновляем настройки позиционирования
			//
			// ... сохраним настройку масштабирования
			//
			blockFormat.setProperty(ZOOM_PROPERTY, _range);
			//
			// ... обновим сами настройки
			//
			if (blockFormat.lineHeightType() == QTextBlockFormat::FixedHeight) {
				blockFormat.setLineHeight(scale(blockFormat.lineHeight(), _range), QTextBlockFormat::FixedHeight);
			}
			if (blockFormat.leftMargin() > 0) {
				blockFormat.setLeftMargin(scale(blockFormat.leftMargin(), _range));
			}
			if (blockFormat.topMargin() > 0) {
				blockFormat.setTopMargin(scale(blockFormat.topMargin(), _range));
			}
			if (blockFormat.rightMargin() > 0) {
				blockFormat.setRightMargin(scale(blockFormat.rightMargin(), _range));
			}
			if (blockFormat.bottomMargin() > 0) {
				blockFormat.setBottomMargin(scale(blockFormat.bottomMargin(), _range));
			}
			cursor.setBlockFormat(blockFormat);
		}

		//
		// Переходим к следующему блоку
		//
		cursor.movePosition(QTextCursor::NextBlock);
	} while (cursor.position() < _endPosition
			 && !cursor.atEnd());

	cursor.endEditBlock();
}

void PagesTextEdit::privateZoomOut(qreal _range, int _startPosition, int _endPosition)
{
	privateZoomIn(-_range, _startPosition, _endPosition);
}

void PagesTextEdit::aboutUpdateZoomRangeHandling()
{
	//
	// Если изменился документ, то нужно перепривязать сигнал обновления масштаба
	//
	if (document() != 0 && m_document != document()) {
		//
		// Обновим документ
		//
		m_document = document();
		connect(m_document, SIGNAL(contentsChange(int,int,int)), this, SLOT(aboutUpdateZoomRange(int,int,int)));

		//
		// Обновим масштаб документа
		//
		privateZoomIn(::zoomRange(m_zoomRange));
	}
}

void PagesTextEdit::aboutUpdateZoomRange(int _position, int _charsRemoved, int _charsAdded)
{
	Q_UNUSED(_charsRemoved);

	if (!m_inZoomHandling) {
		//
		// Обновим масштаб изменённого текста
		//
		privateZoomIn(::zoomRange(m_zoomRange), _position, _position + _charsAdded);
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


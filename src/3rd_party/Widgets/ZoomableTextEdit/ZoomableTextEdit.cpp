#include "ZoomableTextEdit.h"

#include <QWheelEvent>


ZoomableTextEdit::ZoomableTextEdit(QWidget* _parent) :
	QTextEdit(_parent),
	m_zoomRange(0)
{
}

void ZoomableTextEdit::setZoomRange(int _zoomRange)
{
	//
	// Отменяем предыдущее мастштабирование
	//
	zoomOut(m_zoomRange);

	//
	// Обновляем коэффициент
	//
	m_zoomRange = _zoomRange;
	emit zoomRangeChanged(m_zoomRange);

	//
	// Масштабируем с новым коэффициентом
	//
	zoomIn(m_zoomRange);
}

void ZoomableTextEdit::resetZoom()
{
	setZoomRange(m_zoomRange);
}

void ZoomableTextEdit::wheelEvent(QWheelEvent* _event)
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

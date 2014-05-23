#include "PageMetrics.h"

#include <QApplication>
#include <QDesktopWidget>


namespace {
	static qreal mmToInches(qreal mm) { return mm * 0.039370147; }
}

qreal PageMetrics::mmToPx(qreal _mm, bool _x)
{
	return ::mmToInches(_mm) * (_x ? qApp->desktop()->logicalDpiX() : qApp->desktop()->logicalDpiY());
}

PageMetrics::PageMetrics(QPageSize::PageSizeId _pageFormat,
						 QMarginsF _mmPageMargins) :
	m_zoomRange(1)
{
	update(_pageFormat, _mmPageMargins);
}

void PageMetrics::update(QPageSize::PageSizeId _pageFormat, QMarginsF _mmPageMargins)
{
	m_pageFormat = _pageFormat;

	m_mmPageSize = QPageSize(m_pageFormat).rect(QPageSize::Millimeter).size();
	m_mmPageMargins = _mmPageMargins;

	//
	// Рассчитываем значения в пикселах
	//
	const bool x = true, y = false;
	m_pxPageSize =
			QSizeF(mmToPx(m_mmPageSize.width(), x),
				   mmToPx(m_mmPageSize.height(), y));
	m_pxPageMargins =
			QMarginsF(mmToPx(m_mmPageMargins.left(), x),
					  mmToPx(m_mmPageMargins.top(), y),
					  mmToPx(m_mmPageMargins.right(), x),
					  mmToPx(m_mmPageMargins.bottom(), y)
					  );
}

void PageMetrics::zoomIn(qreal _zoomRange)
{
	m_zoomRange = _zoomRange;
}

QPageSize::PageSizeId PageMetrics::pageFormat() const
{
	return m_pageFormat;
}

QSizeF PageMetrics::mmPageSize() const
{
	return m_mmPageSize;
}

QMarginsF PageMetrics::mmPageMargins() const
{
	return m_mmPageMargins * m_zoomRange;
}

QSizeF PageMetrics::pxPageSize() const
{
	return QSizeF(m_pxPageSize.width() * m_zoomRange,
				  m_pxPageSize.height() * m_zoomRange);
}

QMarginsF PageMetrics::pxPageMargins() const
{
	return QMarginsF(m_pxPageMargins.left() * m_zoomRange,
					 m_pxPageMargins.top() * m_zoomRange,
					 m_pxPageMargins.bottom() * m_zoomRange,
					 m_pxPageMargins.right() * m_zoomRange);
}

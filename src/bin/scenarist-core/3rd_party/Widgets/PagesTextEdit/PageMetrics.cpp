#include "PageMetrics.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QFontMetricsF>


qreal PageMetrics::mmToPx(qreal _mm, bool _x)
{
	//
	// FIXME: !!!!
	//
	// Рассчитываем исходя из знания, что один символ шрифта Courier New 12pt
	// высотой 4,8мм и шириной 2,53мм
	//
	const QFontMetricsF courierNewMetrics(QFont("Courier New", 12));
	return _x ? ((courierNewMetrics.width("W") * _mm) / 2.53)
			  : ((courierNewMetrics.lineSpacing() * _mm) / 4.8);
}

QPageSize::PageSizeId PageMetrics::pageSizeIdFromString(const QString& _from)
{
	QPageSize::PageSizeId result = QPageSize::A4;

	if (_from == "A4") result = QPageSize::A4;
	else if (_from == "Letter") result = QPageSize::Letter;
	else
		Q_ASSERT_X(0, Q_FUNC_INFO, qPrintable("Undefined page size: " + _from));

	return result;
}

QString PageMetrics::stringFromPageSizeId(QPageSize::PageSizeId _pageSize)
{
	QString result;

	switch (_pageSize) {
		case QPageSize::A4: result = "A4"; break;
		case QPageSize::Letter: result = "Letter"; break;
		default:
			Q_ASSERT_X(0, Q_FUNC_INFO, qPrintable("Undefined page size: " + QString::number(_pageSize)));
	}

	return result;
}

PageMetrics::PageMetrics(QPageSize::PageSizeId _pageFormat, QMarginsF _mmPageMargins)
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
	return m_mmPageMargins;
}

QSizeF PageMetrics::pxPageSize() const
{
	return QSizeF(m_pxPageSize.width(),
				  m_pxPageSize.height());
}

QMarginsF PageMetrics::pxPageMargins() const
{
	return QMarginsF(m_pxPageMargins.left(),
					 m_pxPageMargins.top(),
					 m_pxPageMargins.right(),
					 m_pxPageMargins.bottom());
}

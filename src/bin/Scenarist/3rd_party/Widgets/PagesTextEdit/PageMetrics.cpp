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

QPageSize::PageSizeId PageMetrics::pageSizeIdFromString(const QString& _from)
{
	QPageSize::PageSizeId result = QPageSize::A4;

	if (_from == "A0") result = QPageSize::A0;
	else if (_from == "A1") result = QPageSize::A1;
	else if (_from == "A2") result = QPageSize::A2;
	else if (_from == "A3") result = QPageSize::A3;
	else if (_from == "A4") result = QPageSize::A4;
	else if (_from == "A5") result = QPageSize::A5;
	else if (_from == "A6") result = QPageSize::A6;
	else
		Q_ASSERT_X(0, Q_FUNC_INFO, qPrintable("Undefined page size: " + _from));

	return result;
}

QString PageMetrics::stringFromPageSizeId(QPageSize::PageSizeId _pageSize)
{
	QString result;

	switch (_pageSize) {
		case QPageSize::A0: result = "A0"; break;
		case QPageSize::A1: result = "A1"; break;
		case QPageSize::A2: result = "A2"; break;
		case QPageSize::A3: result = "A3"; break;
		case QPageSize::A4: result = "A4"; break;
		case QPageSize::A5: result = "A5"; break;
		case QPageSize::A6: result = "A6"; break;
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

#include "qcustomplotextended.h"

QCustomPlotExtended::QCustomPlotExtended(QWidget* _parent) :
	QCustomPlot(_parent)
{
	setMouseTracking(true);
}

void QCustomPlotExtended::setPlotInfo(const QMap<double, QStringList>& _info)
{
	if (m_info != _info) {
		m_info = _info;
	}
}

void QCustomPlotExtended::paintEvent(QPaintEvent* _event)
{
	QCustomPlot::paintEvent(_event);

	//
	// Выводим доступные данные
	//
	if (axisRect()->rect().contains(m_mousePos.toPoint())) {
		//
		// Рисуем дополнительную информацию о графике
		//
		QPainter p(this);

		//
		// Мониторим положение мыши
		//
		p.setPen(QPen(palette().text(), 1));
		p.drawLine(QPoint(m_mousePos.x(), axisRect()->rect().top()), QPoint(m_mousePos.x(), axisRect()->rect().bottom()));

		//
		// Определим ближайшую точку с иннформацией, которую нужно отобразить
		//
		const double mouseX = xAxis->pixelToCoord(m_mousePos.x());
		if (mouseX >= 0) {
			QMap<double, QStringList>::Iterator nearMouse = m_info.upperBound(mouseX);
			if (nearMouse != m_info.end()) {
				if (nearMouse != m_info.begin()) {
					--nearMouse;
				}
				const QString infoTitle = nearMouse.value().first();
				const QString infoText = nearMouse.value().last();

				//
				// Выводим информацию о текущем положении
				//
				if (!infoTitle.isEmpty() && !infoText.isEmpty()) {
					//
					// Определим область заголовка
					//
					QRect titleBoundingRect = p.fontMetrics().boundingRect(infoTitle);
					titleBoundingRect.moveTo(m_mousePos.x() + 20, m_mousePos.y());
					titleBoundingRect.setBottom(titleBoundingRect.bottom() + 6);

					//
					// Определим область текста
					//
					int textRectWidth = 0;
					int textRectHeight = 0;
					foreach (const QString& line, infoText.split("\n")) {
						int lineWidth = p.fontMetrics().width(line);
						if (lineWidth > textRectWidth) {
							textRectWidth = lineWidth;
						}
						textRectHeight += p.fontMetrics().height();
					}
					QRect textBoundingRect(0, 0, textRectWidth + 12, textRectHeight + 6);
					textBoundingRect.moveTo(titleBoundingRect.bottomLeft());

					//
					// Скорректируем области
					//
					if (titleBoundingRect.width() > textBoundingRect.width()) {
						textBoundingRect.setWidth(titleBoundingRect.width());
					} else {
						titleBoundingRect.setWidth(textBoundingRect.width());
					}

					//
					// Сформируем результирующую область
					//
					QRect fullBoundingRect(titleBoundingRect.topLeft(), textBoundingRect.bottomRight());

					//
					// Заливаем область и рисуем рамку
					//
					QColor backgroundColor = palette().base().color();
					backgroundColor.setAlphaF(0.7);
					p.fillRect(fullBoundingRect, backgroundColor);
					p.drawRect(fullBoundingRect);

					//
					// Выводим тексты
					//
					QFont font = p.font();
					font.setBold(true);
					p.setFont(font);
					p.drawText(titleBoundingRect, Qt::AlignCenter, infoTitle);
					//
					font.setBold(false);
					p.setFont(font);
					p.drawText(textBoundingRect.adjusted(6, 0, 0, 0), Qt::AlignLeft, infoText);
				}
			}
		}

		p.end();
	}
}

void QCustomPlotExtended::mouseMoveEvent(QMouseEvent* _event)
{
	QCustomPlot::mouseMoveEvent(_event);

	m_mousePos = _event->localPos();

	update();
}


#ifndef QCUSTOMPLOTEXTENDED_H
#define QCUSTOMPLOTEXTENDED_H

#include "qcustomplot.h"

class QCustomPlotExtended : public QCustomPlot
{
public:
	explicit QCustomPlotExtended(QWidget* _parent = 0);

	void setPlotInfo(const QMap<double, QStringList>& _info);

protected:
	/**
	 * @brief
	 */
	void paintEvent(QPaintEvent* _event);
	void mouseMoveEvent(QMouseEvent* _event);

private:
	/**
	 * @brief Позиция мыши
	 */
	QPointF m_mousePos;

	/**
	 * @brief Информация о графмке
	 */
	QMap<double, QStringList> m_info;
};

#endif // QCUSTOMPLOTEXTENDED_H

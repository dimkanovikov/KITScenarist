#include "GoogleColorsPane.h"

#include <QPainter>
#include <QMouseEvent>

namespace {
	/**
	 * @brief Размер ребра квадрата с цветом
	 */
	const int COLOR_RECT_SIZE = 20;

	/**
	 * @brief Расстояние между двумя соседними квадратами с цветом
	 */
	const int COLOR_RECT_SPACE = 3;

	/**
	 * @brief Количество колонок с цветовыми квадратами
	 */
	const int COLOR_RECT_COLUMNS = 10;

	/**
	 * @brief Количество строк с цветовыми квадратами
	 */
	const int COLOR_RECT_ROWS = 8;

	/**
	 * @brief Отступы панели
	 */
	const int PANEL_MARGIN = 16;

	/**
	 * @brief Размер метки текущего цвета
	 */
	const int COLOR_MARK_SIZE = 5;
}


GoogleColorsPane::GoogleColorsPane(QWidget* _parent) :
	ColorsPane(_parent)
{
	//
	// Рассчитаем фиксированный размер панели
	//
	const int width =
			(COLOR_RECT_SIZE * COLOR_RECT_COLUMNS) // ширина цветовых квадратов
			+ (COLOR_RECT_SPACE * (COLOR_RECT_COLUMNS - 1)) // ширина оступов между квадратами
			+ (PANEL_MARGIN * 2); // ширина полей
	const int height =
			(COLOR_RECT_SIZE * COLOR_RECT_ROWS) // высота цветовых квадратов
			+ (COLOR_RECT_SPACE * (COLOR_RECT_ROWS + 3)) // высота отступов между ними (+3 т.к. между 1, 2 и 3 увеличенные отступы)
			+ (PANEL_MARGIN * 2); // высота полей
	setFixedSize(width, height);

	//
	// Отслеживаем движения мыши
	//
	setMouseTracking( true );

	//
	// Настроим курсор
	//
	setCursor(Qt::PointingHandCursor);
}

QColor GoogleColorsPane::currentColor() const
{
	return m_currentColorInfo.color;
}

void GoogleColorsPane::setCurrentColor(const QColor& _color)
{
	for (int colorIndex = 0; colorIndex < m_colorInfos.count(); ++colorIndex) {
		if (m_colorInfos[colorIndex].color == _color) {
			m_currentColorInfo = m_colorInfos[colorIndex];
			break;
		}
	}
}

void GoogleColorsPane::paintEvent(QPaintEvent * _event)
{
	Q_UNUSED(_event);

	QPainter painter( this );


	//
	// Рисуем первый ряд
	//
	int topMargin = PANEL_MARGIN;
	int leftMargin = PANEL_MARGIN;
	QList<QColor> colors;
	colors << QColor(0,0,0)
		   << QColor(67,67,67)
		   << QColor(102,102,102)
		   << QColor(153,153,153)
		   << QColor(183,183,183)
		   << QColor(204,204,204)
		   << QColor(217,217,217)
		   << QColor(239,239,239)
		   << QColor(243,243,243)
		   << QColor(255,255,255);
	for (int column = 0; column < COLOR_RECT_COLUMNS; ++column) {
		QRectF colorRect;
		colorRect.setLeft(leftMargin);
		colorRect.setTop(topMargin);
		colorRect.setWidth(COLOR_RECT_SIZE);
		colorRect.setHeight(COLOR_RECT_SIZE);

		painter.fillRect(colorRect, colors.at(column));
		m_colorInfos.append(ColorKeyInfo(colors.at(column), colorRect));

		leftMargin += COLOR_RECT_SIZE + COLOR_RECT_SPACE;
	}
	topMargin += COLOR_RECT_SIZE + (COLOR_RECT_SPACE * 3);


	//
	// Второй ряд
	//
	leftMargin = PANEL_MARGIN;
	colors.clear();
	colors << QColor(152,0,0)
		   << QColor(255,0,0)
		   << QColor(255,135,0)
		   << QColor(255,255,0)
		   << QColor(0,255,0)
		   << QColor(0,255,255)
		   << QColor(74,134,232)
		   << QColor(0,0,255)
		   << QColor(153,0,255)
		   << QColor(255,0,255);
	for (int column = 0; column < COLOR_RECT_COLUMNS; ++column) {
		QRectF colorRect;
		colorRect.setLeft(leftMargin);
		colorRect.setTop(topMargin);
		colorRect.setWidth(COLOR_RECT_SIZE);
		colorRect.setHeight(COLOR_RECT_SIZE);

		painter.fillRect(colorRect, colors.at(column));
		m_colorInfos.append(ColorKeyInfo(colors.at(column), colorRect));

		leftMargin += COLOR_RECT_SIZE + COLOR_RECT_SPACE;
	}
	topMargin += COLOR_RECT_SIZE + (COLOR_RECT_SPACE * 3);


	//
	// Остальные ряды
	//
	colors.clear();
	colors << QColor("#e6b8af")
		   << QColor("#f4cccc")
		   << QColor("#fce5cd")
		   << QColor("#fff2cc")
		   << QColor("#d9ead3")
		   << QColor("#d0e0e3")
		   << QColor("#c9daf8")
		   << QColor("#cfe2f3")
		   << QColor("#d9d2e9")
		   << QColor("#ead1dc") // ****
		   << QColor("#da7d6a")
		   << QColor("#ea9999")
		   << QColor("#f9cb9c")
		   << QColor("#ffe599")
		   << QColor("#b6d7a8")
		   << QColor("#a2c4c9")
		   << QColor("#a4c2f4")
		   << QColor("#9fc5e8")
		   << QColor("#b4a7d6")
		   << QColor("#d5a6bd") // ****
		   << QColor("#c63f24")
		   << QColor("#e06666")
		   << QColor("#f6b26b")
		   << QColor("#ffd966")
		   << QColor("#93c47d")
		   << QColor("#76a5af")
		   << QColor("#6d9eeb")
		   << QColor("#6fa8dc")
		   << QColor("#8e7cc3")
		   << QColor("#c27ba0") // ****
		   << QColor("#a21b00")
		   << QColor("#cc0000")
		   << QColor("#e69138")
		   << QColor("#f1c232")
		   << QColor("#6aa84f")
		   << QColor("#45818e")
		   << QColor("#3c78d8")
		   << QColor("#3d85c6")
		   << QColor("#674ea7")
		   << QColor("#a64d79") // ****
		   << QColor("#831f0c")
		   << QColor("#990000")
		   << QColor("#b45f06")
		   << QColor("#bf9000")
		   << QColor("#38761d")
		   << QColor("#134f5c")
		   << QColor("#1155cc")
		   << QColor("#0b5394")
		   << QColor("#351c75")
		   << QColor("#741b47") // ****
		   << QColor("#5a0f00")
		   << QColor("#660000")
		   << QColor("#783f04")
		   << QColor("#7f6000")
		   << QColor("#274e13")
		   << QColor("#0c343d")
		   << QColor("#1c4587")
		   << QColor("#073763")
		   << QColor("#20124d")
		   << QColor("#4c1130");
	for (int row = 0; row < (COLOR_RECT_ROWS - 2); ++row) {
		leftMargin = PANEL_MARGIN;
		for (int column = 0; column < COLOR_RECT_COLUMNS; ++column) {
			QRectF colorRect;
			colorRect.setLeft(leftMargin);
			colorRect.setTop(topMargin);
			colorRect.setWidth(COLOR_RECT_SIZE);
			colorRect.setHeight(COLOR_RECT_SIZE);

			painter.fillRect(colorRect, colors.at((row * COLOR_RECT_COLUMNS) + column));
			m_colorInfos.append(ColorKeyInfo(colors.at((row * COLOR_RECT_COLUMNS) + column), colorRect));

			leftMargin += COLOR_RECT_SIZE + COLOR_RECT_SPACE;
		}
		topMargin += COLOR_RECT_SIZE + COLOR_RECT_SPACE;
	}

	//
	// Обрамление
	//
	const QPoint mousePos = mapFromGlobal(QCursor::pos());
	for(int colorIndex = 0; colorIndex < m_colorInfos.count(); ++colorIndex) {
		if(m_colorInfos[colorIndex].rect.contains(mousePos))
		{
			QRectF borderRect = m_colorInfos[colorIndex].rect;
			borderRect.setTop(borderRect.top() - 2);
			borderRect.setLeft(borderRect.left() - 2);
			borderRect.setBottom(borderRect.bottom() + 1);
			borderRect.setRight(borderRect.right() + 1);

			painter.setPen(palette().text().color());
			painter.drawRect(borderRect);

			break;
		}
	}

	//
	// Текущий
	//
	if (m_currentColorInfo.isValid()) {
		//
		// ... рамка
		//
		QRectF borderRect = m_currentColorInfo.rect;
		borderRect.setTop(borderRect.top() - 2);
		borderRect.setLeft(borderRect.left() - 2);
		borderRect.setBottom(borderRect.bottom() + 1);
		borderRect.setRight(borderRect.right() + 1);

		painter.setPen(palette().text().color());
		painter.drawRect(borderRect);

		//
		// ... метка в центре
		//
		const QPointF center = borderRect.center();
		QRectF markRect(center.x() - COLOR_MARK_SIZE / 2, center.y() - COLOR_MARK_SIZE / 2,
			COLOR_MARK_SIZE, COLOR_MARK_SIZE);
		painter.fillRect(markRect, palette().text());
		painter.setPen(palette().base().color());
		painter.drawRect(markRect);
	}
}

void GoogleColorsPane::mouseMoveEvent(QMouseEvent* _event)
{
	Q_UNUSED(_event);

	repaint();
}

void GoogleColorsPane::mousePressEvent(QMouseEvent* _event)
{
	for (int colorIndex = 0; colorIndex < m_colorInfos.count(); ++colorIndex) {
		if (m_colorInfos[colorIndex].rect.contains(_event->pos())) {
			m_currentColorInfo = m_colorInfos[colorIndex];
			emit selected(m_currentColorInfo.color);

			repaint();
			break;
		}
	}
}

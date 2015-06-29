#include "WordHighlightColorsPane.h"

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
	const int COLOR_RECT_COLUMNS = 5;

	/**
	 * @brief Количество строк с цветовыми квадратами
	 */
	const int COLOR_RECT_ROWS = 3;

	/**
	 * @brief Отступы панели
	 */
	const int PANEL_MARGIN = 16;

	/**
	 * @brief Размер метки текущего цвета
	 */
	const int COLOR_MARK_SIZE = 5;
}


WordHighlightColorsPane::WordHighlightColorsPane(QWidget* _parent) :
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
			+ (COLOR_RECT_SPACE * (COLOR_RECT_ROWS)) // высота отступов между ними (+3 т.к. между 1, 2 и 3 увеличенные отступы)
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

QColor WordHighlightColorsPane::currentColor() const
{
	return m_currentColorInfo.color;
}

void WordHighlightColorsPane::setCurrentColor(const QColor& _color)
{
	for (int colorIndex = 0; colorIndex < m_colorInfos.count(); ++colorIndex) {
		if (m_colorInfos[colorIndex].color == _color) {
			m_currentColorInfo = m_colorInfos[colorIndex];
			break;
		}
	}
}

void WordHighlightColorsPane::paintEvent(QPaintEvent * _event)
{
	Q_UNUSED(_event);

	QPainter painter( this );


	//
	// Рисуем панель
	//
	int topMargin = PANEL_MARGIN;
	int leftMargin = PANEL_MARGIN;
	QList<QColor> colors;
	colors << QColor("#ffff00")
		   << QColor("#00ff00")
		   << QColor("#00ffff")
		   << QColor("#ff00ff")
		   << QColor("#0000ff") // ****
		   << QColor("#ff0000")
		   << QColor("#000080")
		   << QColor("#008080")
		   << QColor("#008000")
		   << QColor("#800080") // ****
		   << QColor("#800000")
		   << QColor("#808000")
		   << QColor("#808080")
		   << QColor("#c0c0c0")
		   << QColor("#000000");
	for (int row = 0; row < COLOR_RECT_ROWS; ++row) {
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

void WordHighlightColorsPane::mouseMoveEvent(QMouseEvent* _event)
{
	Q_UNUSED(_event);

	repaint();
}

void WordHighlightColorsPane::mousePressEvent(QMouseEvent* _event)
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

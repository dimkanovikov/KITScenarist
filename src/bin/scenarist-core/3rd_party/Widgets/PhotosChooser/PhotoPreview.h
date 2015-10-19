#ifndef PHOTOPREVIEW_H
#define PHOTOPREVIEW_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.h>

class QLabel;


class PhotoPreview : public QLightBoxWidget
{
	Q_OBJECT
public:
	explicit PhotoPreview(QWidget *parent);

	/**
	 * @brief Установить картинку для отображения
	 */
	void setImage(const QPixmap& _pixmap);

protected:
	/**
	 * @brief Переопределяется для скрытия виджета, когда на нём кликнули
	 */
	void mousePressEvent(QMouseEvent* _event);

	void resizeEvent(QResizeEvent* _event);

private:
	void repaintPixmap();

private:
	QLabel* m_pixmapLabel;
	QPixmap m_pixmap;

};

#endif // PHOTOPREVIEW_H

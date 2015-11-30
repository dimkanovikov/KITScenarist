#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.h>

class QLabel;

//
// TODO: сделать общий предпросмотрщик с другой панелью изображений
//


class ImagePreview : public QLightBoxWidget
{
	Q_OBJECT
public:
	explicit ImagePreview(QWidget *parent);

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

#endif // IMAGEPREVIEW_H

#include "ImagePreview.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>


ImagePreview::ImagePreview(QWidget *parent) :
	QLightBoxWidget(parent, true)
{
	m_pixmapLabel = new QLabel(this);
	m_pixmapLabel->setScaledContents(true);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addStretch();
	layout->addWidget(m_pixmapLabel, 0, Qt::AlignCenter | Qt::AlignHCenter);
	layout->addStretch();
	setLayout(layout);
}

void ImagePreview::setImage(const QPixmap& _pixmap)
{
	m_pixmap = _pixmap;

	repaintPixmap();
}

void ImagePreview::mousePressEvent(QMouseEvent* _event)
{
	hide();

	_event->accept();
}

void ImagePreview::resizeEvent(QResizeEvent* _event)
{
	QLightBoxWidget::resizeEvent(_event);

	repaintPixmap();
}

void ImagePreview::repaintPixmap()
{
	QSize scaleSize = m_pixmap.size();
	if ((scaleSize.width() > parentWidget()->size().width())
		|| (scaleSize.height() > parentWidget()->size().height())){
		scaleSize = parentWidget()->size();
	}
	m_pixmapLabel->setPixmap(
				m_pixmap.scaled(scaleSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

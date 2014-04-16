#include "PhotoPreview.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>


PhotoPreview::PhotoPreview(QWidget *parent) :
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

void PhotoPreview::setImage(const QPixmap& _pixmap)
{
	m_pixmap = _pixmap;

	repaintPixmap();
}

void PhotoPreview::mousePressEvent(QMouseEvent* _event)
{
	hide();

	_event->accept();
}

void PhotoPreview::resizeEvent(QResizeEvent* _event)
{
	QLightBoxWidget::resizeEvent(_event);

	repaintPixmap();
}

void PhotoPreview::repaintPixmap()
{
	QSize scaleSize = m_pixmap.size();
	if ((scaleSize.width() > parentWidget()->size().width())
		|| (scaleSize.height() > parentWidget()->size().height())){
		scaleSize = parentWidget()->size();
	}
	m_pixmapLabel->setPixmap(
				m_pixmap.scaled(scaleSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

#include "PhotosChooser.h"

#include "PhotoLabel.h"
#include "PhotoPreview.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


PhotosChooser::PhotosChooser(QWidget* _parent) :
	QWidget(_parent),
	m_mainPhoto(new PhotoLabel(this)),
	m_additionalPhoto1(new PhotoLabel(this)),
	m_additionalPhoto2(new PhotoLabel(this)),
	m_additionalPhoto3(new PhotoLabel(this)),
	m_additionalPhoto4(new PhotoLabel(this)),
	m_photoPreview(0)
{
	m_mainPhoto->setMinimumSize(144, 180);
	connect(m_mainPhoto, SIGNAL(photoClicked(QPixmap)), this, SLOT(showPhoto(QPixmap)));

	m_additionalPhoto1->setMinimumSize(72, 90);
	connect(m_additionalPhoto1, SIGNAL(photoClicked(QPixmap)), this, SLOT(showPhoto(QPixmap)));

	m_additionalPhoto2->setMinimumSize(72, 90);
	connect(m_additionalPhoto2, SIGNAL(photoClicked(QPixmap)), this, SLOT(showPhoto(QPixmap)));

	m_additionalPhoto3->setMinimumSize(72, 90);
	connect(m_additionalPhoto3, SIGNAL(photoClicked(QPixmap)), this, SLOT(showPhoto(QPixmap)));

	m_additionalPhoto4->setMinimumSize(72, 90);
	connect(m_additionalPhoto4, SIGNAL(photoClicked(QPixmap)), this, SLOT(showPhoto(QPixmap)));

	QHBoxLayout* additionalLayout1 = new QHBoxLayout;
	additionalLayout1->addWidget(m_additionalPhoto1);
	additionalLayout1->addWidget(m_additionalPhoto2);
	QHBoxLayout* additionalLayout2 = new QHBoxLayout;
	additionalLayout2->addWidget(m_additionalPhoto3);
	additionalLayout2->addWidget(m_additionalPhoto4);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins(6, 0, 6, 6));
	layout->addWidget(m_mainPhoto, 1);
	layout->addLayout(additionalLayout1);
	layout->addLayout(additionalLayout2);

	this->setLayout(layout);

	setStyleSheet("QLabel { background-color: palette(button); border: 1px solid palette(text); }");
}

void PhotosChooser::showPhoto(const QPixmap& _photo)
{
	if (!_photo.isNull()) {
		if (m_photoPreview == 0) {
			m_photoPreview = new PhotoPreview(this);
		}
		m_photoPreview->setImage(_photo);
		m_photoPreview->show();
	}
}

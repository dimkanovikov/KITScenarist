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
	connect(m_mainPhoto, SIGNAL(photoChanged()), this, SIGNAL(photoChanged()));

	m_additionalPhoto1->setMinimumSize(72, 90);
	connect(m_additionalPhoto1, SIGNAL(photoClicked(QPixmap)), this, SLOT(showPhoto(QPixmap)));
	connect(m_additionalPhoto1, SIGNAL(photoChanged()), this, SIGNAL(photoChanged()));

	m_additionalPhoto2->setMinimumSize(72, 90);
	connect(m_additionalPhoto2, SIGNAL(photoClicked(QPixmap)), this, SLOT(showPhoto(QPixmap)));
	connect(m_additionalPhoto2, SIGNAL(photoChanged()), this, SIGNAL(photoChanged()));

	m_additionalPhoto3->setMinimumSize(72, 90);
	connect(m_additionalPhoto3, SIGNAL(photoClicked(QPixmap)), this, SLOT(showPhoto(QPixmap)));
	connect(m_additionalPhoto3, SIGNAL(photoChanged()), this, SIGNAL(photoChanged()));

	m_additionalPhoto4->setMinimumSize(72, 90);
	connect(m_additionalPhoto4, SIGNAL(photoClicked(QPixmap)), this, SLOT(showPhoto(QPixmap)));
	connect(m_additionalPhoto4, SIGNAL(photoChanged()), this, SIGNAL(photoChanged()));

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

void PhotosChooser::setPhotos(const QList<QPixmap>& _photos)
{
	if (!_photos.value(0).isNull()) {
		m_mainPhoto->setPhoto(_photos.value(0));
	}
	if (!_photos.value(1).isNull()) {
		m_additionalPhoto1->setPhoto(_photos.value(1));
	}
	if (!_photos.value(2).isNull()) {
		m_additionalPhoto2->setPhoto(_photos.value(2));
	}
	if (!_photos.value(3).isNull()) {
		m_additionalPhoto3->setPhoto(_photos.value(3));
	}
	if (!_photos.value(4).isNull()) {
		m_additionalPhoto4->setPhoto(_photos.value(4));
	}
}

QList<QPixmap> PhotosChooser::photos() const
{
	QList<QPixmap> photos;

	if (!m_mainPhoto->photo().isNull()) {
		photos.append(m_mainPhoto->photo());
	}
	if (!m_additionalPhoto1->photo().isNull()) {
		photos.append(m_additionalPhoto1->photo());
	}
	if (!m_additionalPhoto2->photo().isNull()) {
		photos.append(m_additionalPhoto2->photo());
	}
	if (!m_additionalPhoto3->photo().isNull()) {
		photos.append(m_additionalPhoto3->photo());
	}
	if (!m_additionalPhoto4->photo().isNull()) {
		photos.append(m_additionalPhoto4->photo());
	}

	return photos;
}

void PhotosChooser::clear()
{
	m_mainPhoto->setPhoto(QPixmap());
	m_additionalPhoto1->setPhoto(QPixmap());
	m_additionalPhoto2->setPhoto(QPixmap());
	m_additionalPhoto3->setPhoto(QPixmap());
	m_additionalPhoto4->setPhoto(QPixmap());
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

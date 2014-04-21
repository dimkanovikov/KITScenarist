#include "PhotoLabel.h"

#include <QApplication>
#include <QFileDialog>
#include <QPainter>
#include <QStyle>
#include <QToolButton>
#include <QVariant>


PhotoLabel::PhotoLabel(QWidget* _parent) :
	QLabel(_parent)
{
	setCursor(Qt::PointingHandCursor);

	m_clearButton = new QToolButton(this);
	m_clearButton->setIcon(QIcon(":/Graphics/Icons/Editing/red_cross.png"));
	m_clearButton->setStyleSheet("border: none;");
	m_clearButton->setAttribute(Qt::WA_OpaquePaintEvent);
	m_clearButton->resize(m_clearButton->sizeHint());
	m_clearButton->hide();

	connect(m_clearButton, SIGNAL(clicked()), this, SLOT(clearPhoto()));
}

void PhotoLabel::setPhoto(const QPixmap& _photo)
{
	m_photo = _photo;

	//
	// Если фотография не пустая скорректируем позицию для вывода
	//
	QPixmap photoToShow;
	if (!m_photo.isNull()) {
		//
		// Изображение должно быть чуть меньше метки, чтобы не увеличивать её размер
		//
		const int delta = 2;
		QSize photoSize(width() - delta, height() - delta);
		photoToShow = QPixmap(photoSize);

		//
		// Рисуем масштабированное фото
		//

		QPainter painter;
		painter.begin(&photoToShow);
		painter.fillRect(0, 0, width(), height(), qApp->palette().button());

		QPixmap scaledPhoto = m_photo.scaled(photoSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		painter.drawPixmap((width() - scaledPhoto.width()) / 2,
						   (height() - scaledPhoto.height()) / 2,
						   scaledPhoto);
		painter.end();
	}

	//
	// Устанавливаем изображение
	//
	setPixmap(photoToShow);

	//
	// Уведомляем о смене фотографии
	//
	emit photoChanged();
}

QPixmap PhotoLabel::photo() const
{
	return m_photo;
}

void PhotoLabel::enterEvent(QEvent* _event)
{
	//
	// Если фотография установлена покажем кнопку удаления
	//
	if (!m_photo.isNull()) {
		m_clearButton->move(this->width() - m_clearButton->width(), 0);
		m_clearButton->show();
	}

	QLabel::enterEvent(_event);
}

void PhotoLabel::leaveEvent(QEvent* _event)
{
	//
	// Скроем кнопку удаления
	//
	m_clearButton->hide();

	QLabel::leaveEvent(_event);
}

void PhotoLabel::mousePressEvent(QMouseEvent* _event)
{
	//
	// Если фотография не установлена откроем диалог выбора файла для выбора фотографии
	//
	if (m_photo.isNull()) {

		QString imageFile =
				QFileDialog::getOpenFileName(this,
											 tr("Choose image"),
											 QString(),
											 tr("Images (*.png *.jpeg *.jpg)"));
		if (!imageFile.isEmpty()) {
			setPhoto(QPixmap(imageFile));
		}
	}
	//
	// Если фотография установлена, то откроем её
	//
	else {
		emit photoClicked(m_photo);
	}

	QLabel::mousePressEvent(_event);
}

void PhotoLabel::resizeEvent(QResizeEvent* _event)
{
	setPhoto(m_photo);
	QLabel::resizeEvent(_event);
}

void PhotoLabel::clearPhoto()
{
	setPhoto(QPixmap());
	m_clearButton->hide();
}

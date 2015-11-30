#include "ImagesPane.h"

#include "FlowLayout.h"
#include "AddImageButton.h"
#include "ImageLabel.h"
#include "ImagePreview.h"


ImagesPane::ImagesPane(QWidget* _parent) :
	QScrollArea(_parent),
	m_layout(new FlowLayout(viewport())),
	m_addImageButton(new AddImageButton(viewport())),
	m_imagePreview(0)
{
	initView();
	initConnections();
}

void ImagesPane::clear()
{
	//
	// Закрываем виджеты изображений
	//
	while (m_layout->count() > 1) {
		QLayoutItem* layoutItem = m_layout->takeAt(1);

		QWidget* imageButton = layoutItem->widget();
		imageButton->close();

		delete imageButton;
		imageButton = NULL;

		delete layoutItem;
		layoutItem = NULL;
	}

	//
	// Очищаем список изображений
	//
	m_images.clear();
}

void ImagesPane::addImage(const QPixmap& _image)
{
	m_images.append(_image);

	ImageLabel* imageLabel = new ImageLabel(this);
	imageLabel->setFixedSize(200, 150);
	imageLabel->setImage(_image);
	imageLabel->setSortOrder(m_images.size() - 1);

	connect(imageLabel, &ImageLabel::clicked, this, &ImagesPane::showImage);
	connect(imageLabel, &ImageLabel::removeRequested, this, &ImagesPane::removeImage);

	m_layout->insertWidget(m_layout->count() - 1, imageLabel);

	emit imageAdded();
}

void ImagesPane::addImageFromFile(const QString& _imagePath)
{
	addImage(QPixmap(_imagePath));
}

QList<QPixmap> ImagesPane::images() const
{
	return m_images;
}

void ImagesPane::showImage()
{
	if (ImageLabel* imageLabel = qobject_cast<ImageLabel*>(sender())) {
		//
		// Создаём предпросмотрщика только на данном шаге, чтобы он охватил всю иерархию виджетов
		//
		if (m_imagePreview == 0) {
			m_imagePreview = new ImagePreview(viewport());
		}
		m_imagePreview->setImage(imageLabel->image());
		m_imagePreview->show();
	}
}

void ImagesPane::removeImage()
{
	if (ImageLabel* imageLabel = qobject_cast<ImageLabel*>(sender())) {
		//
		// Убираем изображение из списка
		//
		m_images.removeAt(imageLabel->sortOrder());

		//
		// Закрываем и удаляем кнопку
		//
		int imageButtonIndex = m_layout->indexOf(imageLabel);
		QLayoutItem* layoutItem = m_layout->takeAt(imageButtonIndex);

		imageLabel->close();

		delete imageLabel;
		imageLabel = NULL;

		delete layoutItem;
		layoutItem = NULL;

		emit imageRemoved();
	}
}

void ImagesPane::initView()
{
	setWidgetResizable(true);

	m_addImageButton->setFixedSize(150, 150);

	m_layout->addWidget(m_addImageButton);
}

void ImagesPane::initConnections()
{
	connect(m_addImageButton, &AddImageButton::fileChoosed, this, &ImagesPane::addImageFromFile);
}

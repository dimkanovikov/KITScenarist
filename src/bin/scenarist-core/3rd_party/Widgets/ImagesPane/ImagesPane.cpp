#include "ImagesPane.h"

#include "FlowLayout.h"
#include "AddImageButton.h"
#include "ImageLabel.h"
#include "ImagePreview.h"

#include <QDragEnterEvent>
#include <QMimeData>


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
		QLayoutItem* layoutItem = m_layout->takeAt(0);

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
	const int imageSortOrder = m_images.size();

	m_images.append(_image);

	ImageLabel* imageLabel = new ImageLabel(this);
	imageLabel->setFixedSize(200, 150);
	imageLabel->setImage(_image);
	imageLabel->setSortOrder(imageSortOrder);

	connect(imageLabel, &ImageLabel::clicked, this, &ImagesPane::showImage);
	connect(imageLabel, &ImageLabel::removeRequested, this, &ImagesPane::removeImage);

	m_layout->insertWidget(m_layout->count() - 1, imageLabel);

	emit imageAdded(_image, imageSortOrder);
}

void ImagesPane::addImageFromFile(const QString& _imagePath)
{
	m_lastSelectedImagePath = _imagePath;
	addImage(QPixmap(_imagePath));
}

QList<QPixmap> ImagesPane::images() const
{
	return m_images;
}

QString ImagesPane::lastSelectedImagePath() const
{
	return m_lastSelectedImagePath;
}

void ImagesPane::setLastSelectedImagePath(const QString& _path)
{
	if (m_lastSelectedImagePath != _path) {
		m_lastSelectedImagePath = _path;

		m_addImageButton->setLastSelectedImagePath(m_lastSelectedImagePath);
	}
}

void ImagesPane::dragEnterEvent(QDragEnterEvent* _event)
{
	//
	// TODO: редко когда бывает, что приходит именно само изображение, как правило - это ссылка или путь к файлу
	//
	if (_event->mimeData()->hasImage()) {
		_event->acceptProposedAction();
	}
}

void ImagesPane::dropEvent(QDropEvent* _event)
{
	if (_event->mimeData()->hasImage()) {
		QImage image = qvariant_cast<QImage>(_event->mimeData()->imageData());
		addImage(QPixmap::fromImage(image));
	}
}

bool ImagesPane::eventFilter(QObject* _object, QEvent* _event)
{
	if (_object == viewport()) {
		if (_event->type() == QEvent::DragEnter) {
			dragEnterEvent(static_cast<QDragEnterEvent*>(_event));
		} else if (_event->type() == QEvent::Drop) {
			dropEvent(static_cast<QDropEvent*>(_event));
		}
	}

	return QScrollArea::eventFilter(_object, _event);
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
		const QPixmap removedImage = imageLabel->image();
		const int removedImageSortOrder = imageLabel->sortOrder();

		//
		// Убираем изображение из списка
		//
		m_images.removeAt(removedImageSortOrder);

		//
		// Закрываем и удаляем кнопку
		//
		int imageLabelIndex = m_layout->indexOf(imageLabel);
		QLayoutItem* layoutItem = m_layout->takeAt(imageLabelIndex);

		imageLabel->close();

		delete imageLabel;
		imageLabel = NULL;

		delete layoutItem;
		layoutItem = NULL;

		emit imageRemoved(removedImage, removedImageSortOrder);
	}
}

void ImagesPane::initView()
{
	setWidgetResizable(true);
	setAcceptDrops(true);

	viewport()->installEventFilter(this);
	viewport()->setAcceptDrops(true);

	m_addImageButton->setFixedSize(150, 150);

	m_layout->addWidget(m_addImageButton);
}

void ImagesPane::initConnections()
{
	connect(m_addImageButton, &AddImageButton::fileChoosed, this, &ImagesPane::addImageFromFile);
}

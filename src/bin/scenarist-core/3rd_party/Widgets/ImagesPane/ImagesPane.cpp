#include "ImagesPane.h"

#include "FlowLayout.h"
#include "AddImageButton.h"
#include "ImageLabel.h"
#include "ImagePreview.h"

#include <NetworkRequestLoader.h>

#include <QDragEnterEvent>
#include <QMimeData>


ImagesPane::ImagesPane(QWidget* _parent) :
    QScrollArea(_parent),
    m_layout(0),
    m_addImageButton(new AddImageButton(viewport())),
    m_imagePreview(0)
{
    initView();
    initConnections();
}

void ImagesPane::setReadOnly(bool _readOnly)
{
    if (m_isReadOnly != _readOnly) {
        m_isReadOnly = _readOnly;
        m_addImageButton->setVisible(!_readOnly);
    }
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
    if (!_image.isNull()) {
        const int imageSortOrder = m_images.size();

        m_images.append(_image);

        ImageLabel* imageLabel = new ImageLabel(this);
        imageLabel->setFixedSize(200, 150);
        imageLabel->setImage(_image);
        imageLabel->setSortOrder(imageSortOrder);
        imageLabel->setReadOnly(m_isReadOnly);

        connect(imageLabel, &ImageLabel::clicked, this, &ImagesPane::showImage);
        connect(imageLabel, &ImageLabel::removeRequested, this, &ImagesPane::removeImage);

        m_layout->insertWidget(m_layout->count() - 1, imageLabel);

        emit imageAdded(_image, imageSortOrder);
    }
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
    _event->acceptProposedAction();
}

void ImagesPane::dragMoveEvent(QDragMoveEvent* _event)
{
    _event->acceptProposedAction();
}

void ImagesPane::dragLeaveEvent(QDragLeaveEvent* _event)
{
    _event->accept();
}

void ImagesPane::dropEvent(QDropEvent* _event)
{
    if (m_isReadOnly) {
        return;
    }

    const QMimeData *mimeData = _event->mimeData();

    //
    // Первым делом проверяем список ссылок, возможно выбраны сразу несколько фотогафий
    //
    if (mimeData->hasUrls()) {
        foreach (const QUrl& url, mimeData->urls()) {
            //
            // Обрабатываем только изображения
            //
            const QString urlString = url.toString().toLower();
            if (urlString.contains(".png")
                || urlString.contains(".jpg")
                || urlString.contains(".jpeg")
                || urlString.contains(".gif")
                || urlString.contains(".tiff")
                || urlString.contains(".bmp")) {
                //
                // ... локальные считываем из файла
                //
                if (url.scheme() == "file") {
                    const QPixmap pixmap(url.toLocalFile());
                    addImage(pixmap);
                }
                //
                // ... подгружаем картинки с инета
                //
                else if (url.scheme() == "http"
                           || url.scheme() == "https") {
                    const QByteArray pixmapData = NetworkRequestLoader::loadSync(url);
                    const QImage image = QImage::fromData(pixmapData);
                    const QPixmap pixmap = QPixmap::fromImage(image);
                    addImage(pixmap);
                }
            }
        }
    } else if (mimeData->hasImage()) {
        const QPixmap pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
        addImage(pixmap);
    }

    _event->acceptProposedAction();
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

        //
        // Обновляем порядок сортировки для последующих изображений
        //
        for (int imageIndex = removedImageSortOrder; imageIndex < m_images.size(); ++imageIndex) {
            QLayoutItem* imageForUpdateItem = m_layout->itemAt(imageIndex);
            if (ImageLabel* imageForUpdate = qobject_cast<ImageLabel*>(imageForUpdateItem->widget())) {
                imageForUpdate->setSortOrder(imageForUpdate->sortOrder() - 1);
            }
        }
    }
}

void ImagesPane::initView()
{
    setWidgetResizable(true);
    setAcceptDrops(true);

    m_addImageButton->setFixedSize(150, 150);

    QWidget* contents = new QWidget(this);
    contents->installEventFilter(this);
    contents->setAcceptDrops(true);
    m_layout = new FlowLayout(contents);
    m_layout->addWidget(m_addImageButton);
    setWidget(contents);
}

void ImagesPane::initConnections()
{
    connect(m_addImageButton, &AddImageButton::fileChoosed, this, &ImagesPane::addImageFromFile);
}

#ifndef IMAGEHELPER
#define IMAGEHELPER

#include <QByteArray>
#include <QBuffer>
#include <QPixmap>

namespace {
	/**
	 * @brief Максимальные размеры изображения
	 */
	/** @{ */
	const int IMAGE_MAX_WIDTH = 1600;
	const int IMAGE_MAX_HEIGHT = 1200;
	/** @} */

	/**
	 * @brief Формат сохраняемых изображений
	 */
	const char* IMAGE_FILE_FORMAT = "PNG";

	/**
	 * @brief Используем минимальное качество изображения (всё-таки у нас приложение не для фотографов)
	 */
	const int IMAGE_FILE_QUALITY = 0;
}


/**
 * @brief Вспомогательные функции для работы с изображениями
 */
class ImageHelper
{
public:
	/**
	 * @brief Сохранение изображения в массив байт
	 */
	static QByteArray bytesFromImage(const QPixmap& _image) {
		//
		// Если необходимо корректируем размер изображения
		//
		QPixmap imageScaled = _image;
		if (imageScaled.width() > IMAGE_MAX_WIDTH
			|| imageScaled.height() > IMAGE_MAX_HEIGHT) {
			imageScaled = imageScaled.scaled(IMAGE_MAX_WIDTH, IMAGE_MAX_HEIGHT, Qt::KeepAspectRatio);
		}

		//
		// Сохраняем изображение
		//
		QByteArray imageData;
		QBuffer imageDataBuffer(&imageData);
		imageDataBuffer.open(QIODevice::WriteOnly);
		imageScaled.save(&imageDataBuffer, IMAGE_FILE_FORMAT, IMAGE_FILE_QUALITY);
		return imageData;
	}

	/**
	 * @brief Загрузить изображение из массива байт
	 */
	static QPixmap imageFromBytes(const QByteArray& _bytes) {
		QPixmap image;
		image.loadFromData(_bytes);
		return image;
	}
};

#endif // IMAGEHELPER


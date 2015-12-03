#ifndef PHOTOLABEL_H
#define PHOTOLABEL_H

#include <QLabel>

class QToolButton;


/**
 * @brief Виджет самовыбирающейся картинки
 */
class ImageLabel : public QLabel
{
	Q_OBJECT

public:
	explicit ImageLabel(QWidget* _parent = 0);

	/**
	 * @brief Установить фотографию
	 */
	void setImage(const QPixmap& _image);

	/**
	 * @brief Получить фотографию
	 */
	QPixmap image() const;

	/**
	 * @brief Установить порядок сортировки
	 */
	void setSortOrder(int _sortOrder);

	/**
	 * @brief Получить порядок сортировки
	 */
	int sortOrder() const;

	/**
	 * @brief Установить режим "только для чтения"
	 */
	void setReadOnly(bool _readOnly);

signals:
	/**
	 * @brief Сигнал щелчка по изображению
	 */
	void clicked();

	/**
	 * @brief Пользователь хочет удалить изображение
	 */
	void removeRequested();

protected:
	/**
	 * @brief Переопределяется для прорисовки кнопок действий во время наведения мышки
	 */
	void enterEvent(QEvent* _event);

	/**
	 * @brief Переопределяется для сокрытия кнопок действий во время выхода мышки
	 */
	void leaveEvent(QEvent* _event);

	/**
	 * @brief Переопределяется для запуска события выбора изображения
	 */
	void mousePressEvent(QMouseEvent* _event);

	/**
	 * @brief Переопределяется для обновления фотографии при изменении размера виджета
	 */
	void resizeEvent(QResizeEvent* _event);

private:
	/**
	 * @brief Выбранная фотография
	 */
	QPixmap m_image;

	/**
	 * @brief Порядок сортировки
	 */
	int m_sortOrder;

	/**
	 * @brief Кнопка очистки фотографии
	 */
	QToolButton* m_clearButton;

	/**
	 * @brief Режим "только для чтения"
	 */
	bool m_isReadOnly;
};

#endif // PHOTOLABEL_H

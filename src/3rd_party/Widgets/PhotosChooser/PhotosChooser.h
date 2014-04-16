#ifndef PHOTOSCHOOSER_H
#define PHOTOSCHOOSER_H

#include <QWidget>

class PhotoLabel;
class QToolButton;
class PhotoPreview;


/**
 * @brief Класс для выбора фотографий
 */
class PhotosChooser : public QWidget
{
	Q_OBJECT

public:
	explicit PhotosChooser(QWidget* _parent = 0);

private slots:
	/**
	 * @brief Показать фотографию
	 */
	void showPhoto(const QPixmap& _photo);

private:
	/**
	 * @brief Основная фотография
	 */
	PhotoLabel* m_mainPhoto;

	/**
	 * @brief Дополнительные фотографии
	 */
	/** @{ */
	PhotoLabel* m_additionalPhoto1;
	PhotoLabel* m_additionalPhoto2;
	PhotoLabel* m_additionalPhoto3;
	PhotoLabel* m_additionalPhoto4;
	/** @} */

	/**
	 * @brief Виджет для отображения фото в полном размере
	 */
	PhotoPreview* m_photoPreview;
};

#endif // PHOTOSCHOOSER_H

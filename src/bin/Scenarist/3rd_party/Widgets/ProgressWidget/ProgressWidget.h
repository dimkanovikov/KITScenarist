#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.h>

class QLabel;


/**
 * @brief Виджет прогресса
 */
class ProgressWidget : public QLightBoxWidget
{
	Q_OBJECT

public:
	explicit ProgressWidget(QWidget* _parent = 0, bool _followToHeadWidget = true);

	/**
	 * @brief Показать виджет с заданными заголовком и описание
	 */
	void showProgress(const QString& _title, const QString& _description);

	/**
	 * @brief Обновить текст отображаемый в виджете
	 */
	void setProgressText(const QString& _title, const QString& _description);

	/**
	 * @brief Скрыть виджет и просигнализировать пользователю, о том, что операция завершилась
	 */
	void finish();

private:
	/**
	 * @brief Заголовок
	 */
	QLabel* m_title;

	/**
	 * @brief Описание
	 */
	QLabel* m_description;
};

#endif // PROGRESSWIDGET_H

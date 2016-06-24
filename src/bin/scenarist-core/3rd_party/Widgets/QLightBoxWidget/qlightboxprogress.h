#ifndef QLIGHTBOXPROGRESS_H
#define QLIGHTBOXPROGRESS_H

#include "qlightboxwidget.h"

class QLabel;


/**
 * @brief Виджет прогресса
 */
class QLightBoxProgress : public QLightBoxWidget
{
	Q_OBJECT

public:
	explicit QLightBoxProgress(QWidget* _parent, bool _folowToHeadWidget = true);
	~QLightBoxProgress();

	/**
	 * @brief Показать виджет с заданными заголовком и описание
	 */
	void showProgress(const QString& _title, const QString& _description);

	/**
	 * @brief Обновить текст отображаемый в виджете
	 */
	void setProgressText(const QString& _title, const QString& _description);

	/**
	 * @brief Установить значение прогресса
	 * @note Если значение меньше нуля, или больше ста, то значение прогресса не отображается
	 */
	static void setProgressValue(int _value);

	/**
	 * @brief Скрыть виджет и просигнализировать пользователю, о том, что операция завершилась
	 */
	void finish();

private:
	/**
	 * @brief Последний использовавшийся виджет прогресса
	 */
	static QLightBoxProgress* s_lastUsedWidget;

	/**
	 * @brief Заголовок
	 */
	QLabel* m_title;

	/**
	 * @brief Описание
	 */
	QLabel* m_description;
};

#endif // QLIGHTBOXPROGRESS_H

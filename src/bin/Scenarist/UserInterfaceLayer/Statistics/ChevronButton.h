#ifndef CHEVRONBUTTON_H
#define CHEVRONBUTTON_H

#include <QRadioButton>


/**
 * @brief Кнопка с шевроном
 */
class ChevronButton : public QRadioButton
{
	Q_OBJECT

public:
	explicit ChevronButton(QWidget* _parent = 0);
	explicit ChevronButton(const QString& _text, QWidget* _parent = 0);

protected:
	/**
	 * @brief Переопределяется для прорисовки шеврона
	 */
	void paintEvent(QPaintEvent* _event);

private:
	/**
	 * @brief Настроить виджет
	 */
	void init();
};

#endif // CHEVRONBUTTON_H

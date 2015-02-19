#ifndef FLATBUTTON_H
#define FLATBUTTON_H

#include <QToolButton>


/**
 * @brief Класс кнопки, которая имеет 3 иконки, для заданных состояний кнопки
 */
class FlatButton : public QToolButton
{
	Q_OBJECT

public:
	FlatButton(QWidget* _parent = 0);

	/**
	 * @brief Установить иконки
	 */
	void setIcons(const QIcon& _icon, const QIcon& _hoverIcon = QIcon(),
		const QIcon& _checkedIcon = QIcon());

protected:
	/**
	 * @brief Переопределяются для обновления иконки
	 */
	/** @{ */
	void enterEvent(QEvent* _event);
	void leaveEvent(QEvent* _event);
	/** @} */

private slots:
	/**
	 * @brief Обновить значёк
	 */
	void aboutUpdateIcon();

private:
	/**
	 * @brief Базовая иконка
	 */
	QIcon m_icon;

	/**
	 * @brief Иконка для состояния "курсор над кнопкой"
	 */
	QIcon m_hoverIcon;

	/**
	 * @brief Иконка для состояния "включёно"
	 */
	QIcon m_checkedIcon;
};

#endif // FLATBUTTON_H

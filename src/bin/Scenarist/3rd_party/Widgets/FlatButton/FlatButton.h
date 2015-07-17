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
	void setIcons(const QIcon& _icon, const QIcon& _checkedIcon = QIcon(),
		const QIcon& _hoverIcon = QIcon());

	/**
	 * @brief Обновить иконки
	 * @note Основное использование, когда кнопки созданы в дизайнере и иконки назначены там же
	 */
	void updateIcons();

protected:
	/**
	 * @brief Переопределяем для обновления цвета иконки, при смене палитры
	 */
	bool event(QEvent* _event);

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

	/**
	 * @brief Флаг того, что иконка с состоянии "включено" просто окрашивается в цвет выделения
	 */
	bool m_checkedIconHighlight;
};

#endif // FLATBUTTON_H

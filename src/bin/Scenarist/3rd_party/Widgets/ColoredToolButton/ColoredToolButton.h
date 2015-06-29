#ifndef COLOREDTOOLBUTTON_H
#define COLOREDTOOLBUTTON_H

#include <QToolButton>

class ColorsPane;


/**
 * @brief Кнопка с выбором цвета и окрашиваемой иконкой
 */
class ColoredToolButton : public QToolButton
{
	Q_OBJECT

public:
	/**
	 * @brief Виды цветовых панелей
	 */
	enum ColorsPaneType {
		NonePane,
		Google,
		WordHighlight
	};

public:
	ColoredToolButton(const QIcon& _icon, QWidget* _parent = 0);
	~ColoredToolButton();

	/**
	 * @brief Установить цветовую панель
	 */
	void setColorsPane(ColorsPaneType _pane);

public slots:
	/**
	 * @brief Установить цвет
	 */
	void setColor(const QColor& _color);

signals:
	/**
	 * @brief Кнопка нажата
	 */
	void clicked(const QColor& _color);

protected:
	/**
	 * @brief Переопределяем для обновления цвета иконки, при смене палитры
	 */
	bool event(QEvent* _event);

private slots:
	/**
	 * @brief Раскрасить иконку
	 */
	void aboutUpdateIcon(const QColor& _color);

	/**
	 * @brief Обработка нажатия на кнопку
	 */
	void aboutClicked();

private:
	/**
	 * @brief Иконка
	 */
	QIcon m_icon;

	/**
	 * @brief Используется ли "цвет по-умолчанию"
	 * @note Если да, то цвет иконки задаётся как цвет текста из палитры виджета
	 */
	bool m_defaultColor;

	/**
	 * @brief Цветовая палитра
	 */
	ColorsPane* m_colorsPane;
};

#endif // COLOREDTOOLBUTTON_H

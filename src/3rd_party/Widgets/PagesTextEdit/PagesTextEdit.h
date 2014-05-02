#ifndef PAGESTEXTEDIT_H
#define PAGESTEXTEDIT_H

#include <QTextEdit>

class PagesTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit PagesTextEdit(QWidget* _parent = 0);

	/**
	 * @brief Установить размер страницы текста
	 */
	void setPageSize(int _charsInLine, int _linesInPage);

	/**
	 * @brief Получить режим отображения текста
	 */
	bool usePageMode() const;

public slots:
	/**
	 * @brief Установить режим отображения текста
	 */
	void setUsePageMode(bool _use);

protected:
	/**
	 * @brief Переопределяется для корректировки документа и прорисовки оформления страниц
	 */
	void paintEvent(QPaintEvent* _event);

private:
	/**
	 * @brief Обновить внутреннюю геометрию
	 */
	void updateInnerGeometry();

	/**
	 * @brief Нарисовать оформление страниц документа
	 */
	void paintPagesView();

private:
	/**
	 * @brief Режим отображения текста
	 *
	 * true - постраничный
	 * false - сплошной
	 */
	bool m_usePageMode;

	/**
	 * @brief Количество символов в строке
	 */
	int m_charsInLine;

	/**
	 * @brief Количество строк на странице
	 */
	int m_linesInPage;
};

#endif // PAGESTEXTEDIT_H

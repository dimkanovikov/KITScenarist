#ifndef SIMPLETEXTEDITOR_H
#define SIMPLETEXTEDITOR_H

#include <QTextEdit>


/**
 * @brief Простейший редактор текста
 */
class SimpleTextEditor : public QTextEdit
{
	Q_OBJECT

public:
	explicit SimpleTextEditor(QWidget *parent = 0);

protected:
	/**
	 * @brief Переопределяется для добавления пунктов форматирования текста
	 */
	void contextMenuEvent(QContextMenuEvent* _event);

	/**
	 * @brief Переопределяется для реализации увеличения/уменьшения текста
	 */
	void wheelEvent(QWheelEvent* _event);

private slots:
	void textBold();
	void textUnderline();
	void textItalic();

	void currentCharFormatChanged(const QTextCharFormat &format);

private:
	void setupMenu();
	void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

	/**
	 * @brief Установить масштабирование
	 */
	void setZoomRange(int _zoomRange);

private:
	QAction* actionTextBold;
	QAction* actionTextUnderline;
	QAction* actionTextItalic;

	int m_zoomRange;

	/**
	 * @brief Синхронизация масштабирования всех редакторов данного типа
	 */
	static QList<SimpleTextEditor*> s_editors;
};

#endif // SIMPLETEXTEDITOR_H

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
	void contextMenuEvent(QContextMenuEvent* _event);

private slots:
	void textBold();
	void textUnderline();
	void textItalic();

	void currentCharFormatChanged(const QTextCharFormat &format);

private:
	void setupMenu();
	void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

private:
	QAction* actionTextBold;
	QAction* actionTextUnderline;
	QAction* actionTextItalic;

};

#endif // SIMPLETEXTEDITOR_H

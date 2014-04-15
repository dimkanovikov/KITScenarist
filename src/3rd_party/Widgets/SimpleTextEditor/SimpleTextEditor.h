#ifndef SIMPLETEXTEDITOR_H
#define SIMPLETEXTEDITOR_H

#include <QWidget>

class QTextCharFormat;
class QTextEdit;
class QToolBar;


/**
 * @brief Простейший редактор текста
 */
class SimpleTextEditor : public QWidget
{
	Q_OBJECT
public:
	explicit SimpleTextEditor(QWidget *parent = 0);

signals:

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

	QToolBar *tb;
	QTextEdit *textEdit;

};

#endif // SIMPLETEXTEDITOR_H

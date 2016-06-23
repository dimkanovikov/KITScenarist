#ifndef SIMPLETEXTEDITOR_H
#define SIMPLETEXTEDITOR_H

#include <3rd_party/Widgets/PagesTextEdit/PageTextEdit.h>

class QGestureEvent;


/**
 * @brief Простейший редактор текста
 */
class SimpleTextEditor : public PageTextEdit
{
	Q_OBJECT

public:
	explicit SimpleTextEditor(QWidget *parent = 0);
	~SimpleTextEditor();

protected:
	/**
	 * @brief Переопределяем для обработки жестов
	 */
	bool event(QEvent* _event);

	/**
	 * @brief Переопределяется для добавления пунктов форматирования текста
	 */
	void contextMenuEvent(QContextMenuEvent* _event);

	/**
	 * @brief Переопределяется для реализации увеличения/уменьшения текста
	 */
	void wheelEvent(QWheelEvent* _event);

	/**
	 * @brief Обрабатываем жест увеличения масштаба
	 */
	void gestureEvent(QGestureEvent* _event);

	/**
	 * @brief Вставляется только простой текст
	 */
	void insertFromMimeData(const QMimeData* _source);

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
	 * @brief Инерционный тормоз масштабирования при помощи жестов
	 */
	int m_gestureZoomInertionBreak;

	/**
	 * @brief Синхронизация масштабирования всех редакторов данного типа
	 */
	static QList<SimpleTextEditor*> s_editors;
};

#endif // SIMPLETEXTEDITOR_H

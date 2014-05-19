#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QFrame>

class QPushButton;
class QLineEdit;
class QTextEdit;
class QLabel;


/**
 * @brief Виджет - поисковая строка для текстового документа
 */
class SearchWidget : public QFrame
{
	Q_OBJECT

public:
	explicit SearchWidget(QWidget* _parent = 0);

	/**
	 * @brief Установить документ для поиска
	 */
	void setEditor(QTextEdit* _editor);

private slots:
	/**
	 * @brief Перейти к следующему совпадению
	 */
	void aboutFindNext();

	/**
	 * @brief Перейти к предыдущему совпадению
	 */
	void aboutFindPrev();

private:
	/**
	 * @brief Поиск текста в заданном направлении
	 */
	void findText(bool _backward);

private:
	/**
	 * @brief Редактор текста, в котором производится поиск
	 */
	QTextEdit* m_editor;

	/**
	 * @brief Перейти к предыдущему совпадению
	 */
	QPushButton* m_prevMatch;

	/**
	 * @brief Перейти к следующему совпадению
	 */
	QPushButton* m_nextMatch;

	/**
	 * @brief Поле для ввода искомого текста
	 */
	QLineEdit* m_searchText;

	/**
	 * @brief Последний искомый текст
	 */
	QString m_lastSearchText;
};

#endif // SEARCHWIDGET_H

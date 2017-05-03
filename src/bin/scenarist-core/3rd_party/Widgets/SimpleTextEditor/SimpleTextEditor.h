#ifndef SIMPLETEXTEDITOR_H
#define SIMPLETEXTEDITOR_H

#include <3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h>


/**
 * @brief Простейший редактор текста
 */
class SimpleTextEditor : public SpellCheckTextEdit
{
	Q_OBJECT

public:
	explicit SimpleTextEditor(QWidget *parent = 0);

	/**
	 * @brief Методы настройки формата
	 */
	/** @{ */
	void setTextBold(bool _bold);
	void setTextUnderline(bool _underline);
	void setTextItalic(bool _italic);
	void setTextColor(const QColor& _color);
	void setTextBackgroundColor(const QColor& _color);
    void setTextFont(const QFont& _font);
	/** @} */

protected:
	/**
	 * @brief Добавляем отдельный тип для собственных данных
	 */
	QMimeData* createMimeDataFromSelection() const;

	/**
	 * @brief Вставляется только простой текст
	 */
	void insertFromMimeData(const QMimeData* _source);

private:
	/**
	 * @brief Объединить новое форматированные с выделенным текстом, или со словом под курсором
	 */
	void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    /**
     * @brief Скорректировать межстрочный интервал
     */
    void correctLineSpacing(int _from, int _removed, int _added);

	/**
	 * @brief Даём виджету доступ к защищённым членам класса
	 */
	friend class SimpleTextEditorWidget;
};

#endif // SIMPLETEXTEDITOR_H

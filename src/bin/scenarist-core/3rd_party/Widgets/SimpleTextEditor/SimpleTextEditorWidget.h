#ifndef SIMPLETEXTEDITORWIDGET_H
#define SIMPLETEXTEDITORWIDGET_H

#include <QPageSize>
#include <QWidget>

#include <3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.h>

class ColoredToolButton;
class FlatButton;
class SimpleTextEditor;
class ScalableWrapper;
class QComboBox;
class QLabel;


class SimpleTextEditorWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Включить/выключить проверку правописания
     */
    static void enableSpellCheck(bool _enable, SpellChecker::Language _language = SpellChecker::Undefined);

public:
    explicit SimpleTextEditorWidget(QWidget *parent = 0);
    ~SimpleTextEditorWidget();

    /**
     * @brief Показать/скрыть панель инструментов редактора
     */
    void setToolbarVisible(bool _visible);

    /**
     * @brief Установить режим только чтения
     */
    void setReadOnly(bool _readOnly);

    /**
     * @brief Использовать ли постраничный режим в редакторе
     */
    void setUsePageMode(bool _use);

    /**
     * @brief Задать параметры страницы
     */
    void setPageSettings(QPageSize::PageSizeId _pageSize, const QMarginsF& _margins, Qt::Alignment _numberingAlign);

    /**
     * @brief Пуст ли документ в редакторе
     */
    bool isEmpty() const;

    /**
     * @brief Получить текст редактора
     */
    /** @{ */
    QString toHtml() const;
    QString toPlainText() const;
    /** @} */

    /**
     * @brief Установить текст в редактор
     */
    /** @{ */
    void setHtml(const QString& _html);
    void setPlainText(const QString& _text);
    /** @} */

    /**
     * @brief Очистить редактор текста
     */
    void clear();

    /**
     * @brief Редактор текста
     */
    SimpleTextEditor* editor() const;

signals:
    /**
     * @brief Изменился текст
     */
    void textChanged();

private:
    /**
     * @brief Настроить представление
     */
    void initView();

    /**
     * @brief Настроить соединения
     */
    void initConnections();

    /**
     * @brief Настроить внешний вид
     */
    void initStyleSheet();

private:
    /**
     * @brief Собственно редактор текста
     */
    SimpleTextEditor* m_editor;

    /**
     * @brief Обёртка редактора, позволяющая его масштабировать
     */
    ScalableWrapper* m_editorWrapper;

    /**
     * @brief Панель инструментов редактора текста
     */
    QWidget* m_toolbar;

    /**
     * @brief Выпадающий список шрифтов
     */
    QComboBox* m_textFont;

    /**
     * @brief Выпадающий список размера шрифта
     */
    QComboBox* m_textFontSize;

    /**
     * @brief Кнопки смены начертания
     */
    /** @{ */
    FlatButton* m_textBold;
    FlatButton* m_textItalic;
    FlatButton* m_textUnderline;
    /** @} */

    /**
     * @brief Цвет текста
     */
    ColoredToolButton* m_textColor;

    /**
     * @brief Цвет фона
     */
    ColoredToolButton* m_textBackgroundColor;

    /**
     * @brief Виджет заполняющий пустое понстранство на панели инструментов
     */
    QLabel* m_toolbarSpace;

    /**
     * @brief Обновляется ли панель инструментов в данный момент
     */
    bool m_isInTextFormatUpdate;
};

#endif // SIMPLETEXTEDITORWIDGET_H

#ifndef SCENARIOTEXTEDITWIDGET_H
#define SCENARIOTEXTEDITWIDGET_H

#include <QFrame>

class FlatButton;
class QComboBox;
class QLabel;
class QMenu;
class QTextCursor;
class ScalableWrapper;
class SearchWidget;

namespace BusinessLogic {
    class ScenarioTextDocument;
}

namespace UserInterface
{
    class ScenarioTextEdit;
    class ScenarioFastFormatWidget;
    class ScenarioReviewPanel;
    class ScenarioReviewView;
    class ScriptZenModeControls;


    /**
     * @brief Виджет расширенного редактора текста сценария
     */
    class ScenarioTextEditWidget : public QFrame
    {
        Q_OBJECT

    public:
        explicit ScenarioTextEditWidget(QWidget* _parent = 0);

        /**
         * @brief Получить панель инструментов
         */
        QWidget* toolbar() const;

        /**
         * @brief Получить текущий редактируемый документ
         */
        BusinessLogic::ScenarioTextDocument* scenarioDocument() const;

        /**
         * @brief Установить документ для редактирования
         */
        void setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document, bool _isDraft = false);

        /**
         * @brief Установить хронометраж
         */
        void setDuration(const QString& _duration);

        /**
         * @brief Установить значения счётчиков
         */
        void setCountersInfo(const QStringList& _counters);

        /**
         * @brief Включить/выключить отображение номеров сцен
         */
        void setShowScenesNumbers(bool _show);

        /**
         * @brief Задать префикс номеров сцен
         */
        void setSceneNumbersPrefix(const QString& _prefix);

        /**
         * @brief Включить/выключить отображение номеров реплик
         */
        void setShowDialoguesNumbers(bool _show);

        /**
         * @brief Установить значение необходимости подсвечивать блоки
         */
        void setHighlightBlocks(bool _highlight);

        /**
         * @brief Установить значение необходимости подсвечивать текущую строку
         */
        void setHighlightCurrentLine(bool _highlight);

        /**
         * @brief Установить необходимость автоматических замен
         */
        void setAutoReplacing(bool _capitalizeFirstWord, bool _correctDoubleCapitals,
            bool _replaceThreeDots, bool _smartQuotes);

        /**
         * @brief Включить/выключить постраничное отображение
         */
        void setUsePageView(bool _use);

        /**
         * @brief Включить/выключить проверку правописания
         */
        void setUseSpellChecker(bool _use);

        /**
         * @brief Показывать ли автодополнения в пустых блоках
         */
        void setShowSuggestionsInEmptyBlocks(bool _show);

        /**
         * @brief Установить язык проверки орфографии
         */
        void setSpellCheckLanguage(int _language);

        /**
         * @brief Настроить цвета текстового редактора
         */
        void setTextEditColors(const QColor& _textColor, const QColor& _backgroundColor);

        /**
         * @brief Настроить масштабирование редактора сценария
         */
        void setTextEditZoomRange(qreal _zoomRange);

        /**
         * @brief Зафиксированы ли сцены
         */
        void setFixed(bool _fixed);

        /**
         * @brief Получить текущую позицию курсора
         */
        int cursorPosition() const;

        /**
         * @brief Установить позицию курсора
         */
        void setCursorPosition(int _position, bool _isReset = false, bool _forceScroll = true);

        /**
         * @brief Установить тип текущего блока
         */
        void setCurrentBlockType(int _type);

        /**
         * @brief Добавить элемент в указанной позиции с заданным текстом и типом
         */
        void addItem(int _position, int _type, const QString& _name, const QString& _header,
            const QString& _description, const QColor& _color);

        /**
         * @brief Изменить элемент в указанной позиции с заданным текстом и типом
         */
        void editItem(int _startPosition, int _type, const QString& _name, const QString& _header,
            const QString& _colors);

        /**
         * @brief Удалить текст в заданном интервале
         */
        void removeText(int _from, int _to);

        /**
         * @brief Обновить все элементы связанные с перечнем стилей блоков стиля сценария
         */
        void updateStylesElements();

        /**
         * @brief Обновить горячие клавиши смены блоков
         */
        void updateShortcuts();

        /**
         * @brief Установить список дополнительных курсоров для отрисовки
         */
        void setAdditionalCursors(const QMap<QString, int>& _cursors);

        /**
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

        /**
         * @brief Пролистать сценарий, чтобы был виден заданный курсор соавтора
         */
        void scrollToAdditionalCursor(int _additionalCursorIndex);

#ifdef Q_OS_MAC
        /**
         * @brief Сформировать меню "Правка"
         */
        void buildEditMenu(QMenu* _menu);
#endif

        /**
         * @brief Подготовить интерфейс к поиску
         */
        void prepareToSearch();

        /**
         * @brief Показать/скрыть поле поиска
         */
        void aboutShowSearch();

        /**
         * @brief Показать/скрыть виджет быстрого форматирования
         */
        void aboutShowFastFormat();

        /**
         * @brief Включить/выключить дзен режим
         */
        void setZenMode(bool _isZen);

    signals:
        /**
         * @brief Запрос на отмену последнего действия
         */
        void undoRequest();

        /**
         * @brief Запрос на повтор последнего действия
         */
        void redoRequest();

        /**
         * @brief Запрос на блокировку/разблокировку номеров сцен
         */
        void changeSceneNumbersLockingRequest();

        /**
         * @brief Изменился режим отображения сценария
         */
        void textModeChanged();

        /**
         * @brief Изменился текст сценария
         */
        void textChanged();

        /**
         * @brief Изменилась позиция курсора
         */
        void cursorPositionChanged(int _position);

        /**
         * @brief Изменился коэффициент масштабирования текстового редактора
         */
        void zoomRangeChanged(qreal _zoomRange);

        /**
         * @brief Выйти из дзен режима
         */
        void quitFromZenMode();

        /**
         * @brief Пользователь хочет добавить закладку в заданном месте документа
         */
        void addBookmarkRequested(int _position);

        /**
         * @brief Пользователь хочет убрать закладку в заданном месте документа
         */
        void removeBookmarkRequested(int _position);

        /**
         * @brief Пользователь хочет переименовать номер сцены
         */
        void renameSceneNumberRequested(const QString& _newName, int _position);

    private slots:
        /**
         * @brief Обновить текущий режим (поэпизодник или текст)
         */
        void updateTextMode(bool _outlineMode);

        /**
         * @brief Обновить текущий стиль текста
         */
        void aboutUpdateTextStyle();

        /**
         * @brief Сменить стиль текста
         */
        void aboutChangeTextStyle();

        /**
         * @brief Обработчик изменения позиции курсора
         */
        void aboutCursorPositionChanged();

        /**
         * @brief Обработка изменения текста
         */
        void aboutTextChanged();

        /**
         * @brief Обработка изменения стиля текста
         */
        void aboutStyleChanged();

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить выпадающий список стилей в зависимости от текущего стиля
         */
        void initStylesCombo();
        void updateStylesCombo();

        /**
         * @brief Настроить соединения
         */
        void initConnections();
        void initEditorConnections();
        void removeEditorConnections();

        /**
         * @brief Настроить внешний вид
         */
        void initStyleSheet();

    private:
        /**
         * @brief Собственно редактор текста
         */
        ScenarioTextEdit* m_editor;

        /**
         * @brief Обёртка редактора, позволяющая его масштабировать
         */
        ScalableWrapper* m_editorWrapper;

        /**
         * @brief Панель инструментов
         */
        QWidget* m_toolbar;

        /**
         * @brief Кнопка включения режима поэпизодника
         */
        FlatButton* m_outline;

        /**
         * @brief Стили текста документа
         */
        QComboBox* m_textStyles;

        /**
         * @brief Кнопка отмены действия
         */
        FlatButton* m_undo;

        /**
         * @brief Кнопка повтора действия
         */
        FlatButton* m_redo;

        /**
         * @brief Кнопка панели поиска
         */
        FlatButton* m_search;

        /**
         * @brief Кнопка панели быстрого форматирования
         */
        FlatButton* m_fastFormat;

        /**
         * @brief Кнопка и панель рецензирования
         */
        ScenarioReviewPanel* m_review;

        /**
         * @brief Кнопка блокировки/разблокировки номеров сцен
         */
        FlatButton* m_lockUnlock;

        /**
         * @brief Хронометраж сценария
         */
        QLabel* m_duration;

        /**
         * @brief Счётчики сценария
         */
        QLabel* m_countersInfo;

        /**
         * @brief Виджет поиска
         */
        SearchWidget* m_searchLine;

        /**
         * @brief Виджет быстрого форматирования текста
         */
        ScenarioFastFormatWidget* m_fastFormatWidget;

        /**
         * @brief Панель со списком комментариев
         */
        ScenarioReviewView* m_reviewView;

        /**
         * @brief Виджет управления дзен режимом
         */
        ScriptZenModeControls* m_zenControls = nullptr;
    };
}

#endif // SCENARIOTEXTEDITWIDGET_H

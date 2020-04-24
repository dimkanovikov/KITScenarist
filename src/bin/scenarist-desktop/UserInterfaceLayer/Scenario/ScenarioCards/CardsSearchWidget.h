#ifndef CARDSSEARCHWIDGET_H
#define CARDSSEARCHWIDGET_H

#include <QFrame>

class QLineEdit;
class QLabel;
class QPushButton;


/**
 * @brief Виджет - поисковая строка карточек
 */
class CardsSearchWidget : public QFrame
{
    Q_OBJECT

public:
    explicit CardsSearchWidget(QWidget* _parent = nullptr);

    /**
     * @brief Выделить искомый текст
     * @note Применяется при повторном показе, чтобы можно было сразу вводить новый текст
     */
    void selectText();

signals:
    /**
     * @brief Применить заданный фильтр
     */
    void searchRequested(const QString& _text, bool _caseSensitive, bool _filterByText, bool _filterByTags);

private:
    /**
     * @brief Настроить представление
     */
    void initView();

    /**
     * @brief Настроить внешний вид
     */
    void initStyleSheet();

    /**
     * @brief Настроить соединения
     */
    void initConnections();

    /**
     * @brief Просигналить о том, что сменились параметры поиска
     */
    void notifySearchRequested();

private:
    /**
     * @brief Поле для ввода искомого текста
     */
    QLineEdit* m_searchText = nullptr;

    /**
     * @brief Искать с учётом регистра или без
     */
    QPushButton* m_caseSensitive = nullptr;

    /**
     * @brief Искать везде
     */
    QPushButton* m_searchEverywhere = nullptr;

    /**
     * @brief Искать в тексте
     */
    QPushButton* m_searchInText = nullptr;

    /**
     * @brief Искать в тэгах
     */
    QPushButton* m_searchInTags = nullptr;
};

#endif // CARDSSEARCHWIDGET_H

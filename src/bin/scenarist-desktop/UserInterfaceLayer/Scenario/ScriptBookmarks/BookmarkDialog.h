#ifndef BOOKMARKDIALOG_H
#define BOOKMARKDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

class QPushButton;

namespace Ui {
    class BookmarkDialog;
}

namespace UserInterface
{
    /**
     * @brief Представление списка закладок
     */
    class BookmarkDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit BookmarkDialog(QWidget* _parent, bool _isNew);
        ~BookmarkDialog();

        /**
         * @brief Получить текст закладки
         */
        QString bookmarkText() const;

        /**
         * @brief Установить текст закладки
         */
        void setBookmarkText(const QString& _text);

        /**
         * @brief Получить цвет закладки
         */
        QColor bookmarkColor() const;

        /**
         * @brief Установить цвет закладки
         */
        void setBookmarkColor(const QColor& _color);

    protected:
        /**
         * @brief Виджет на который нужно установить фокус при отображении
         */
        QWidget* focusedOnExec() const override;

    private:
        /**
         * @brief Настроить представление
         */
        void initView() override;

        /**
         * @brief Настроить соединения
         */
        void initConnections() override;

        /**
         * @brief Настроить внешний вид диалога
         */
        void initStyleSheet();

    private:
        /**
         * @brief Интерфейс
         */
        Ui::BookmarkDialog* m_ui = nullptr;

        /**
         * @brief Кнопка добавления
         */
        QPushButton* m_addButton = nullptr;
    };
}

#endif // BOOKMARKDIALOG_H

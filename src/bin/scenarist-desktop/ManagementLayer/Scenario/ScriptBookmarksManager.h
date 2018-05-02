#ifndef SCRIPTBOOKMARKSMANAGER_H
#define SCRIPTBOOKMARKSMANAGER_H

#include <QObject>

namespace BusinessLogic {
    class ScriptBookmarksModel;
}

namespace UserInterface {
    class ScriptBookmarks;
}


namespace ManagementLayer
{
    /**
     * @brief Управляющий списком закладок
     */
    class ScriptBookmarksManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ScriptBookmarksManager(QObject* _parent, QWidget* _parentWidget);

        QWidget* view() const;

        /**
         * @brief Установить модель закладок
         */
        void setBookmarksModel(BusinessLogic::ScriptBookmarksModel* _model);

        /**
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

        /**
         * @brief Добавить закладку
         */
        void addBookmark(int _position);

        /**
         * @brief Изменить закладку
         */
        void editBookmark(const QModelIndex& _index);

        /**
         * @brief Удалить закладку
         */
        void removeBookmark(const QModelIndex& _index);

        /**
         * @brief Выбрать закладку
         */
        /** @{ */
        void selectBookmark(const QModelIndex& _index);
        void selectBookmark(int _position);
        /** @} */

    signals:
        /**
         * @brief Запрос на добавление закладки
         */
        void addBookmarkRequested(int _position, const QString& _text, const QColor& _color);

        /**
         * @brief Пользователь хочет добавить закладку
         */
        void editBookmarkRequested(int _position, const QString& _text, const QColor& _color);

        /**
         * @brief Пользователь хочет удалить выбранную закладку
         */
        void removeBookmarkRequested(int _position);

        /**
         * @brief Пользователь выбрал закладку
         */
        void bookmarkSelected(int _position);

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения
         */
        void initConnection();

    private:
        /**
         * @brief Представление
         */
        UserInterface::ScriptBookmarks* m_view = nullptr;

        /**
         * @brief Модель закладок
         */
        BusinessLogic::ScriptBookmarksModel* m_model = nullptr;
    };
}

#endif // SCRIPTBOOKMARKSMANAGER_H

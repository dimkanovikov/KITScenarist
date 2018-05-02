#ifndef SCRIPTBOOKMARKS_H
#define SCRIPTBOOKMARKS_H

#include <QWidget>

class FlatButton;
class QAbstractItemModel;
class QLabel;
class QTreeView;


namespace UserInterface
{
    /**
     * @brief Представление списка закладок
     */
    class ScriptBookmarks : public QWidget
    {
        Q_OBJECT

    public:
        explicit ScriptBookmarks(QWidget* _parent = nullptr);

        /**
         * @brief Установить модель справочника
         */
        void setModel(QAbstractItemModel* _model);

        /**
         * @brief Установить текущую выделенную закладку
         */
        void setCurrentIndex(const QModelIndex& _index);

        /**
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

    signals:
        /**
         * @brief Пользователь хочет добавить закладку
         */
        void editBookmarkPressed(const QModelIndex& _index);

        /**
         * @brief Пользователь хочет удалить выбранную закладку
         */
        void removeBookmarkPressed(const QModelIndex& _index);

        /**
         * @brief Пользователь выбрал закладку
         */
        void bookmarkSelected(const QModelIndex& _index);

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

        /**
         * @brief Уведомить о том, что пользователь хочет изменить закладку
         */
        void notifyBookmarkEditPressed();

    private:
        /**
         * @brief Заголовок
         */
        QLabel* m_topEmptyLabel = nullptr;

        /**
         * @brief Кнопка добавления закладки
         */
        FlatButton* m_edit = nullptr;

        /**
         * @brief Кнопка удаления закладки
         */
        FlatButton* m_remove = nullptr;

        /**
         * @brief Дерево навигации
         */
        QTreeView* m_items = nullptr;
    };
}

#endif // SCRIPTBOOKMARKS_H

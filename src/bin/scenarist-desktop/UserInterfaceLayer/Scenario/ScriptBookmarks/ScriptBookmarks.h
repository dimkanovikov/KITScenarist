#ifndef SCRIPTBOOKMARKS_H
#define SCRIPTBOOKMARKS_H

#include <QWidget>

class FlatButton;
class QAbstractItemModel;
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
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

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
         * @brief Дерево навигации
         */
        QTreeView* m_items = nullptr;
    };
}

#endif // SCRIPTBOOKMARKS_H

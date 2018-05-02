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
    };
}

#endif // SCRIPTBOOKMARKSMANAGER_H

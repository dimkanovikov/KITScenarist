#ifndef TOOLSMANAGER_H
#define TOOLSMANAGER_H

#include <QObject>

namespace UserInterface {
    class ToolsView;
}


namespace ManagementLayer
{
    /**
     * @brief Управляющий инструментами
     */
    class ToolsManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ToolsManager(QObject* _parent, QWidget* _parentWidget);

        QWidget* view() const;

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

    private:
        /**
         * @brief Представление страницы инструментов
         */
        UserInterface::ToolsView* m_view = nullptr;
    };
}

#endif // TOOLSMANAGER_H

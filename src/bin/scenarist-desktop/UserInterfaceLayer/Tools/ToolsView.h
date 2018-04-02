#ifndef TOOLSVIEW_H
#define TOOLSVIEW_H

#include <QWidget>


namespace UserInterface
{
    /**
     * @brief Представление страницы инструментов
     */
    class ToolsView : public QWidget
    {
        Q_OBJECT

    public:
        explicit ToolsView(QWidget* _parent = nullptr);

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения для формы
         */
        void initConnections();

        /**
         * @brief Настроить внешний вид
         */
        void initStyleSheet();
    };
}

#endif // TOOLSVIEW_H

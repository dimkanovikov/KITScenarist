#ifndef MENUVIEW_H
#define MENUVIEW_H

#include <QWidget>

class QMenu;
class QPushButton;

namespace Ui {
    class MenuView;
}


namespace UserInterface
{
    /**
     * @brief Меню приложения
     */
    class MenuView : public QWidget
    {
        Q_OBJECT

    public:
        explicit MenuView(QWidget* _parent = nullptr);
        ~MenuView();

        /**
         * @brief Установить меню для оборачивания
         */
        void setMenu(QMenu* _menu);

        /**
         * @brief Получить обёрнутое меню
         */
        QMenu* menu() const;

        /**
         * @brief Настроить доступность заданного пункта меню
         */
        void setMenuItemEnabled(int _index, bool _enabled);

        /**
         * @brief Деактивировать доступность действий над проектом
         */
        void disableProjectActions();

        /**
         * @brief Активировать доступность действий над проектом
         */
        void enableProjectActions();

    signals:
        /**
         * @brief Запрос на скрытие меню
         */
        void hideRequested();

    protected:
        /**
         * @brief Переопределяем для обработки события смены палитры
         */
        bool event(QEvent* _event) override;

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить кнопки меню
         */
        void initMenuButtons();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

        /**
         * @brief Настроить стили
         */
        void initStyleSheet();

    private:
        /**
         * @brief Интерфейс
         */
        Ui::MenuView* m_ui = nullptr;

        /**
         * @brief Меню, которое нужно обернуть
         */
        QMenu* m_menu = nullptr;

        /**
         * @brief Список кнопок соответствующий пунктам меню
         */
        QVector<QPushButton*> m_menuButtons;
    };
}

#endif // MENUVIEW_H

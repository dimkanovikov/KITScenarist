#ifndef CARDSRESIZER_H
#define CARDSRESIZER_H

#include <QWidget>

namespace Ui {
    class CardsResizer;
}


namespace UserInterface {
    /**
     * @brief Виджет настройки размера карт
     */
    class CardsResizer : public QWidget
    {
        Q_OBJECT

    public:
        explicit CardsResizer(QWidget *parent = 0);
        ~CardsResizer();

        /**
         * @brief Размер карточки
         */
        int cardSize() const;

        /**
         * @brief Отношение сторон карточки
         */
        int cardRatio() const;

        /**
         * @brief Расстояние между карточками
         */
        int distance() const;

        /**
         * @brief Количество карточек в ряду
         */
        int cardsInRow() const;

    signals:
        /**
         * @brief Изменились параметры расположения карточек
         */
        void parametersChanged();

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
        Ui::CardsResizer* m_ui;
    };
}

#endif // CARDSRESIZER_H

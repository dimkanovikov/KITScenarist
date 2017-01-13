/*
 * Copyright (C) 2015  Dimka Novikov, to@dimkanovikov.pro
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * Full license: https://github.com/dimkanovikov/WidgetAnimationFramework/blob/master/LICENSE
 */

#ifndef SIDESLIDEANIMATOR_H
#define SIDESLIDEANIMATOR_H

#include "../../WAF.h"
#include "../../AbstractAnimator.h"

class QPropertyAnimation;


/**
 * Widgets Animation Framework
 */
namespace WAF
{
    class SideSlideBackgroundDecorator;


    /**
     * @brief Аниматор выдвижения виджета из-за стороны приложения
     */
    class SideSlideAnimator : public AbstractAnimator
    {
        Q_OBJECT

    public:
        explicit SideSlideAnimator(QWidget* _widgetForSlide);

        /**
         * @brief Установить сторону, откуда выдвигать виджет
         */
        void setApplicationSide(ApplicationSide _side);

        /**
         * @brief Использовать ли декорирование фона
         */
        void setDecorateBackground(bool _decorate);

        /**
         * @brief Длительность анимации
         */
        int animationDuration() const;

        /**
         * @brief Выдвинуть виджет
         */
        /** @{ */
        void animateForward();
        void slideIn();
        /** @} */

        /**
         * @brief Задвинуть виджет
         */
        /** @{ */
        void animateBackward();
        void slideOut();
        /** @} */

    protected:
        /**
         * @brief Переопределяется, чтобы корректировать размер выкатываемого виджета
         */
        bool eventFilter(QObject* _object, QEvent* _event);

    private:
        /**
         * @brief Получить виджет, который нужно анимировать
         */
        QWidget* widgetForSlide() const;

    private:
        /**
         * @brief Сторона из-за которой выкатывать виджет
         */
        ApplicationSide m_side;

        /**
         * @brief Необходимо ли декорировать фон
         */
        bool m_decorateBackground;

        /**
         * @brief Объект для анимирования выезжания
         */
        QPropertyAnimation* m_animation;

        /**
         * @brief Помошник затемняющий фон под выезжающим виджетом
         */
        SideSlideBackgroundDecorator* m_decorator;
    };
}

#endif // SIDESLIDEANIMATOR_H

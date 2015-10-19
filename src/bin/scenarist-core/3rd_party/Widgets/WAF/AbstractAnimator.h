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

#ifndef ABSTRACTANIMATOR
#define ABSTRACTANIMATOR

#include <QObject>

/**
 * Widgets Animation Framework
 */
namespace WAF
{
	/**
	 * @brief Абстрактный класс аниматора
	 */
	class AbstractAnimator : public QObject
    {
	public:
        explicit AbstractAnimator(QObject* _parent = 0) : QObject(_parent) {}

		/**
		 * @brief Выполнить прямую анимацию
		 */
        virtual void animateForward() = 0;

		/**
		 * @brief Выполнить обратную анимацию
		 */
        virtual void animateBackward() = 0;
	};
}

#endif // ABSTRACTANIMATOR


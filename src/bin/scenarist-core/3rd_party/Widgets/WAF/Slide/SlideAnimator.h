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

#ifndef SLIDEANIMATOR_H
#define SLIDEANIMATOR_H

#include "../AbstractAnimator.h"
#include "../Animation.h"

class QPropertyAnimation;


/**
 * Widgets Animation Framework
 */
namespace WAF
{
	class SlideBackgroundDecorator;


	/**
	 * @brief Аниматор выдвижения виджета
	 */
	class SlideAnimator : public AbstractAnimator
	{
		Q_OBJECT

	public:
		explicit SlideAnimator(QWidget* _widgetForSlide);

		/**
		 * @brief Установить направление выдвижения
		 */
		void setAnimationDirection(AnimationDirection _direction);

		/**
		 * @brief Фиксировать фон при анимации (по умолчанию фон фиксируется)
		 */
		void setFixBackground(bool _fix);

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
		 * @brief Переопределяется, чтобы корректировать позицию перекрывающего виджета
		 */
		bool eventFilter(QObject* _object, QEvent* _event);

	private:
		/**
		 * @brief Получить виджет, который нужно анимировать
		 */
		QWidget* widgetForSlide() const;

	private:
		/**
		 * @brief Направление, по которому выкатывать виджет
		 */
		AnimationDirection m_direction;

		/**
		 * @brief Фиксировать фон при анимации
		 */
		bool m_isFixBackground;

		/**
		 * @brief Объект для анимирования выезжания
		 */
		QPropertyAnimation* m_animation;

		/**
		 * @brief Помошник перекрывающий анимируемый виджет
		 */
		SlideBackgroundDecorator* m_decorator;
	};
}

#endif // SLIDEANIMATOR_H

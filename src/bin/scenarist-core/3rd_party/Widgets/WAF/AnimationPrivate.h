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

#ifndef ANIMATIONPRIVATE
#define ANIMATIONPRIVATE

#include <QMap>

class QWidget;


/**
 * Widgets Animation Framework
 */
namespace WAF
{
	class AbstractAnimator;


	/**
	 * @brief Данные фасада анимаций
	 */
	class AnimationPrivate
	{
	public:
		/**
		 * @brief Есть ли аниматор выкатывания для заданного виджета
		 */
		bool hasSideSlideAnimator(QWidget* _widget) const {
			return m_sideSlideAnimators.contains(_widget);
		}

		/**
		 * @brief Получить аниматор выкатывания для заданного виджета
		 */
		AbstractAnimator* sideSlideAnimator(QWidget* _widget) const {
			return m_sideSlideAnimators.value(_widget, 0);
		}

		/**
		 * @brief Сохранить аниматор выкатывания для заданного виджета
		 */
		void saveSideSlideAnimator(QWidget* _widget, AbstractAnimator* _animator) {
			if (!hasSideSlideAnimator(_widget)) {
				m_sideSlideAnimators.insert(_widget, _animator);
			}
		}

	private:
		/**
		 * @brief Карта аниматоров, связанных с управляемыми видежатами
		 */
		QMap<QWidget*, AbstractAnimator*> m_sideSlideAnimators;
	};
}

#endif // ANIMATIONPRIVATE


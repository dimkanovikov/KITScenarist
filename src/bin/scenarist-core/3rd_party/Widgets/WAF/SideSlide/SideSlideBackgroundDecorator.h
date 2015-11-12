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

#ifndef SIDESLIDEBACKGROUNDDECORATOR_H
#define SIDESLIDEBACKGROUNDDECORATOR_H

#include <QTimeLine>
#include <QWidget>


/**
 * Widgets Animation Framework
 */
namespace WAF
{
	/**
	 * @brief Класс декорирующий задний план анимации выкатывания
	 */
	class SideSlideBackgroundDecorator : public QWidget
	{
		Q_OBJECT

	public:
		explicit SideSlideBackgroundDecorator(QWidget* _parent);

		/**
		 * @brief Сохранить изображение родительского виджета
		 */
		void grabParent();

		/**
		 * @brief Задекорировать фон
		 */
		void decorate(bool _dark);

	signals:
		/**
		 * @brief На виджете произведён щелчёк мышью
		 */
		void clicked();

	protected:
		/**
		 * @brief Переопределяется для прорисовки декорации
		 */
		void paintEvent(QPaintEvent* _event);

		/**
		 * @brief Переопределяется для отлавливания щелчка мышью
		 */
		void mousePressEvent(QMouseEvent* _event);

	private:
		/**
		 * @brief Таймлайн для реализации анимированного декорирования
		 */
		QTimeLine m_timeline;

		/**
		 * @brief Фоновое изображение
		 */
		QPixmap m_background;

		/**
		 * @brief Цвет декорирования фона
		 */
		QColor m_decorationColor;
	};
}

#endif // SIDESLIDEBACKGROUNDDECORATOR_H

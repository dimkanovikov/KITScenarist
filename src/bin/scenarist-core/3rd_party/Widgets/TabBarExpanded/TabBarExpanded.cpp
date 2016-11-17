/*
* Copyright (C) 2016 Alexey Polushkin, armijo38@yandex.ru
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* Full license: http://dimkanovikov.pro/license/GPLv3
*/

#include "TabBarExpanded.h"

TabBarExpanded::TabBarExpanded(QWidget* _parent) : TabBar(_parent)
{

}

QSize TabBarExpanded::tabSizeHint(int _index) const
{
    if (count() != 0) {
        return QSize(width() / count(), height());
    } else {
        return TabBar::tabSizeHint(_index);
    }
}

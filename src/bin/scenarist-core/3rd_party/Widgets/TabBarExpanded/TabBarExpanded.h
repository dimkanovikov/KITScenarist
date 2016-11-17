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

#ifndef TABBAREXPANDED_H
#define TABBAREXPANDED_H

#include <3rd_party/Widgets/TabBar/TabBar.h>

class TabBarExpanded : public TabBar
{
    Q_OBJECT

public:
    explicit TabBarExpanded(QWidget* _parent = 0);

protected:
    QSize tabSizeHint(int _index) const override;
};

#endif // TABBAREXPANDED_H

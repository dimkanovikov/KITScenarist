/*
* Copyright (C) 2015 Dimka Novikov, to@dimkanovikov.pro
* Copyright (C) 2016 Alexey Polushkin, armijo38@yandex.ru
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* Full license: http://dimkanovikov.pro/license/LGPLv3
*/

#ifndef WEBLOADERGLOBAL_H
#define WEBLOADERGLOBAL_H

#include <QtCore/QtGlobal>

#if defined(WEBLOADER_LIBRARY)
# define WEBLOADER_EXPORT Q_DECL_EXPORT
#else
# define WEBLOADER_EXPORT Q_DECL_IMPORT
#endif

#endif // WEBLOADERGLOBAL_H

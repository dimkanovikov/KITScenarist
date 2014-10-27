/*
    Copyright (C)  2010  Brad Hards <bradh@frogmouth.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RTFREADER_COMMENTPCDATADESTINATION_H
#define RTFREADER_COMMENTPCDATADESTINATION_H

#include <QtCore/QString>
#include <QtGui/QColor>

#include "PcdataDestination.h"

namespace RtfReader
{
    class Reader;

    class CommentPcdataDestination: public PcdataDestination
    {
      public:
	CommentPcdataDestination( Reader *reader, AbstractRtfOutput *output, const QString &name );

	virtual ~CommentPcdataDestination();

	virtual void aboutToEndDestination();
      };
}

#endif
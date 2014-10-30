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

#ifndef RTFREADER_TOKEN_H
#define RTFREADER_TOKEN_H

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace RtfReader
{
    enum TokenType { OpenGroup, CloseGroup, Control, Plain, Binary };

    /**
      RTF token
      */
    class Token {
      public: /* TODO: convert to getters / setters */
	void dump() const;
	TokenType type;
	QString name;
	bool hasParameter;
	QString parameter;
	QByteArray binaryData;
    };
}

#endif

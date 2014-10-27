/*
    Copyright (C)  2008, 2010  Brad Hards <bradh@frogmouth.net>
 
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

#ifndef RTFREADER_DESTINATION_H
#define RTFREADER_DESTINATION_H

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QTextCharFormat>

#include "AbstractRtfOutput.h"

namespace RtfReader
{
    class Reader;

    /**
       A representation of a destination
    */
    class Destination {
    public:
        // TODO: remove reader
	Destination( Reader *reader, AbstractRtfOutput *output, const QString &name );

	virtual ~Destination();

	bool hasName( const QString &name );

	QString name() const;

	virtual void handleControlWord( const QString &controlWord, bool hasValue, const int value );

	virtual void handlePlainText( const QString &plainText );

	virtual void aboutToEndDestination();
	
	// TODO: this doesn't belong here - remove it.
	QTextCharFormat m_textCharFormat;

    protected:
	QString m_name;
	Reader *m_reader;
	AbstractRtfOutput *m_output;
    };

    QDebug operator<<(QDebug dbg, const Destination &dest);
}

#endif

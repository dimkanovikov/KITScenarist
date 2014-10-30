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

#include "Destination.h"

#include <QtCore/QStack>

namespace RtfReader
{
    Destination::Destination( Reader *reader, AbstractRtfOutput *output, const QString &name ) :
     m_name( name ), m_reader(reader), m_output( output )
    {
    }

    Destination::~Destination()
    {}

    bool Destination::hasName( const QString &name )
    {
	return m_name == name;
    }

    QString Destination::name() const
    {
	return m_name;
    }

    void Destination::handleControlWord( const QString &, bool, const int )
    {
    };

    void Destination::handlePlainText( const QString &plainText )
    {
	qDebug() << "plain text:" << plainText << "in" << m_name;
    };

    void Destination::aboutToEndDestination()
    {};
}
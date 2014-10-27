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

#include "FontTableDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
    FontTableDestination::FontTableDestination( Reader *reader, AbstractRtfOutput *output, const QString &name ) :
      Destination( reader, output, name )
    {
    }

    FontTableDestination::~FontTableDestination()
    {
    }

    void FontTableDestination::handleControlWord( const QString &controlWord, bool hasValue, const int value )
    {
	if ( controlWord == "f" ) {
	  m_currentFontTableIndex = value;
	} else if ( controlWord == "froman" ) {
	  m_fontTableEntry.setFontFamily( Roman );
	} else if ( controlWord == "fswiss" ) {
	  m_fontTableEntry.setFontFamily( Swiss );
	} else if ( controlWord == "fnil" ) {
	  m_fontTableEntry.setFontFamily( Nil );
	} else if ( controlWord == "fmodern" ) {
	  m_fontTableEntry.setFontFamily( Modern );
	} else if ( controlWord == "fscript" ) {
	  m_fontTableEntry.setFontFamily( Script );
	} else if ( controlWord == "fdecor" ) {
	  m_fontTableEntry.setFontFamily( Decor );
	} else if ( controlWord == "ftech" ) {
	  m_fontTableEntry.setFontFamily( Tech );
	} else if ( controlWord == "fbidi" ) {
	  m_fontTableEntry.setFontFamily( Bidi );
	} else if ( controlWord == "fprq" ) {
	  m_fontTableEntry.setFontPitch( static_cast<enum FontPitch>(value) );
	} else if ( controlWord == "fcharset" ) {
	  // TODO: need to figure out how to sanely handle this
	} else if ( hasValue ) {
	    qDebug() << "unhandled fonttbl control word:" << controlWord << "(" << value << ")";
	} else {
	    qDebug() << "unhandled fonttbl control word:" << controlWord << "( invalid value )";
	}
    }

    void FontTableDestination::handlePlainText( const QString &plainText )
    {
	if ( plainText == ";" ) {
	    m_output->insertFontTableEntry( m_fontTableEntry, m_currentFontTableIndex );
	} else if ( plainText.endsWith( ";" ) ) {
	    // probably a font name with a terminating delimiter
	    int delimiterPosition = plainText.indexOf( ";" );
	    if ( delimiterPosition == ( plainText.length() - 1) ) {
		// It is at the end, chop it off
		QString fontName = plainText.left( delimiterPosition );
		m_fontTableEntry.setFontName( fontName );
		m_output->insertFontTableEntry( m_fontTableEntry, m_currentFontTableIndex );
	    } else {
		// we were not expecting a name with a delimiter other than at the end
		qDebug() << "Font name with embedded delimiter: " << plainText;
	    }
	} else {
	    // plain font name
	    m_fontTableEntry.setFontName( plainText );
	}
    }

    void FontTableDestination::aboutToEndDestination()
    {
	// TODO
    }
}
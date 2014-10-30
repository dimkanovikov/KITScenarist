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

#ifndef RTFREADER_FONTTABLEENTRY_H
#define RTFREADER_FONTTABLEENTRY_H

namespace RtfReader
{
    enum FontFamily { Nil, Roman, Swiss, Modern, Script, Decor, Tech, Bidi };

    enum FontPitch { Default = 0, Fixed = 1, Variable = 2 };

    class FontTableEntry
    {
      public:	
	FontTableEntry() : m_fontFamily( Nil ), m_fontPitch( Default )
	{}

	enum FontFamily fontFamily() const
	{ return m_fontFamily; }

	void setFontFamily( enum FontFamily fontFamily )
	{ m_fontFamily = fontFamily; }

	enum FontPitch fontPitch() const
	{ return m_fontPitch; }

	void setFontPitch( enum FontPitch fontPitch )
	{ m_fontPitch = fontPitch; }

	QString fontName() const
	{ return m_fontName; }

	void setFontName( const QString &fontName )
	{ m_fontName = fontName; }

      protected:
	enum FontFamily m_fontFamily;
	enum FontPitch m_fontPitch;
	QString m_fontName;
    };
}

#endif

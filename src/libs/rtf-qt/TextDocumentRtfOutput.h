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

#ifndef RTFREADER_TEXTDOCUMENTRTFOUTPUT_H
#define RTFREADER_TEXTDOCUMENTRTFOUTPUT_H

#include "AbstractRtfOutput.h"

class QImage;
class QTextCursor;
class QTextDocument;
class QTextImageFormat;

#include <QtCore/QStack>
#include <QtGui/QTextCharFormat>

namespace RtfReader
{
    class Reader;

    class TextDocumentRtfOutput: public AbstractRtfOutput
    {
      public:
	TextDocumentRtfOutput( QTextDocument *document );

	virtual ~TextDocumentRtfOutput();

	virtual void startGroup();

	virtual void endGroup();

	virtual void appendText( const QString &text );

	virtual void insertPar();

	virtual void insertTab();

	virtual void insertLeftQuote();
	virtual void insertRightQuote();
	virtual void insertLeftDoubleQuote();
	virtual void insertRightDoubleQuote();

	virtual void insertEnDash();
	virtual void insertEmDash();

	virtual void insertEnSpace();
	virtual void insertEmSpace();

	virtual void insertBullet();

	virtual void setFontItalic( const int value );

	virtual void setFontBold( const int value );

	virtual void setFontUnderline( const int value );

	virtual void setFontPointSize( const int pointSize );

	virtual void setForegroundColour( const int colourIndex );
	virtual void setHighlightColour( const int colourIndex );
	virtual void setParagraphPatternBackgroundColour( const int colourIndex );

	virtual void setFont( const int fontIndex );

	virtual void setDefaultFont( const int fontIndex );

	virtual void setFontSuperscript();
	virtual void setFontSubscript();

	virtual void setTextDirectionLeftToRight();
	virtual void setTextDirectionRightToLeft();

	virtual void appendToColourTable( const QColor &colour );

	virtual void insertFontTableEntry( FontTableEntry fontTableEntry, quint32 fontTableIndex );
	virtual void insertStyleSheetTableEntry( quint32 stylesheetTableIndex, StyleSheetTableEntry stylesheetTableEntry );

	virtual void resetParagraphFormat();
	virtual void resetCharacterProperties();

	virtual void setParagraphAlignmentLeft();
	virtual void setParagraphAlignmentCentred();
	virtual void setParagraphAlignmentJustified();
	virtual void setParagraphAlignmentRight();

	virtual void setFirstLineIndent( const int twips );
	virtual void setLeftIndent( const int twips );
	virtual void setRightIndent( const int twips );

	virtual void createImage( const QImage &image, const QTextImageFormat &format );

	virtual void setPageHeight( const int pageHeight );
	virtual void setPageWidth( const int pageWidth );

	virtual void setSpaceBefore( const int twips );
	virtual void setSpaceAfter( const int twips );

      protected:
	// The text cursor on the document being generated
	QTextCursor *m_cursor;

	QStack<QTextCharFormat> m_textCharFormatStack;

	QTextBlockFormat m_paragraphFormat;

	QList<QColor> m_colourTable;

	QHash<int, FontTableEntry> m_fontTable;
	int m_defaultFontIndex;

	QHash<int, StyleSheetTableEntry> m_stylesheetTable;

	QTextDocument *m_document;

	bool m_haveSetFont;

	/**
	  Convenience routine to convert a size in twips into pixels
	*/
	qreal pixelsFromTwips( const int twips );

      };
}

#endif
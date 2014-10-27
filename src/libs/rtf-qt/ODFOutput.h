/*
    Copyright (C)  2011 Brad Hards <bradh@frogmouth.net>

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

#ifndef RTFREADER_ODFOUTPUT_H
#define RTFREADER_ODFOUTPUT_H

#include "AbstractRtfOutput.h"

#include <QtCore/QString>

class QuaZip;
class QuaZipFile;
class QXmlStreamWriter;

namespace RtfReader
{
    struct ManifestEntry
    {
	QString fullPath;
	QString mediaType;
    };

    class ODFOutput: public AbstractRtfOutput
    {
      public:
	ODFOutput( const QString &odfFileName );

	virtual ~ODFOutput();

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
	virtual void resetParagraphFormat();
	virtual void setParagraphAlignmentLeft();
	virtual void setParagraphAlignmentCentred();
	virtual void setParagraphAlignmentJustified();
	virtual void setParagraphAlignmentRight();
	virtual void setFirstLineIndent( const int twips );
	virtual void setLeftIndent( const int twips );
	virtual void setRightIndent( const int twips );
	virtual void setFontItalic( const int value );
	virtual void setFontBold( const int value );
	virtual void setFontUnderline( const int value );
	virtual void setFontPointSize( const int value );
	virtual void setFontSuperscript();
	virtual void setFontSubscript();
	virtual void setForegroundColour( const int value );
	virtual void setHighlightColour( const int value );
	virtual void setParagraphPatternBackgroundColour( const int value );
	virtual void setFont( const int fontIndex );
	virtual void setDefaultFont( const int fontIndex );
	virtual void setTextDirectionLeftToRight();
	virtual void setTextDirectionRightToLeft();
	virtual void resetCharacterProperties();
	virtual void createImage( const QImage &image, const QTextImageFormat &format);
	virtual void setPageHeight( const int pageHeight );
	virtual void setPageWidth( const int pageWidth );
	virtual void setSpaceBefore( const int value );
	virtual void setSpaceAfter( const int value );
	virtual void appendToColourTable( const QColor &colour );
	virtual void insertFontTableEntry( FontTableEntry fontTableEntry, quint32 fontTableIndex );
	virtual void insertStyleSheetTableEntry( quint32 stylesheetTableIndex, StyleSheetTableEntry stylesheetTableEntry );

      protected:
	void addManifestEntry( const QString &fullPath, const QString &mediaType );
	void writeManifestFile();
	void writeMetadataElement( QXmlStreamWriter *xmlStream, const QString &nameSpace, const QString &element, const QString &value );
	void writeKeywordsMetadata( QXmlStreamWriter *xmlStream, const QString &keywords );
	void writeEditingTimeMetadata( QXmlStreamWriter *xmlStream, const int editMinutes );
	void writeMetadataElementDateTime( QXmlStreamWriter *xmlStream, const QString &nameSpace, const QString &element, const QDateTime &value );
	void writeMetadataFile();

	QuaZip				*m_odfZipFile;
	QuaZipFile			*m_content;
	QXmlStreamWriter		*m_contentXML;
	QuaZipFile			*m_settings;
	QXmlStreamWriter		*m_settingsXML;
	QuaZipFile			*m_styles;
	QXmlStreamWriter		*m_stylesXML;
	
	bool				m_haveOpenTextParagraph;
	QList<struct ManifestEntry>	m_manifestEntries;
    };
}

#endif
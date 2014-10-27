/*
    Copyright (C)  2011  Brad Hards <bradh@frogmouth.net>

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

#include "ODFOutput.h"

#include "quazip.h"
#include "quazipfile.h"

#include <QtCore/QDebug>
#include <QtCore/QXmlStreamWriter>

static const QString officeNamespace( "urn:oasis:names:tc:opendocument:xmlns:office:1.0" );
static const QString textNamespace( "urn:oasis:names:tc:opendocument:xmlns:text:1.0" );
static const QString manifestNamespace( "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" );
static const QString configNamespace( "urn:oasis:names:tc:opendocument:xmlns:config:1.0" );
static const QString metaNamespace( "urn:oasis:names:tc:opendocument:xmlns:meta:1.0" );
static const QString dcNamespace( "http://purl.org/dc/elements/1.1/" );

// static const QString rtfSettingsNamespace( "http://www.frogmouth.net/RtfReaderDraft0.0" );

static const QString mimetype( "application/vnd.oasis.opendocument.text" );

namespace RtfReader
{
    ODFOutput::ODFOutput( const QString &odfFileName ) : AbstractRtfOutput(),
	m_haveOpenTextParagraph( false )
    {
	m_odfZipFile = new QuaZip( odfFileName );
	m_odfZipFile->open( QuaZip::mdCreate );
	addManifestEntry( "/", "application/vnd.oasis.opendocument.text" );

	QuaZipFile mimetypeFile( m_odfZipFile );
	QuaZipNewInfo mimetypeInfo( "mimetype" );
	mimetypeInfo.externalAttr = ( 0644 << 16 );
	/* we should be able to store here, not DEFLATE */
	mimetypeFile.open( QIODevice::WriteOnly, mimetypeInfo );
	mimetypeFile.write( mimetype.toLocal8Bit() );
	mimetypeFile.close();

	m_styles = new QuaZipFile( m_odfZipFile );
	QuaZipNewInfo stylesInfo( "styles.xml" );
	stylesInfo.externalAttr = ( 0644 << 16 );
	m_styles->open( QIODevice::WriteOnly, stylesInfo );
	m_stylesXML = new QXmlStreamWriter( m_styles );
	m_stylesXML->setAutoFormatting( true );
	m_stylesXML->writeNamespace( officeNamespace, QString( "office" ) );
	m_stylesXML->writeStartDocument();
	m_stylesXML->writeStartElement( officeNamespace, QString( "document-styles" ) );
	m_stylesXML->writeAttribute( officeNamespace, QString( "version" ), QString( "1.1" ) );
	m_stylesXML->writeStartElement( officeNamespace, QString( "automatic-styles" ) );
	m_stylesXML->writeEndElement(); // closes "automatic-styles"
	m_stylesXML->writeStartElement( officeNamespace, QString( "master-styles" ) );
	m_stylesXML->writeEndElement(); // closes "master-styles"
	m_stylesXML->writeEndDocument();
	qDebug() << "result at styles.xml file level:" << m_styles->getZipError();
	m_styles->close();
	addManifestEntry( "styles.xml", "text/xml" );

	m_settings = new QuaZipFile( m_odfZipFile );
	QuaZipNewInfo settingsInfo( "settings.xml" );
	settingsInfo.externalAttr = ( 0644 << 16 );
	m_settings->open( QIODevice::WriteOnly, settingsInfo );
	m_settingsXML = new QXmlStreamWriter( m_settings );
	m_settingsXML->setAutoFormatting( true );
	m_settingsXML->writeNamespace( officeNamespace, QString( "office" ) );
	m_settingsXML->writeNamespace( configNamespace, QString( "config" ) );
	m_settingsXML->writeStartDocument();
	m_settingsXML->writeStartElement( officeNamespace, QString( "document-settings" ) );
	m_settingsXML->writeAttribute( officeNamespace, QString( "version" ), QString( "1.1" ) );
	m_settingsXML->writeEndElement(); // closes "document-settings"
	m_settingsXML->writeEndDocument();
	qDebug() << "result at settings.xml file level:" << m_settings->getZipError();
	m_settings->close();
	addManifestEntry( "settings.xml", "text/xml" );

	m_content = new QuaZipFile( m_odfZipFile );
	QuaZipNewInfo contentInfo( "content.xml" );
	contentInfo.externalAttr = ( 0644 << 16 );
	m_content->open( QIODevice::WriteOnly, contentInfo );
	m_contentXML = new QXmlStreamWriter( m_content );
	m_contentXML->setAutoFormatting( true );
	m_contentXML->writeNamespace( officeNamespace, QString( "office" ) );
	m_contentXML->writeNamespace( textNamespace, QString( "text" ) );
	m_contentXML->writeStartDocument();
	m_contentXML->writeStartElement( officeNamespace, QString( "document-content" ) );
	m_contentXML->writeAttribute( officeNamespace, QString( "version" ), QString( "1.1" ) );
	m_contentXML->writeStartElement( officeNamespace, QString( "body" ) );
	m_contentXML->writeStartElement( officeNamespace, QString( "text" ) );
    }

    ODFOutput::~ODFOutput()
    {
	m_contentXML->writeEndElement(); // closes "text"
	m_contentXML->writeEndElement(); // closes "body"
	m_contentXML->writeEndElement(); // closes "document-content"
	m_contentXML->writeEndDocument();
	qDebug() << "result at content.xml file level:" << m_content->getZipError();
	m_content->close();
	addManifestEntry( "content.xml", "text/xml" );

	writeMetadataFile();

	writeManifestFile();

	m_odfZipFile->close();
	qDebug() << "result at zip file level:" << m_odfZipFile->getZipError();
	delete m_odfZipFile;
    }

    void ODFOutput::writeMetadataElement( QXmlStreamWriter *xmlStream, const QString &nameSpace, const QString &element, const QString &value )
    {
	if ( value.isEmpty() ) {
	    return;
	}
	xmlStream->writeTextElement( nameSpace, element, value );
    }

    void ODFOutput::writeMetadataElementDateTime( QXmlStreamWriter *xmlStream, const QString &nameSpace, const QString &element, const QDateTime &value )
    {
	if ( ! value.isValid() ) {
	    return;
	}
	xmlStream->writeTextElement( nameSpace, element, value.toString( Qt::ISODate ) );
    }
    
    void ODFOutput::writeKeywordsMetadata( QXmlStreamWriter *xmlStream, const QString &keywords )
    {
	QStringList keywordList = keywords.split(",", QString::SkipEmptyParts);
	for ( int i = 0; i < keywordList.count(); ++i ) {
	    writeMetadataElement( xmlStream, metaNamespace, QString( "keyword" ), keywordList.at( i ).simplified() );
	}
    }
    
    void ODFOutput::writeEditingTimeMetadata( QXmlStreamWriter *xmlStream, const int editMinutes )
    {
	if ( editMinutes == 0 ) {
	    return;
	}
	int residualHours = editMinutes / 60;
	int minutes = editMinutes - ( 60 * residualHours );
	int days = residualHours / 24;
	int hours = residualHours - ( 24 * days );
	QString formattedDate = QString( "P%1DT%2H%3M0S" ).arg( days ).arg( hours ).arg( minutes );
	writeMetadataElement(xmlStream, metaNamespace, QString( "editing-duration" ), formattedDate );
    }

    void ODFOutput::writeMetadataFile()
    {
	QuaZipFile *meta = new QuaZipFile( m_odfZipFile );

	QuaZipNewInfo metaInfo( "meta.xml" );
	metaInfo.externalAttr = ( 0644 << 16 );
	meta->open( QIODevice::WriteOnly, metaInfo );

	QXmlStreamWriter *metaXML = new QXmlStreamWriter( meta );
	metaXML->setAutoFormatting( true );
	metaXML->writeNamespace( officeNamespace, QString( "office" ) );
	metaXML->writeNamespace( metaNamespace, QString( "meta" ) );
	metaXML->writeNamespace( dcNamespace, QString( "dc" ) );
	metaXML->writeStartDocument();
	metaXML->writeStartElement( officeNamespace, QString( "document-meta" ) );
	metaXML->writeAttribute( officeNamespace, QString( "version" ), QString( "1.1" ) );
	metaXML->writeStartElement( officeNamespace, QString( "meta" ) );
	writeMetadataElement( metaXML, metaNamespace, QString( "generator" ), generatorInformation() );
	writeMetadataElement( metaXML, dcNamespace, QString( "title" ), title() );
	writeMetadataElement( metaXML, dcNamespace, QString( "description" ), documentComment() );
	writeMetadataElement( metaXML, dcNamespace, QString( "subject" ), subject() );
	writeMetadataElement( metaXML, metaNamespace, QString( "initial-creator" ), author() );
	writeMetadataElement( metaXML, dcNamespace, QString( "creator" ), operatorName() );
	writeMetadataElementDateTime( metaXML, metaNamespace, QString( "creation-date" ), created() );
	writeMetadataElementDateTime( metaXML, dcNamespace, QString( "date" ), revised() );
	writeMetadataElementDateTime( metaXML, metaNamespace, QString( "print-date" ), printed() );
	writeMetadataElement( metaXML, metaNamespace, QString( "editing-cycles" ), QString::number( versionNumber() ) );
	writeEditingTimeMetadata( metaXML, totalEditingTime() );
	writeKeywordsMetadata( metaXML, keywords() );
	metaXML->writeEndElement(); // closes "meta"
	metaXML->writeEndElement(); // closes "master-styles"
	metaXML->writeEndDocument();
	qDebug() << "result at meta.xml file level:" << meta->getZipError();
	meta->close();
	addManifestEntry( "meta.xml", "text/xml" );
    }
 
    void ODFOutput::addManifestEntry( const QString &fullPath, const QString &mediaType )
    {
	ManifestEntry entry;
	entry.fullPath = fullPath;
	entry.mediaType = mediaType;
	m_manifestEntries.append( entry );
    }
    
    void ODFOutput::writeManifestFile()
    {
	QuaZipFile manifestFile( m_odfZipFile );
	QuaZipNewInfo manifestInfo( "META-INF/manifest.xml" );
	manifestInfo.externalAttr = ( 0644 << 16 );
	manifestFile.open( QIODevice::WriteOnly, manifestInfo );
	QXmlStreamWriter manifestXML( &manifestFile );
	manifestXML.setAutoFormatting( true );
	manifestXML.writeNamespace( manifestNamespace, QString( "manifest" ) );
	manifestXML.writeStartDocument();
	manifestXML.writeStartElement( manifestNamespace, QString( "manifest" ) );
	for (int i = 0; i < m_manifestEntries.count(); ++i) {
	    manifestXML.writeStartElement( manifestNamespace, QString( "file-entry" ) );
	    manifestXML.writeAttribute( manifestNamespace, QString( "media-type" ), m_manifestEntries[i].mediaType );
	    manifestXML.writeAttribute( manifestNamespace, QString( "full-path" ), m_manifestEntries[i].fullPath );
	    manifestXML.writeEndElement(); // closes "file-entry"
	}
	manifestXML.writeEndElement(); // closes "manifest" for outer scope
	manifestXML.writeEndDocument();
	qDebug() << "result at manifest.xml file level:" << manifestFile.getZipError();
	manifestFile.close();
    }

    void ODFOutput::appendText( const QString &text )
    {
	if ( ! m_haveOpenTextParagraph ) {
	    m_contentXML->writeStartElement( textNamespace, QString( "p" ) );
	    m_haveOpenTextParagraph = true;
	}
	m_contentXML->writeCharacters( text );
    }

    void ODFOutput::insertPar()
    {
        if ( ! m_haveOpenTextParagraph ) {
	    m_contentXML->writeStartElement( textNamespace, QString( "p" ) );
	}
	m_contentXML->writeEndElement();
	m_haveOpenTextParagraph = false;
    }

    void ODFOutput::insertTab()
    {
        /* TODO: implement this */
    }

    void ODFOutput::insertLeftQuote()
    {
        appendText( QString( QChar( 0x2018 ) ) );
    }

    void ODFOutput::insertRightQuote()
    {
        appendText( QString( QChar( 0x2019 ) ) );
    }

    void ODFOutput::insertLeftDoubleQuote()
    {
	// TODO: figure out why this doesn't render properly
        appendText( QString( QChar( 0x201c) ) );
    }

    void ODFOutput::insertRightDoubleQuote()
    {
	// TODO: figure out why this doesn't render properly
	appendText( QString( QChar( 0x201d ) ) );
    }

    void ODFOutput::insertEnDash()
    {
        appendText( QString( QChar( 0x2013 ) ) );
    }

    void ODFOutput::insertEmDash()
    {
        appendText( QString( QChar( 0x2014 ) ) );
    }

    void ODFOutput::insertEnSpace()
    {
        appendText( QString( QChar( 0x2002 ) ) );
    }

    void ODFOutput::insertEmSpace()
    {
        appendText( QString( QChar( 0x2003 ) ) );
    }

    void ODFOutput::insertBullet()
    {
        appendText( QString( QChar( 0x2022 ) ) );
    }

    void ODFOutput::resetParagraphFormat()
    {
        /* TODO: implement this */
    }

    void ODFOutput::setParagraphAlignmentLeft()
    {
        /* TODO: implement this */
    }

    void ODFOutput::setParagraphAlignmentCentred()
    {
        /* TODO: implement this */
    }

    void ODFOutput::setParagraphAlignmentJustified()
    {
        /* TODO: implement this */
    }

    void ODFOutput::setParagraphAlignmentRight()
    {
        /* TODO: implement this */
    }

    void ODFOutput::setFirstLineIndent( const int twips )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setLeftIndent( const int twips )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setRightIndent( const int twips )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setFontItalic( const int value )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setFontBold( const int value )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setFontUnderline( const int value )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setFontPointSize( const int value )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setFontSuperscript()
    {
        /* TODO: implement this */
    }

    void ODFOutput::setFontSubscript()
    {
        /* TODO: implement this */
    }

    void ODFOutput::setForegroundColour( const int value )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setHighlightColour( const int value )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setParagraphPatternBackgroundColour( const int value )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setFont( const int fontIndex )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setDefaultFont( const int fontIndex )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setTextDirectionLeftToRight()
    {
        /* TODO: implement this */
    }

    void ODFOutput::setTextDirectionRightToLeft()
    {
        /* TODO: implement this */
    }

    void ODFOutput::resetCharacterProperties()
    {
        /* TODO: implement this */
    }

    void ODFOutput::createImage( const QImage &image, const QTextImageFormat &format)
    {
        /* TODO: implement this */
    }

    void ODFOutput::setPageHeight( const int pageHeight )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setPageWidth( const int pageWidth )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setSpaceBefore( const int value )
    {
        /* TODO: implement this */
    }

    void ODFOutput::setSpaceAfter( const int value )
    {
        /* TODO: implement this */
    }

    void ODFOutput::appendToColourTable( const QColor &colour )
    {
        /* TODO: implement this */
    }

    void ODFOutput::insertFontTableEntry( FontTableEntry fontTableEntry, quint32 fontTableIndex )
    {
        /* TODO: implement this */
    }

    void ODFOutput::insertStyleSheetTableEntry( quint32 stylesheetTableIndex, StyleSheetTableEntry stylesheetTableEntry )
    {
        /* TODO: implement this */
    }

}

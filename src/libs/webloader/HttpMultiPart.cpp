#include "HttpMultiPart.h"
#include "QFreeDesktopMime/freedesktopmime.h"

#include <QtCore/QStringList>
#include <QtCore/QFile>


HttpPart::HttpPart( HttpPartType type ) :
	m_type( type )
{

}

HttpPart::HttpPartType HttpPart::type() const
{
	return m_type;
}

void HttpPart::setText( const QString name, const QString value )
{
	setName( name );
	setValue( value );
}

void HttpPart::setFile( const QString name, const QString filePath )
{
	setName( name );
	setFilePath( filePath );
}

QString HttpPart::name() const
{
	return m_name;
}

QString HttpPart::value() const
{
	return m_value;
}

QString HttpPart::fileName() const
{
	return value();
}

QString HttpPart::filePath() const
{
	return m_filePath;
}



void HttpPart::setName( const QString name )
{
	if ( m_name != name )
		m_name = name;
}

void HttpPart::setValue( const QString value )
{
	if ( m_value != value )
		m_value = value;
}

void HttpPart::setFileName( const QString fileName )
{
	setValue( fileName );
}

void HttpPart::setFilePath( const QString filePath )
{
	if ( m_filePath != filePath ) {
		m_filePath = filePath;
		QString fileName = filePath.split('/').last();
		setFileName( fileName );
	}
}





HttpMultiPart::HttpMultiPart()
{
}

void HttpMultiPart::setBoundary( const QString boundary )
{
	if ( m_boundary != boundary )
		m_boundary = boundary;
}

void HttpMultiPart::addPart( HttpPart part )
{
	m_parts.append( part );
}

QByteArray HttpMultiPart::data()
{
	QByteArray multiPartData;
	foreach ( HttpPart httpPart, parts() ) {
		QByteArray partData = makeDataFromPart( httpPart );
		multiPartData.append( partData );
	}
	// Добавление отметки о завершении данных
	{
		QByteArray endData = makeEndData();
		multiPartData.append( endData );
	}
	return multiPartData;
}

QByteArray HttpMultiPart::makeDataFromPart( HttpPart part )
{
	QByteArray partData;
	switch ( part.type() ) {
	case HttpPart::Text: {
		partData = makeDataFromTextPart( part );
		break;
	}
	case HttpPart::File: {
		partData = makeDataFromFilePart( part );
		break;
	}
	}
	return partData;
}

QByteArray HttpMultiPart::makeDataFromTextPart( HttpPart part )
{
	QByteArray partData;

	partData.append( "--" );
	partData.append( boundary() );
	partData.append( crlf() );

	partData.append(
				QString( "Content-Disposition: form-data; name=\"%1\"%3%3%2" )
				.arg( part.name(), part.value(), crlf() )
				);

	partData.append( crlf() );

	return partData;
}

QByteArray HttpMultiPart::makeDataFromFilePart(HttpPart part)
{
	QByteArray partData;

	partData.append( "--" );
	partData.append( boundary() );
	partData.append( crlf() );

	{
		QFile uploadFile( part.filePath() );
		uploadFile.open( QIODevice::ReadOnly );
		// Определение mime типа файла
		QFreeDesktopMime mimeTypeDetector;
		QString contentType = mimeTypeDetector.fromFile( &uploadFile );
		uploadFile.seek( 0 ); // Несколько байт были считаны

		partData.append(
					QString( "Content-Disposition: form-data; name=\"%1\"; filename=\"%2\"%4"
							 "Content-Type: %3%4%4"
							 )
					.arg( part.name(),
						  part.fileName(),
						  contentType,
						  crlf() )
					);

		while ( !uploadFile.atEnd() ) {
			QByteArray readed = uploadFile.read(1024);
			partData.append( readed );
		}
		uploadFile.close();
	}

	partData.append( crlf() );
	return partData;
}

QByteArray HttpMultiPart::makeEndData()
{
	QByteArray partData;

	partData.append( "--" );
	partData.append(boundary());
	partData.append( "--" );
	partData.append( crlf() );

	return partData;
}

QString HttpMultiPart::boundary() const
{
	return m_boundary;
}

QString HttpMultiPart::crlf() const
{
	QString crlf;
	crlf = 0x0d;
	crlf += 0x0a;
	return crlf;
}

QList<HttpPart> HttpMultiPart::parts() const
{
	return m_parts;
}

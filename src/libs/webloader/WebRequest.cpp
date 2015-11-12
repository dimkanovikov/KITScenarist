#include "WebRequest.h"
#include "HttpMultiPart.h"


#include <QFile>
#include <QStringList>
#include <QSslConfiguration>

//! Заголовки запроса
const QByteArray USER_AGENT_HEADER = "User-Agent";
const QByteArray REFERER_HEADER = "Referer";
//! Параметры запроса
// UserAgent запроса
const QByteArray USER_AGENT = "Web Robot v.0.12.1";
// Boundary для разделения атрибутов запроса, при использовании  multi-part form data
const QString BOUNDARY = "---------------------------7d935033608e2";
// ContentType запроса
const QString CONTENT_TYPE_DEFAULT = "application/x-www-form-urlencoded";
const QString CONTENT_TYPE = "multipart/form-data; boundary=" + BOUNDARY;


WebRequest::WebRequest()
{

}

WebRequest::~WebRequest()
{

}

QUrl WebRequest::urlToLoad() const
{
	return m_urlToLoad;
}

void WebRequest::setUrlToLoad( QUrl url )
{
	if ( urlToLoad() != url )
		m_urlToLoad = url;
}

QUrl WebRequest::urlReferer() const
{
	return m_urlReferer;
}

void WebRequest::setUrlReferer( QUrl url )
{
	if ( urlReferer() != url )
		m_urlReferer = url;
}

void WebRequest::clearAttributes()
{
	m_attributes.clear();
	m_attributeFiles.clear();
}

void WebRequest::addAttribute( QString name, QVariant value )
{
	QPair< QString, QVariant > attribute;
	attribute.first = name;
	attribute.second = value;
	addAttribute( attribute );
}

void WebRequest::addAttributeFile( QString name, QString filePath )
{
	QPair< QString, QString > attributeFile;
	attributeFile.first = name;
	attributeFile.second = filePath;
	addAttributeFile( attributeFile );
}
#include <QDebug>
QNetworkRequest WebRequest::networkRequest( bool addContentHeaders )
{
	QNetworkRequest request( urlToLoad() );
	// Установка заголовков запроса
	// User-Agent
	request.setRawHeader( USER_AGENT_HEADER, USER_AGENT );
	// Referer
	if ( !urlReferer().isEmpty() )
		request.setRawHeader( REFERER_HEADER, urlReferer().toString().toUtf8().data() );
	// ContentType по-умолчанию
	request.setHeader( QNetworkRequest::ContentTypeHeader, CONTENT_TYPE_DEFAULT );

	if ( addContentHeaders ) {
		// ContentType
		request.setHeader( QNetworkRequest::ContentTypeHeader, CONTENT_TYPE );
		// ContentLength
		request.setHeader( QNetworkRequest::ContentLengthHeader, multiPartData().size() );
	}

	return request;
}

QByteArray WebRequest::multiPartData()
{
	HttpMultiPart multiPart;
	multiPart.setBoundary( BOUNDARY );

	// Добавление текстовых атрибутов
	QPair< QString, QVariant > attribute;
	foreach ( attribute, attributes() ) {
		QString attributeName  = attribute.first;
		QString attributeValue = attribute.second.toString();

		HttpPart textPart( HttpPart::Text );
		textPart.setText( attributeName, attributeValue );

		multiPart.addPart( textPart );
	}

	// Добавление атрибутов-файлов
	QPair< QString, QString > attributeFile;
	foreach ( attributeFile, attributeFiles() ) {
		QString attributeName     = attributeFile.first;
		QString attributeFilePath = attributeFile.second;

		HttpPart filePart( HttpPart::File );
		filePart.setFile( attributeName, attributeFilePath );

		multiPart.addPart( filePart );
	}

	return multiPart.data();
}


//*****************************************************************************
// Методы доступа к данным класса, а так же вспомогательные
// методы для работы с данными класса

QList<QPair<QString, QVariant> > WebRequest::attributes() const
{
	return m_attributes;
}

void WebRequest::addAttribute( QPair<QString, QVariant> attribute )
{
	if ( !attributes().contains( attribute ) )
		m_attributes.append( attribute );
}

QList<QPair<QString, QString> > WebRequest::attributeFiles() const
{
	return m_attributeFiles;
}

void WebRequest::addAttributeFile( QPair<QString, QString> attributeFile )
{
	if ( !attributeFiles().contains( attributeFile ) )
		m_attributeFiles.append( attributeFile );
}

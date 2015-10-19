#ifndef HTTPMULTIPART_H
#define HTTPMULTIPART_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QList>

class HttpPart
{
public:
	enum HttpPartType {
		Text,
		File
	};

public:
	HttpPart( HttpPartType type = Text );
	HttpPartType type() const;
	void setText( const QString name, const QString value );
	void setFile( const QString name, const QString filePath );

public:
	QString name() const;
	QString value() const;
	QString fileName() const;
	QString filePath() const;


private:
	void setName( const QString name );
	void setValue( const QString value );
	void setFileName( const QString fileName );
	void setFilePath( const QString filePath );

private:
	HttpPartType m_type;
	QString m_name,
			m_value,
			m_filePath;
};

class HttpMultiPart
{
public:
	HttpMultiPart();
	void setBoundary(const QString boundary);
	void addPart( HttpPart part );

	QByteArray data();

private:
	QByteArray makeDataFromPart( HttpPart part );
	QByteArray makeDataFromTextPart( HttpPart part );
	QByteArray makeDataFromFilePart( HttpPart part );
	QByteArray makeEndData();

private:
	QString boundary() const;
	QString crlf() const;
	QList<HttpPart> parts() const;

private:
	QString m_boundary;
	QList<HttpPart> m_parts;
};

#endif // HTTPMULTIPART_H

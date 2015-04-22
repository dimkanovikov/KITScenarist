#include "WebLoader.h"

#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHttpMultiPart>
#include <QtCore/QTimer>
#include <QEventLoop>

// не все сайты передают суммарный размер загружаемой страницы,
// поэтому для отображения прогресса загрузки используется
// заранее заданное число (средний размер веб-страницы)
const int POSSIBLE_RECIEVED_MAX_FILE_SIZE = 120000;


WebLoader::WebLoader( QObject * parent, QNetworkCookieJar * jar ) :
	QThread( parent ),
	m_networkManager( 0 ),
	m_cookieJar( jar ),
	m_request( 0 ),
	m_requestMethod( Undefined ),
	m_isNeedRedirect( true )
{

}

WebLoader::~WebLoader()
{
	if ( m_request )
		delete m_request;
	if ( m_networkManager )
		m_networkManager->deleteLater();//delete m_networkManager;//
}

void WebLoader::setCookieJar( QNetworkCookieJar *jar )
{
	if ( cookieJar() != jar )
		m_cookieJar = jar;
}

void WebLoader::setRequestMethod( WebLoader::RequestMethod method )
{
	if ( requestMethod() != method )
		m_requestMethod = method;
}

void WebLoader::addRequestAttribute( QString name, QVariant value )
{
	request()->addAttribute( name, value );
}

void WebLoader::addRequestAttributeFile( QString name, QString filePath )
{
	request()->addAttributeFile( name, filePath );
}

void WebLoader::loadAsync( QUrl urlToLoad, QUrl referer )
{
	request()->setUrlToLoad( urlToLoad );
	request()->setUrlReferer  ( referer );

	start();
}

QByteArray WebLoader::loadSync(QUrl urlToLoad, QUrl referer)
{
	request()->setUrlToLoad(urlToLoad);
	request()->setUrlReferer(referer);

	QEventLoop loop;
	connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
	start();
	loop.exec();

	return downloadedData();
}

QUrl WebLoader::url() const
{
	return m_request->urlToLoad();
}


//*****************************************************************************
// Внутренняя реализация класса

void WebLoader::run()
{
	initNetworkManager();

	do
	{
		//! Начало загрузки страницы m_request->url()
		emit uploadProgress( 0 );
		emit downloadProgress( 0 );

		QNetworkReply *reply;

		switch ( requestMethod() ) {

			default:
			case WebLoader::Get: {
				QNetworkRequest request = this->request()->networkRequest();
				reply = networkManager()->get( request );
				break;
			}

			case WebLoader::Post: {
				QNetworkRequest networkRequest = request()->networkRequest( true );
				QByteArray data = request()->multiPartData();
				reply = networkManager()->post( networkRequest, data );
				break;
			}

		} // switch

		connect( reply, SIGNAL(uploadProgress(qint64,qint64)),
				 this,    SLOT(uploadProgress(qint64,qint64)) );
		connect( reply, SIGNAL(downloadProgress(qint64,qint64)),
				 this,    SLOT(downloadProgress(qint64,qint64)) );
		connect( reply, SIGNAL(error(QNetworkReply::NetworkError)),
				 this,    SLOT(downloadError(QNetworkReply::NetworkError)) );
		connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
				this, SLOT(downloadSslErrors(QList<QSslError>)));
		connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
				reply, SLOT(ignoreSslErrors()));


		exec(); // входим в поток обработки событий, ожидая завершения отработки networkManager'а

	} while ( isNeedRedirect() );

	emit downloadComplete( downloadedData() );
	emit downloadComplete( QString( downloadedData() ) );
}

void WebLoader::uploadProgress( qint64 uploadedBytes, qint64 totalBytes )
{
	//! отправлено [uploaded] байт из [total]
	if (totalBytes > 0)
		emit uploadProgress( ((float)uploadedBytes / totalBytes) * 100 );
}

void WebLoader::downloadProgress( qint64 recievedBytes, qint64 totalBytes )
{
	//! загружено [recieved] байт из [total]
	// не все сайты передают суммарный размер загружаемой страницы,
	// поэтому для отображения прогресса загрузки используется
	// заранее заданное число (средний размер веб-страницы)
	if (totalBytes < 0)
		totalBytes = POSSIBLE_RECIEVED_MAX_FILE_SIZE;
	emit downloadProgress( ((float)recievedBytes / totalBytes) * 100 );
}
#include <QDebug>
void WebLoader::downloadComplete( QNetworkReply * reply )
{
	//! Завершена загрузка страницы [request()->url()]

	// требуется ли редирект?
	if ( !reply->header( QNetworkRequest::LocationHeader ).isNull() ) {
		//! Осуществляется редирект по ссылке [redirectUrl]
		// Referer'ом становится ссылка по хоторой был осуществлен запрос
		QUrl refererUrl = request()->urlToLoad();
		request()->setUrlReferer( refererUrl );
		// Получаем ссылку для загрузки из заголовка ответа [Loacation]
		QUrl redirectUrl = reply->header( QNetworkRequest::LocationHeader ).toUrl();
		request()->setUrlToLoad( redirectUrl );
		setRequestMethod( WebLoader::Get ); // Редирект всегда методом Get
		setIsNeedRedirect( true );
	} else {
		//! Загружены данные [reply->bytesAvailable()]
		qint64 downloadedDataSize = reply->bytesAvailable();
		QByteArray downloadedData = reply->read( downloadedDataSize );
		setDownloadedData( downloadedData );
		reply->deleteLater();
		setIsNeedRedirect( false );
	}

	if (!isRunning()) {
		wait(100);
	}
	quit(); // прерываем цикл обработки событий потока ( возвращаемся в run() )
}

namespace {
	static QString networkErrorToString(QNetworkReply::NetworkError networkError) {
		QString result;
		switch (networkError) {
			case QNetworkReply::ConnectionRefusedError: result = "the remote server refused the connection (the server is not accepting requests)"; break;
			case QNetworkReply::RemoteHostClosedError: result = "the remote server closed the connection prematurely, before the entire reply was received and processed"; break;
			case QNetworkReply::HostNotFoundError: result = "the remote host name was not found (invalid hostname)"; break;
			case QNetworkReply::TimeoutError: result = "the connection to the remote server timed out"; break;
			case QNetworkReply::OperationCanceledError: result = "the operation was canceled via calls to abort() or close() before it was finished."; break;
			case QNetworkReply::SslHandshakeFailedError: result = "the SSL/TLS handshake failed and the encrypted channel could not be established. The sslErrors() signal should have been emitted."; break;
			case QNetworkReply::TemporaryNetworkFailureError: result = "the connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established."; break;
			case QNetworkReply::NetworkSessionFailedError: result = "the connection was broken due to disconnection from the network or failure to start the network."; break;
			case QNetworkReply::BackgroundRequestNotAllowedError: result = "the background request is not currently allowed due to platform policy."; break;
			case QNetworkReply::ProxyConnectionRefusedError: result = "the connection to the proxy server was refused (the proxy server is not accepting requests)"; break;
			case QNetworkReply::ProxyConnectionClosedError: result = "the proxy server closed the connection prematurely, before the entire reply was received and processed"; break;
			case QNetworkReply::ProxyNotFoundError: result = "the proxy host name was not found (invalid proxy hostname)"; break;
			case QNetworkReply::ProxyTimeoutError: result = "the connection to the proxy timed out or the proxy did not reply in time to the request sent"; break;
			case QNetworkReply::ProxyAuthenticationRequiredError: result = "the proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)"; break;
			case QNetworkReply::ContentAccessDenied: result = "the access to the remote content was denied (similar to HTTP error 401)"; break;
			case QNetworkReply::ContentOperationNotPermittedError: result = "the operation requested on the remote content is not permitted"; break;
			case QNetworkReply::ContentNotFoundError: result = "the remote content was not found at the server (similar to HTTP error 404)"; break;
			case QNetworkReply::AuthenticationRequiredError: result = "the remote server requires authentication to serve the content but the credentials provided were not accepted (if any)"; break;
			case QNetworkReply::ContentReSendError: result = "the request needed to be sent again, but this failed for example because the upload data could not be read a second time."; break;
			case QNetworkReply::ContentConflictError: result = "the request could not be completed due to a conflict with the current state of the resource."; break;
			case QNetworkReply::ContentGoneError: result = "the requested resource is no longer available at the server."; break;
			case QNetworkReply::InternalServerError: result = "the server encountered an unexpected condition which prevented it from fulfilling the request."; break;
			case QNetworkReply::OperationNotImplementedError: result = "the server does not support the functionality required to fulfill the request."; break;
			case QNetworkReply::ServiceUnavailableError: result = "the server is unable to handle the request at this time."; break;
			case QNetworkReply::ProtocolUnknownError: result = "the Network Access API cannot honor the request because the protocol is not known"; break;
			case QNetworkReply::ProtocolInvalidOperationError: result = "the requested operation is invalid for this protocol"; break;
			case QNetworkReply::UnknownNetworkError: result = "an unknown network-related error was detected"; break;
			case QNetworkReply::UnknownProxyError: result = "an unknown proxy-related error was detected"; break;
			case QNetworkReply::UnknownContentError: result = "an unknown error related to the remote content was detected"; break;
			case QNetworkReply::ProtocolFailure: result = "a breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.)"; break;
			case QNetworkReply::UnknownServerError: result = "an unknown error related to the server response was detected"; break;
			case QNetworkReply::NoError: result = "No error";
		}

		return result;
	}


}

void WebLoader::downloadError( QNetworkReply::NetworkError networkError )
{
	switch ( networkError ) {

		case QNetworkReply::NoError:
			//! Загрузка прошла без ошибок
			break;
		default:
			setLastError( QString( tr( "Sorry, we have some error while loading. Error is: %1") )
						  .arg(::networkErrorToString(networkError))
						  );
			emit error( lastError() );
			break;

	}
}

void WebLoader::downloadSslErrors(const QList<QSslError>& _errors)
{
	QString fullError;
	foreach (const QSslError& error, _errors) {
		if (!fullError.isEmpty()) {
			fullError.append("\n");
		}
		fullError.append(error.errorString());
	}

	setLastErrorDetails(fullError);
}


//*****************************************************************************
// Методы доступа к данным класса, а так же вспомогательные
// методы для работы с данными класса

QNetworkAccessManager * WebLoader::networkManager()
{
	if ( !m_networkManager )
		m_networkManager = new QNetworkAccessManager();
	return m_networkManager;
}

void WebLoader::initNetworkManager()
{
	// параметры
	if ( cookieJar() ) {
		networkManager()->setCookieJar( cookieJar() );
		cookieJar()->setParent( 0 );
	}
	// содинения
	connect( networkManager(), SIGNAL(finished(QNetworkReply*)),
			 this,    SLOT(downloadComplete(QNetworkReply*)) );
}

QNetworkCookieJar * WebLoader::cookieJar() const
{
	return m_cookieJar;
}

WebRequest * WebLoader::request()
{
	if ( !m_request )
		m_request = new WebRequest();
	return m_request;
}

WebLoader::RequestMethod WebLoader::requestMethod() const
{
	return m_requestMethod;
}

bool WebLoader::isNeedRedirect() const
{
	return m_isNeedRedirect;
}

void WebLoader::setIsNeedRedirect( bool isNeedRedirect )
{
	if ( m_isNeedRedirect != isNeedRedirect )
		m_isNeedRedirect = isNeedRedirect;
}

QByteArray WebLoader::downloadedData() const
{
	return m_downloadedData;
}

void WebLoader::setDownloadedData( QByteArray data )
{
	m_downloadedData = data;
}

QString WebLoader::lastError() const
{
	return m_lastError;
}

QString WebLoader::lastErrorDetails() const
{
	return m_lastErrorDetails;
}

void WebLoader::setLastError(QString errorText)
{
	if ( m_lastError != errorText )
		m_lastError = errorText;
}

void WebLoader::setLastErrorDetails(const QString& _details)
{
	if (m_lastErrorDetails != _details) {
		m_lastErrorDetails = _details;
	}
}

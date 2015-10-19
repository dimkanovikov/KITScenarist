#ifndef WEBLOADER_H
#define WEBLOADER_H

#include "WebLoaderGlobal.h"
#include "WebRequest.h"

#include <QtCore/QThread>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;
class QNetworkCookieJar;


/*!
  \class WebLoader

  \brief Класс для работы с http-протоколом
  */
class WEBLOADER_EXPORT WebLoader : public QThread
{
	Q_OBJECT

public:
	/*!
	  \enum Метод запроса
	  */
	enum RequestMethod {
		Undefined, /*!< Метод не установлен */
		Get,
		Post
	};

public:
	/*!
	  \fn Конструктор
	  */
	explicit WebLoader(QObject * _parent = 0, QNetworkCookieJar * _jar = 0 );
	/*!
	  \fn Деструктор
	  */
	virtual ~WebLoader();

	/*!
	  \fn Установка куков для сессии загрузчика
	  \param cookie - коллекция куков
	  */
	void setCookieJar( QNetworkCookieJar * cookieJar );
	/*!
	  \fn Установка метода запроса
	  \param method - метод запроса
	  */
	void setRequestMethod( RequestMethod method );
	/*!
	 * \fn Очистить все старые атрибуты запроса
	 */
	void clearRequestAttributes();
	/*!
	  \fn Добавление атрибута в запрос
	  \param name   - имя атрибута
	  \param value  - значение атрибута
	  */
	void addRequestAttribute( QString name,
							  QVariant value);
	/*!
	  \fn Добавление файла в запрос
	  \param name     - имя атрибута
	  \param filePath - путь к файлу
	  */
	void addRequestAttributeFile( QString name,
								  QString filePath );
	/*!
	  \fn Отправка запроса (асинхронное выполнение)
	  \param urlToLoad - ссылка для запроса
	  \param referer   - реферальная ссылка
	  */
	void loadAsync( QUrl urlToLoad, QUrl referer = QUrl() );

	/*!
	  \fn Отправка запроса (синхронное выполнение)
	  \param urlToLoad - ссылка для запроса
	  \param referer   - реферальная ссылка
	  */
	QByteArray loadSync( QUrl urlToLoad, QUrl referer = QUrl() );

	QUrl url() const;
	QString lastError() const;
	QString lastErrorDetails() const;

signals:
	/*!
	  \fn Прогресс отправки запроса на сервер
	  \param Процент отправленных данных
	  */
	void uploadProgress( int );
	/*!
	  \fn Прогресс загрузки данных с сервера
	  \param Процент загруженных данных
	  */
	void downloadProgress( int );
	/*!
	  \fn Данные загружены
	  \param Загруженные данные
	  */
	void downloadComplete( QByteArray );
	void downloadComplete( QString );
	/*!
	  \fn Сигнал об ошибке
	  \param Текст ошибки
	  */
	void error( QString );


//*****************************************************************************
// Внутренняя реализация класса

private:
	/*!
	  \fn Работа потока
	  */
	void run();

private slots:
	/*!
	  \fn Прогресс отправки запроса на сервер
	  \param uploadedBytes - отправлено байт
	  \param totalBytes - байт к отправке
	  */
	void uploadProgress( qint64 uploadedBytes, qint64 totalBytes );
	/*!
	  \fn Прогресс загрузки данных с сервера
	  \param recievedBytes загружено байт
	  \param totalBytes - байт к отправке
	  */
	void downloadProgress( qint64 recievedBytes, qint64 totalBytes );
	/*!
	  \fn Окончание загрузки страницы
	  \param reply - ответ сервера
	  */
	void downloadComplete( QNetworkReply * reply );
	/*!
	  \fn Ошибка при загрузки страницы
	  \param networkError - ошибка
	  */
	void downloadError( QNetworkReply::NetworkError networkError );
	/*!
	 * \fn Ошибки при защищённом подключении
	 * \param Список ошибок
	 */
	void downloadSslErrors(const QList<QSslError>& _errors);


//*****************************************************************************
// Методы доступа к данным класса, а так же вспомогательные
// методы для работы с данными класса
private:
	void initNetworkManager();

// Данные класса
private:
	QNetworkAccessManager * m_networkManager;
	QNetworkCookieJar * m_cookieJar;
	WebRequest * m_request;
	RequestMethod m_requestMethod;
	bool m_isNeedRedirect;
	QByteArray m_downloadedData;
	QString m_lastError;
	QString m_lastErrorDetails;
};

#endif // WEBLOADER_H

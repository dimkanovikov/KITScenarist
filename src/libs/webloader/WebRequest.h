#ifndef WEBREQUEST_H
#define WEBREQUEST_H

#include <QString>
#include <QVariant>
#include <QNetworkRequest>


/*!
  \class WebLoader

  \brief Класс обертка для QNetworkRequest
  */
class WebRequest
{
public:
	/*!
	  \fn Конструктор
	  */
	WebRequest();
	~WebRequest();

	/*!
	  \fn Ссылка запроса
	  */
	QUrl urlToLoad() const;
	/*!
	  \fn Установка ссылки для запроса
	  \param url - ссылка
	  */
	void setUrlToLoad( QUrl url );

	/*!
	  \fn Ссылка referer
	  */
	QUrl urlReferer() const;
	/*!
	  \fn Установка ссылки referer'а
	  \param url - ссылка
	  */
	void setUrlReferer( QUrl url );

	/*!
	  \fn Добавление текстового атрибута в запрос
	  \param name - название атрибута
	  \param value - значение атрибута
	  */
	void addAttribute( QString name, QVariant value );
	/*!
	  \fn Добавление атрибута-файла в запрос
	  \param name - название атрибута
	  \param filePath - путь к файлу
	  */
	void addAttributeFile( QString name, QString filePath);

	/*!
	  \fn Сформированный объект класса QNetworkRequest
	  */
	QNetworkRequest networkRequest( bool addContentHeaders = false );

	/*!
	  \fn Атрибуты запроса
	  */
	QByteArray  multiPartData();

//*****************************************************************************
// Внутренняя реализация класса

private:
	/*!
	  \fn Текстовые атрибуты запроса
	  */
	QList< QPair< QString, QVariant > > attributes() const;
	/*!
	  \fn Добавление текстового атрибута в запрос
	  \param attribute - имя + значения атрибута
	  */
	void addAttribute( QPair< QString, QVariant > attribute );
	/*!
	  \fn Атрибуты-файлы запроса
	  */
	QList<QPair<QString, QString> > attributeFiles() const;
	/*!
	  \fn Добавление атрибута-файла в запрос
	  \param attributeFile - имя атрибута + путь к файлу
	  */
	void addAttributeFile( QPair<QString, QString> attributeFile );

private:
	QUrl m_urlToLoad,
		 m_urlReferer;
	QList< QPair< QString, QVariant > > m_attributes;
	QList< QPair< QString, QString > >  m_attributeFiles;
};

#endif // WEBREQUEST_H

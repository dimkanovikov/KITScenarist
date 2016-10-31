#ifndef FILE_SAVE_XML_H
#define FILE_SAVE_XML_H

#include <QString>
#include <QGraphicsView>


class FileErrorException
{
public:
	FileErrorException (const QString &filename) : _filename(filename) {}
	QString filename() const { return _filename; }
private:
	QString _filename;
};


void fileSaveXml (const QString &_filename, QGraphicsView *_view);
QString createSceneXml (QGraphicsScene *_scene, QGraphicsView *_view = NULL);

#endif // FILE_SAVE_XML_H

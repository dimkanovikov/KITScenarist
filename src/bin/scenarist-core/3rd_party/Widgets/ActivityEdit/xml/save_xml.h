#ifndef FILE_SAVE_XML_H
#define FILE_SAVE_XML_H

class QGraphicsScene;
class QGraphicsView;
class QString;


bool fileSaveXml(const QString& _filename, QGraphicsView* _view);
QString createSceneXml(QGraphicsScene* _scene, QGraphicsView* _view = nullptr);

#endif // FILE_SAVE_XML_H

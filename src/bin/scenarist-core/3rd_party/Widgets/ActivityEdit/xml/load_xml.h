#ifndef FILE_LOAD_XML_H
#define FILE_LOAD_XML_H

class CustomGraphicsScene;
class QGraphicsScene;
class QGraphicsView;
class QString;


bool loadSceneXml(const QString& _xml, QGraphicsScene* _scene, QGraphicsView* _view = NULL);
bool fileLoadXml(const QString& _filename, CustomGraphicsScene* _scene, QGraphicsView* _view = NULL);

#endif // FILE_LOAD_XML_H

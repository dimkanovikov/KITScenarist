#ifndef FILE_LOAD_XML_H
#define FILE_LOAD_XML_H

#include "../scene/customgraphicsscene.h"

#include <QString>


void loadSceneXml(const QString& _xml, QGraphicsScene* _scene, QGraphicsView* _view = NULL);
void fileLoadXml(const QString& _filename, CustomGraphicsScene* _scene, QGraphicsView* _view = NULL);

#endif // FILE_LOAD_XML_H

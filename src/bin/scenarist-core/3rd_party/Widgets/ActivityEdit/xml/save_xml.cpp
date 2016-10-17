#include "save_xml.h"
#include <QHash>
#include "../scene/customgraphicsscene.h"
#include "../shape/card.h"
#include "../flow/flow.h"
#include "../flow/arrowflow.h"
#include "../flow/flowtext.h"
#include "../shape/note.h"
#include "../shape/horizontalline.h"
#include "../shape/verticalline.h"
#include <typeinfo>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>

typedef void (*SAVE_SHAPE_FUNC) (QXmlStreamWriter& _writer, Shape* _shape, QHash<Shape*, int>& _ids);


void saveFlowKnots(QXmlStreamWriter& _writer, Flow* _flow)
{
	QPolygonF poly = _flow->createFlowPolygon();
	_writer.writeAttribute("KnotsCount", QString::number(poly.count()-2));
	for (int i = 1; i < poly.count() - 1; ++i) {
		_writer.writeAttribute("KnotX_" + QString::number(i - 1), QString::number(poly[i].x()));
		_writer.writeAttribute("KnotY_" + QString::number(i - 1), QString::number(poly[i].y()));
	}
}

void saveCardShape(QXmlStreamWriter& _writer, Shape* _shape, QHash<Shape*, int>& _ids)
{
	CardShape* card = dynamic_cast<CardShape*>(_shape);
	_writer.writeStartElement("ActionShape");
	_writer.writeAttribute("id", QString::number(_ids[card]));
	if (card->parentItem() != nullptr) {
		if (CardShape* parentCard = dynamic_cast<CardShape*>(card->parentItem())) {
			_writer.writeAttribute("parent_id", QString::number(_ids[parentCard]));
		}
	}
	_writer.writeAttribute("x", QString::number(card->pos().x()));
	_writer.writeAttribute("y", QString::number(card->pos().y()));
	_writer.writeAttribute("width", QString::number(card->size().width()));
	_writer.writeAttribute("height", QString::number(card->size().height()));
    _writer.writeAttribute("uuid", card->uuid());
	_writer.writeAttribute("card_type", QString::number(card->cardType()));
	_writer.writeAttribute("title", card->title());
	_writer.writeAttribute("description", card->description());
	_writer.writeEndElement();
}

void saveNoteShape(QXmlStreamWriter& _writer, Shape* _shape, QHash<Shape*, int>& _ids)
{
	NoteShape* note = dynamic_cast<NoteShape*>(_shape);
	_writer.writeStartElement("NoteShape");
	_writer.writeAttribute("id", QString::number(_ids[note]));
	_writer.writeAttribute("x", QString::number(note->pos().x()));
	_writer.writeAttribute("y", QString::number(note->pos().y()));
	_writer.writeAttribute("width", QString::number(note->size().width()));
	_writer.writeAttribute("height", QString::number(note->size().height()));
	_writer.writeAttribute("title", note->text());
	_writer.writeEndElement();
}

void saveArrowFlow(QXmlStreamWriter& _writer, Shape* _shape, QHash<Shape*, int>& _ids)
{
	ArrowFlow* flow = dynamic_cast<ArrowFlow*>(_shape);
	_writer.writeStartElement("ArrowFlow");
	_writer.writeAttribute("id", QString::number(_ids[flow]));
	_writer.writeAttribute("from_id", QString::number(_ids[flow->startShape()]));
	_writer.writeAttribute("to_id", QString::number(_ids[flow->endShape()]));
	_writer.writeAttribute("offsetX", QString::number(flow->textShape()->offsetX()));
	_writer.writeAttribute("offsetY", QString::number(flow->textShape()->offsetY()));
	_writer.writeAttribute("text", flow->text());
	saveFlowKnots(_writer, flow);
	_writer.writeEndElement();
}

void saveHorizontalLineShape(QXmlStreamWriter& _writer, Shape* _shape, QHash<Shape*, int>& _ids)
{
	HorizontalLineShape* line = dynamic_cast<HorizontalLineShape*>(_shape);
	_writer.writeStartElement("HorizontalLineShape");
	_writer.writeAttribute("id", QString::number(_ids[line]));
	_writer.writeAttribute("x", QString::number(line->pos().x()));
	_writer.writeAttribute("y", QString::number(line->pos().y()));
	_writer.writeEndElement();
}

void saveVerticalLineShape(QXmlStreamWriter& _writer, Shape* _shape, QHash<Shape*, int>& _ids)
{
	VerticalLineShape* line = dynamic_cast<VerticalLineShape*>(_shape);
	_writer.writeStartElement("VerticalLineShape");
	_writer.writeAttribute("id", QString::number(_ids[line]));
	_writer.writeAttribute("x", QString::number(line->pos().x()));
	_writer.writeAttribute("y", QString::number(line->pos().y()));
	_writer.writeEndElement();
}

void fileSaveXml(const QString& _filename, QGraphicsView* _view)
{
	QFile file(_filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		throw FileErrorException(_filename);
	}
	QTextStream stream(&file);
	stream << createSceneXml(_view->scene(), _view);
	file.close();
}

QString createSceneXml(QGraphicsScene* _scene, QGraphicsView* _view)
{
	QHash<Shape*, int> shapeIds;
	QHash<QString, SAVE_SHAPE_FUNC> saveFunc;

	QString resultXml;
	QXmlStreamWriter writer(&resultXml);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	writer.writeStartElement("cards_xml");

	if (_view) {
		QSize sz = _view->viewport()->size();
		QPointF pt = _view->mapToScene(sz.width()/2, sz.height()/2);
		writer.writeAttribute("viewx", QString::number(pt.x()));
		writer.writeAttribute("viewy", QString::number(pt.y()));
	}

	//
	// Создаем таблицу идентификаторов
	//
	CustomGraphicsScene* scene = dynamic_cast<CustomGraphicsScene*>(_scene);
	QList<Shape*> items = scene->shapes();
	int curid = 0;
	for (Shape *item : items) {
		shapeIds[item] = curid++;
	}

	//
	// Формируем список функций для сохранения объектов
	//
	saveFunc[typeid(CardShape).name()] = saveCardShape;
	saveFunc[typeid(NoteShape).name()] = saveNoteShape;
	saveFunc[typeid(HorizontalLineShape).name()] = saveHorizontalLineShape;
	saveFunc[typeid(VerticalLineShape).name()] = saveVerticalLineShape;
	saveFunc[typeid(ArrowFlow).name()] = saveArrowFlow;

	for (Shape *item : items) {
		if (saveFunc[typeid(*item).name()]) {
			saveFunc[typeid(*item).name()](writer, item, shapeIds);
		} else {
			qDebug() << "unhandled shape: " << typeid(*item).name();
		}
	}

	writer.writeEndElement();
    writer.writeEndDocument();

	return resultXml;
}

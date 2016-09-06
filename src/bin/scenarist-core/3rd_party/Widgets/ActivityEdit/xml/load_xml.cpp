#include "../flow/arrowflow.h"
#include "../flow/flowtext.h"
#include "../shape/card.h"
#include "../shape/horizontalline.h"
#include "../shape/verticalline.h"
#include "../shape/note.h"

#include "load_xml.h"
#include "save_xml.h" // FileErrorException is here
#include <QtXml/QDomNode>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QHash>
#include <QDebug>

typedef Shape *(*LOADFUNC) (QDomNode& _node, QHash<int, Shape*>& _ids);

void loadFlowKnots(const QDomNamedNodeMap& _nodeMap, Flow* _flow)
{
	int count = _nodeMap.namedItem("KnotsCount").toAttr().value().toInt();
	QList<QPointF> list;
	for (int i = 0; i < count; ++i) {
		QPointF pt = QPointF(
			_nodeMap.namedItem("KnotX_" + QString::number(i)).toAttr().value().toDouble(),
			_nodeMap.namedItem("KnotY_" + QString::number(i)).toAttr().value().toDouble()
		);
		list << pt;
	}
	_flow->setFlowKnots(list);
}

Shape* loadArrowFlow(QDomNode& _node, QHash<int, Shape*>& _ids)
{
	const QDomNamedNodeMap& attributes = _node.attributes();
	int id = attributes.namedItem("id").toAttr().value().toInt();
	int sid = attributes.namedItem("from_id").toAttr().value().toInt();
	int eid = attributes.namedItem("to_id").toAttr().value().toInt();
	double deltaX = attributes.namedItem("offsetX").toAttr().value().toDouble();
	double deltaY = attributes.namedItem("offsetY").toAttr().value().toDouble();
	QString text = attributes.namedItem("text").toAttr().value();
	if (!_ids[sid] || !_ids[eid]){
		throw "can't load this";
	}
	ArrowFlow *flow = new ArrowFlow(_ids[sid], _ids[eid]);
	_ids[id] = flow;
	flow->setText(text);
	flow->textShape()->setOffset(deltaX, deltaY);
	loadFlowKnots(attributes, flow);
	return flow;
}

Shape* loadCardShape(QDomNode& _node, QHash<int, Shape*>& _ids)
{
	const QDomNamedNodeMap& attributes = _node.attributes();
	int id = attributes.namedItem("id").toAttr().value().toInt();
	Shape* parentCard = nullptr;
	if (attributes.contains("parent_id")) {
		int parentId = attributes.namedItem("parent_id").toAttr().value().toInt();
		parentCard = _ids[parentId];
	}
	double x = attributes.namedItem("x").toAttr().value().toDouble();
	double y = attributes.namedItem("y").toAttr().value().toDouble();
	double width = attributes.namedItem("width").toAttr().value().toDouble();
	double height = attributes.namedItem("height").toAttr().value().toDouble();
	int cardType = attributes.namedItem("card_type").toAttr().value().toInt();
	QString title = attributes.namedItem("title").toAttr().value();
	QString description = attributes.namedItem("description").toAttr().value();
	CardShape* card = new CardShape((CardShape::CardType)cardType, title, description, QPointF(x,y));
	card->setSize(QSizeF(width, height));
	card->setTitle(title);
	card->setDescription(description);
	if (parentCard != nullptr) {
		card->setParentItem(parentCard);
		card->setPos(x, y);
	}

	_ids[id] = card;
	return card;
}

Shape* loadNoteShape(QDomNode& _node, QHash<int, Shape*>& _ids)
{
	const QDomNamedNodeMap& attributes = _node.attributes();
	int id = attributes.namedItem("id").toAttr().value().toInt();
	double x = attributes.namedItem("x").toAttr().value().toDouble();
	double y = attributes.namedItem("y").toAttr().value().toDouble();
	double width = attributes.namedItem("width").toAttr().value().toDouble();
	double height = attributes.namedItem("height").toAttr().value().toDouble();
	QString title = attributes.namedItem("title").toAttr().value();
	NoteShape* note = new NoteShape(title, QPointF(x,y));
	note->setSize(QSizeF(width, height));
	note->setText(title);
	_ids[id] = note;
	return note;
}

Shape* loadHorizontalLineShape(QDomNode& _node, QHash<int, Shape*>& _ids)
{
	const QDomNamedNodeMap& attributes = _node.attributes();
	int id = attributes.namedItem("id").toAttr().value().toInt();
	double x = attributes.namedItem("x").toAttr().value().toDouble();
	double y = attributes.namedItem("y").toAttr().value().toDouble();
	HorizontalLineShape* line = new HorizontalLineShape(QPointF(x,y));
	_ids[id] = line;
	return line;
}

Shape* loadVerticalLineShape(QDomNode& _node, QHash<int, Shape*>& _ids)
{
	const QDomNamedNodeMap& attributes = _node.attributes();
	int id = attributes.namedItem("id").toAttr().value().toInt();
	double x = attributes.namedItem("x").toAttr().value().toDouble();
	double y = attributes.namedItem("y").toAttr().value().toDouble();
	VerticalLineShape* line = new VerticalLineShape(QPointF(x,y));
	_ids[id] = line;
	return line;
}

void fileLoadXml(const QString& _filename, CustomGraphicsScene* _scene, QGraphicsView* _view)
{
	QFile file(_filename);
	if (!file.open(QIODevice::ReadOnly)) {
		throw FileErrorException(_filename);
	}
	QByteArray data = file.readAll();

	try {
		loadSceneXml(QString(data), _scene, _view);
	}
	catch (...) {
		throw FileErrorException("");
	}
	file.close();
}

void loadSceneXml(const QString& _xml, CustomGraphicsScene* _scene, QGraphicsView* _view)
{
	QDomDocument doc;
	if (!doc.setContent(_xml)) {
		throw FileErrorException("");
	}

	QHash<int, Shape*> ids;
	QHash<QString, LOADFUNC> loadfuncs;
	loadfuncs["ArrowFlow"] = loadArrowFlow;
	loadfuncs["ActionShape"] = loadCardShape;
	loadfuncs["HorizontalLineShape"] = loadHorizontalLineShape;
	loadfuncs["VerticalLineShape"] = loadVerticalLineShape;
	loadfuncs["NoteShape"] = loadNoteShape;

	if (_view) {
		double x = doc.documentElement().attribute("viewx", "0").toDouble();
		double y = doc.documentElement().attribute("viewy", "0").toDouble();
		_view->centerOn(x, y);
	}

	const QDomNodeList& items = doc.documentElement().childNodes();
	QList<QString> shapes = (QList<QString>()
		<< "ActionShape"
		<< "NoteShape"
		<< "HorizontalLineShape"
		<< "VerticalLineShape"
	);
	QList<QString> flows = { "ArrowFlow" };

	QList<int> first, second, other;
	for (int i = 0; i < items.count(); ++i) {
		QDomElement e = items.at(i).toElement();
		if (shapes.contains(e.tagName())) {
			first.append(i);
		} else if (flows.contains(e.tagName())) {
			second.append(i);
		} else {
			other.append(i);
		}
	}

	QList<int> all = (QList<int>() << first << second << other);
	for (int i = 0; i < items.count(); ++i) {
		QDomElement e = items.at(all[i]).toElement();
		if (loadfuncs[e.tagName()]) {
			Shape *shp = loadfuncs[e.tagName()](e, ids);
			_scene->appendShape(shp);
		}
	}
}

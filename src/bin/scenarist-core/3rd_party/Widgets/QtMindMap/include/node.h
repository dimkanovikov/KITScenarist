#ifndef NODE_H
#define NODE_H

#include <QGraphicsTextItem>
#include <QTextCursor>
#include <QGraphicsDropShadowEffect>

#include "edge.h"
//#include "graphwidget.h"
#include "graphlogic.h"

//class GraphWidget;
class GraphLogic;

class Node : public QGraphicsTextItem
{
	Q_OBJECT

public:
	static const QColor defaultBackgroundColor;

public:

	Node(GraphLogic *graphLogic, bool isRoot);
	~Node();

	// add/remove edges
	void addEdge(Edge *edge, bool startsFromThisNode);
	void deleteEdge(Node *otherEnd);
	void deleteEdges();
	void removeEdge(Edge *edge);
	void removeEdges();


	// graph traversal
	QList<Edge *> edges() const;
	QList<Edge *> edgesFrom(const bool &excludeSecondaries = true) const;
	QList<Edge *> edgesToThis(const bool &excludeSecondaries = true) const;
	Edge * edgeTo(const Node* node) const;
	QList<Node *> subtree() const;
	bool isConnected(const Node *node) const;

	// prop set/get
	bool isRoot() const;
	void setBorder(const bool &hasBorder = true);
	void setEditable(const bool &editable = true);
	void setColor(const QColor &color);
	QColor color() const;
	void setTextColor(const QColor &color);
	QColor textColor() const;
	void setScale(const qreal &factor, const QRectF &sceneRect);

	// show numbers in hint mode
	void showNumber(const int &number, const bool& show = true,
					const bool &numberIsSpecial = false);
	// insert picture to the cursor's current position
	void insertPicture(const QString &picture);

	// changing visibility from prot to pub
	// so GraphWidget::keyPressEvent can call it edit during editing
	void keyPressEvent(QKeyEvent *event);

	// переопределяем, чтобы для крневого элемента передавать расширенную область
	QRectF boundingRect() const;

	// calculetes the intersection of line and shape of this Node
	QPointF intersection(const QLineF &line, const bool &reverse = false) const;

	// returns with the biggest angle between the edges
	double calculateBiggestAngle() const;

	static const QPointF newNodeCenter;
	static const QPointF newNodeBottomRigth;

signals:

	void nodeChanged();
	void nodeSelected();
	void nodeEdited();
	void nodeMoved(QGraphicsSceneMouseEvent *event);
	void nodeLostFocus();

protected:

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			   QWidget *widget);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	QPainterPath shape () const;
	void focusOutEvent(QFocusEvent *event);

private:

	double doubleModulo(const double &devided, const double &devisor) const;

	struct EdgeElement
	{
		Edge *edge;
		bool startsFromThisNode;
		EdgeElement(Edge *e, bool s) : edge(e), startsFromThisNode(s) {}
	};


	QList<EdgeElement> m_edgeList;
	GraphLogic *m_graphLogic;
	bool m_isRoot;
	bool m_hasBorder;
	QColor m_color;
	QColor m_textColor;
	QGraphicsDropShadowEffect *m_effect;

	static const double m_pi;
	static const double m_halfPi;
	static const double m_oneAndHalfPi;
	static const double m_twoPi;
};

#endif // NODE_H

#include "../include/graphwidget.h"

#include <QtXml>
#include <QColorDialog>
#include <QApplication>
#include <QScrollBar>
#include <QUndoCommand>

#include "../include/commands.h"

GraphLogic::GraphLogic(GraphWidget *parent)
	: QObject(parent)
	, m_graphWidget(parent)
	, m_activeNode(0)
	, m_editingNode(false)
	, m_edgeAdding(false)
	, m_edgeDeleting(false)
	, m_undoStack(new QUndoStack(this))
{
	m_memberMap.insert(std::pair<int, void(GraphLogic::*)()>
					   (Qt::Key_Insert, &GraphLogic::insertNode));
	m_memberMap.insert(std::pair<int, void(GraphLogic::*)()>
					   (Qt::Key_Delete, &GraphLogic::removeNode));
	m_memberMap.insert(std::pair<int, void(GraphLogic::*)()>
					   (Qt::Key_Enter, &GraphLogic::nodeEdited));
	m_memberMap.insert(std::pair<int, void(GraphLogic::*)()>
					   (Qt::Key_Return, &GraphLogic::nodeEdited));

	m_memberMap.insert(std::pair<int, void(GraphLogic::*)()>
					   (Qt::Key_Up, &GraphLogic::moveNodeUp));
	m_memberMap.insert(std::pair<int, void(GraphLogic::*)()>
					   (Qt::Key_Down, &GraphLogic::moveNodeDown));
	m_memberMap.insert(std::pair<int, void(GraphLogic::*)()>
					   (Qt::Key_Left, &GraphLogic::moveNodeLeft));
	m_memberMap.insert(std::pair<int, void(GraphLogic::*)()>
					   (Qt::Key_Right, &GraphLogic::moveNodeRight));
}

GraphWidget *GraphLogic::graphWidget() const
{
	return m_graphWidget;
}

bool GraphLogic::processKeyEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		nodeLostFocus();
		return true;
	}

	if (m_editingNode)
	{
		m_activeNode->keyPressEvent(event);
		return true;
	}

	if ((event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) &&
		event->modifiers() ^ Qt::ControlModifier)
	{
		scaleUp();
		return true;
	}
	if (event->key() == Qt::Key_Minus &&
		event->modifiers() ^ Qt::ControlModifier)
	{
		scaleDown();
		return true;
	}

	if (event == QKeySequence::Undo) {
		m_undoStack->undo();
		return true;
	}

	if (event == QKeySequence::Redo) {
		m_undoStack->redo();
		return true;
	}

	if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) &&
		event->modifiers() & Qt::ControlModifier)
	{
		insertSiblingNode();
		return true;
	}

	if (m_memberMap.find(event->key()) != m_memberMap.end())
	{
		(this->*m_memberMap[event->key()])();
		return true;
	}

	return false;
}

void GraphLogic::addFirstNode()
{
    Node *node = nodeFactory(true);
	m_graphWidget->scene()->addItem(node);
    m_nodeList.append(node);

	m_activeNode = m_nodeList.first();
	m_activeNode->setBorder();
}

void GraphLogic::removeAllNodes()
{
	foreach(Node *node, m_nodeList)
		delete node;

	m_nodeList.clear();
	m_activeNode = 0;
	m_hintNode = 0;
}

bool GraphLogic::readContentFromXml(const QString& _xml)
{
	m_undoStack->clear();

	// open & parse XML file
	QDomDocument doc("QtMindMap");
	if (!doc.setContent(_xml))
	{
		emit notification(tr("Couldn't parse XML file."));
		return false;
	}

	QDomElement docElem = doc.documentElement();

	//
	// Восстанавливаем масштаб
	//
	m_graphWidget->resetTransform();
	const qreal scaleFactor = docElem.attribute("scale").toDouble();
	m_graphWidget->scale(scaleFactor, scaleFactor);
	//
	// ... и позиционирование
	//
	const int scrollX = docElem.attribute("scroll_x").toInt();
	m_graphWidget->horizontalScrollBar()->setValue(scrollX);
	const int scrollY = docElem.attribute("scroll_y").toInt();
	m_graphWidget->verticalScrollBar()->setValue(scrollY);

	// add nodes
	QDomNodeList nodes = docElem.childNodes().item(0).childNodes();
	for (int i = 0; i < nodes.length(); i++)
	{
		QDomElement e = nodes.item(i).toElement();
		if(!e.isNull())
		{
            Node *node = nodeFactory(e.attribute("is_root").toInt());
			node->disconnect(node, SIGNAL(nodeChanged()), this, SLOT(nodeChanged()));
			m_graphWidget->scene()->addItem(node);
			m_nodeList.append(node);
			node->setHtml(e.attribute("htmlContent"));
			node->setPos(e.attribute("x").toFloat(),
						 e.attribute("y").toFloat());
			node->setScale(e.attribute("scale").toFloat(),
						   m_graphWidget->sceneRect());
			node->setColor(QColor(e.attribute("bg_red").toFloat(),
								  e.attribute("bg_green").toFloat(),
								  e.attribute("bg_blue").toFloat()));
			node->setTextColor(QColor(e.attribute("text_red").toFloat(),
									  e.attribute("text_green").toFloat(),
									  e.attribute("text_blue").toFloat()));
			node->connect(node, SIGNAL(nodeChanged()), this, SLOT(nodeChanged()));
		}
	}

	// add edges
	QDomNodeList edges = docElem.childNodes().item(1).childNodes();
	for (int i = 0; i < edges.length(); i++)
	{
		QDomElement e = edges.item(i).toElement();
		if(!e.isNull())
		{
			Node *source = m_nodeList[e.attribute("source").toInt()];
			Node *destination = m_nodeList[e.attribute("destination").toInt()];

			Edge *edge = new Edge(source, destination);
			source->addEdge(edge, true);
			destination->addEdge(edge, false);

			edge->setColor(QColor(e.attribute("red").toFloat(),
								  e.attribute("green").toFloat(),
								  e.attribute("blue").toFloat()));
			edge->setWidth(e.attribute("width").toFloat());
			edge->setSecondary(e.attribute("secondary").toInt() );

			m_graphWidget->scene()->addItem(edge);
		}
	}

	emit contentChanged();

	// test the first node the active one
	m_activeNode = m_nodeList.first();
	m_activeNode->setBorder();
	m_activeNode->setFocus();

	m_graphWidget->show();

	return true;
}

bool GraphLogic::readContentFromXmlFile(const QString &fileName)
{
	// open & parse XML file
	QDomDocument doc("QtMindMap");
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		emit notification(tr("Couldn't read file."));
		return false;
	}

	return readContentFromXml(file.readAll());
}

QString GraphLogic::writeContentToXml()
{
	// create XML doc object
	QDomDocument doc("QtMindMap");

	QDomElement root = doc.createElement("qtmindmap");
	root.setAttribute("scale", QString::number(m_graphWidget->transform().m11()));
	root.setAttribute("scroll_x", QString::number(m_graphWidget->horizontalScrollBar()->value()));
	root.setAttribute("scroll_y", QString::number(m_graphWidget->verticalScrollBar()->value()));
	doc.appendChild( root );

	// nodes
	QDomElement nodes_root = doc.createElement("nodes");
	root.appendChild(nodes_root);
	foreach(Node *node, m_nodeList)
	{
		QDomElement cn = doc.createElement("node");

		// no need to store ID: parsing order is preorder.
		// cn.setAttribute( "id", QString::number(m_nodeList.indexOf(node)));
        cn.setAttribute( "is_root", node->isRoot() ? "1" : "0");
		cn.setAttribute( "x", QString::number(node->pos().x()));
		cn.setAttribute( "y", QString::number(node->pos().y()));
		cn.setAttribute( "htmlContent", node->toHtml());
		cn.setAttribute( "scale", QString::number(node->scale()));
		cn.setAttribute( "bg_red", QString::number(node->color().red()));
		cn.setAttribute( "bg_green", QString::number(node->color().green()));
		cn.setAttribute( "bg_blue", QString::number(node->color().blue()));
		cn.setAttribute( "text_red", QString::number(node->textColor().red()));
		cn.setAttribute( "text_green", QString::number(node->textColor().green()));
		cn.setAttribute( "text_blue", QString::number(node->textColor().blue()));
		nodes_root.appendChild(cn);
	}

	//edges
	QDomElement edges_root = doc.createElement("edges");
	root.appendChild(edges_root);
	foreach(Edge *edge, allEdges())
	{
		QDomElement cn = doc.createElement("edge");
		cn.setAttribute( "source",
					  QString::number(m_nodeList.indexOf(edge->sourceNode())));
		cn.setAttribute( "destination",
					  QString::number(m_nodeList.indexOf(edge->destNode())));
		cn.setAttribute( "red", QString::number(edge->color().red()));
		cn.setAttribute( "green", QString::number(edge->color().green()));
		cn.setAttribute( "blue", QString::number(edge->color().blue()));
		cn.setAttribute( "width", QString::number(edge->width()));
		cn.setAttribute( "secondary", QString::number(edge->secondary()));

		edges_root.appendChild(cn);
	}

	return doc.toString();
}

void GraphLogic::writeContentToXmlFile(const QString &fileName)
{
	const QString xml = writeContentToXml();

	// write XML doc object to file
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		emit notification(tr("Couldn't open file to write."));
		return;
	}
	QTextStream ts( &file );
	ts << xml;
	file.close();

	// show a statusBar message to the user
	emit notification(tr("Saved."));
}

void GraphLogic::writeContentToPngFile(const QString &fileName)
{
	QImage img(m_graphWidget->scene()->sceneRect().width(),
			   m_graphWidget->scene()->sceneRect().height(),
			   QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&img);

	painter.setRenderHint(QPainter::Antialiasing);

	// Strange that I have to set this, and scene->render() does not do this
	m_graphWidget->scene()->setBackgroundBrush(QApplication::palette().base());

	m_graphWidget->scene()->render(&painter);
	painter.setBackground(QApplication::palette().base());
	painter.end();

	img.save(fileName);

	// show a statusBar message to the user
	emit notification(tr("MindMap exported as ") + fileName);
}

Node * GraphLogic::nodeFactory(bool isRootNode)
{
    Node *node = new Node(this, isRootNode);

	connect(node, SIGNAL(nodeChanged()), this, SLOT(nodeChanged()));
	connect(node, SIGNAL(nodeSelected()), this, SLOT(nodeSelected()));
	connect(node, SIGNAL(nodeEdited()), this, SLOT(nodeEdited()));
	connect(node, SIGNAL(nodeMoved(QGraphicsSceneMouseEvent*)),
			this, SLOT(nodeMoved(QGraphicsSceneMouseEvent*)));
	connect(node, SIGNAL(nodeLostFocus()), this, SLOT(nodeLostFocus()));

	return node;
}

Node* GraphLogic::activeNode() const
{
	return m_activeNode;
}

void GraphLogic::setActiveNode(Node *node)
{
	if (m_activeNode != node) {
		if (m_activeNode!=0)
			m_activeNode->setBorder(false);

		m_activeNode = node;
		if (m_activeNode)
			m_activeNode->setBorder();

		emit activeNodeChanged();
	}
}

void GraphLogic::insertRootNode()
{
	// checks
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	// get the biggest angle between the edges of the Node.
	double angle(m_activeNode->calculateBiggestAngle());

	// let the distance between the current and new Node be 100 pixels
	qreal length(100);

	QPointF pos(m_activeNode->sceneBoundingRect().center() +
				 QPointF(length * cos(angle), length * sin(angle)) -
				 Node::newNodeCenter);

	QRectF rect (m_graphWidget->scene()->sceneRect().topLeft(),
				 m_graphWidget->scene()->sceneRect().bottomRight()
				 - Node::newNodeBottomRigth);

	if (!rect.contains(pos))
	{
		emit notification(tr("New node would be placed outside of the scene."));
		return;
	}

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_pos = pos;


	QUndoCommand *insertRootNodeCommand = new InsertRootNodeCommand(context);
	m_undoStack->push(insertRootNodeCommand);

	emit contentChanged();
}

void GraphLogic::insertNode()
{
	// checks
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	// get the biggest angle between the edges of the Node.
	double angle(m_activeNode->calculateBiggestAngle());

	// let the distance between the current and new Node be 100 pixels
	qreal length(100);

	QPointF pos(m_activeNode->sceneBoundingRect().center() +
				 QPointF(length * cos(angle), length * sin(angle)) -
				 Node::newNodeCenter);

	QRectF rect (m_graphWidget->scene()->sceneRect().topLeft(),
				 m_graphWidget->scene()->sceneRect().bottomRight()
				 - Node::newNodeBottomRigth);

	if (!rect.contains(pos))
	{
		emit notification(tr("New node would be placed outside of the scene."));
		return;
	}

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_pos = pos;


	QUndoCommand *insertNodeCommand = new InsertNodeCommand(context);
	m_undoStack->push(insertNodeCommand);

	emit contentChanged();
}

void GraphLogic::insertSiblingNode()
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node"));
		return;
	}

	// find parent
	Node* parentNode = 0;
	foreach (Edge* edge, m_activeNode->edges()) {
		if (edge->destNode() == m_activeNode) {
			parentNode = edge->sourceNode();
			break;
		}
	}

	if (!parentNode) {
		emit notification(tr("Node hasn't parent for add sibling node"));
		return;
	}

	// turn off editing mode
	if (m_editingNode) {
		nodeLostFocus();
	}
	// select parent node
	selectNode(parentNode);
	// insert sibling
	insertNode();
}

void GraphLogic::removeNode()
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	if (m_activeNode == m_nodeList.first())
	{
		emit notification(tr("Base node cannot be deleted."));
		return;
	}

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;

	QUndoCommand *insertNodeCommand = new RemoveNodeCommand(context);
	m_undoStack->push(insertNodeCommand);

	emit contentChanged();
}

void GraphLogic::nodeEdited()
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	m_editingNode = true;
	m_activeNode->setEditable();
    m_graphWidget->scene()->setFocusItem(m_activeNode);
}

void GraphLogic::scaleUp()
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	if (m_activeNode->scale()+qreal(0.2) > qreal(4))
	{
		emit notification(tr("Too much scaling."));
		return;
	}

	bool subtree(QApplication::keyboardModifiers() & Qt::ControlModifier &&
				 QApplication::keyboardModifiers() & Qt::ShiftModifier);

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_scale = m_activeNode->scale() + qreal(0.1);
	context.m_subtree = subtree;

	QUndoCommand *scaleNodeCommand = new ScaleNodeCommand(context);
	m_undoStack->push(scaleNodeCommand);

	emit contentChanged();
}

void GraphLogic::scaleDown()
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	if (m_activeNode->scale()-qreal(0.2) < qreal(0.1))
	{
		emit notification(tr("Too much scaling."));
		return;
	}

	bool subtree(QApplication::keyboardModifiers() & Qt::ControlModifier &&
				 QApplication::keyboardModifiers() & Qt::ShiftModifier);

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_scale = m_activeNode->scale() + qreal(-0.1);
	context.m_subtree = subtree;

	QUndoCommand *scaleNodeCommand = new ScaleNodeCommand(context);
	m_undoStack->push(scaleNodeCommand);

	emit contentChanged();
}

void GraphLogic::nodeColor()
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	bool subtree(QApplication::keyboardModifiers() & Qt::ControlModifier &&
				 QApplication::keyboardModifiers() & Qt::ShiftModifier);

	// popup a color selector dialogm def color is the curr one.
	QColorDialog dialog(m_graphWidget);
	dialog.setWindowTitle(tr("Select node color"));
	dialog.setCurrentColor(m_activeNode->color());
	if (!dialog.exec())
		return;

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_color = dialog.selectedColor();
	context.m_subtree = subtree;

	QUndoCommand *nodeColorCommand = new NodeColorCommand(context);
	m_undoStack->push(nodeColorCommand);

	emit contentChanged();
}

void GraphLogic::setNodeColor(const QColor& _color)
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	bool subtree(QApplication::keyboardModifiers() & Qt::ControlModifier &&
				 QApplication::keyboardModifiers() & Qt::ShiftModifier);

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_color = _color;
	context.m_subtree = subtree;

	QUndoCommand *nodeColorCommand = new NodeColorCommand(context);
	m_undoStack->push(nodeColorCommand);

	emit contentChanged();
}

void GraphLogic::nodeTextColor()
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	bool subtree(QApplication::keyboardModifiers() & Qt::ControlModifier &&
				QApplication::keyboardModifiers() & Qt::ShiftModifier);

	// popup a color selector dialogm def color is the curr one.
	QColorDialog dialog(m_graphWidget);
	dialog.setWindowTitle(tr("Select text color"));
	dialog.setCurrentColor(m_activeNode->textColor());
	if (!dialog.exec())
		return;

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_color = dialog.selectedColor();
	context.m_subtree = subtree;

	QUndoCommand *nodeTextColorCommand = new NodeTextColorCommand(context);
	m_undoStack->push(nodeTextColorCommand);

	emit contentChanged();
}

void GraphLogic::setNodeTextColor(const QColor& _color)
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	bool subtree(QApplication::keyboardModifiers() & Qt::ControlModifier &&
				QApplication::keyboardModifiers() & Qt::ShiftModifier);

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_color = _color;
	context.m_subtree = subtree;

	QUndoCommand *nodeTextColorCommand = new NodeTextColorCommand(context);
	m_undoStack->push(nodeTextColorCommand);

	emit contentChanged();
}

void GraphLogic::addEdge()
{
	emit notification(tr("Add edge: select destination node."));
	m_edgeAdding = true;
}

void GraphLogic::removeEdge()
{
	emit notification(tr("Delete edge: select other end-node."));
	m_edgeDeleting = true;
}

void GraphLogic::insertPicture(const QString &picture)
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	m_activeNode->insertPicture(picture);
}

void GraphLogic::nodeChanged()
{
	emit contentChanged();
}

void GraphLogic::nodeSelected()
{
	// if node == 0 then nodeSelected invoked after a signal from a Node
	selectNode(dynamic_cast<Node*>(QObject::sender()));
}

void GraphLogic::nodeMoved(QGraphicsSceneMouseEvent *event)
{
	// move just the active Node, or it's subtree too?
	QList <Node *> nodeList;
	if (event->modifiers() & Qt::ControlModifier &&
		event->modifiers() & Qt::ShiftModifier)
	{
		nodeList = m_activeNode->subtree();
	}
	else
	{
		nodeList.push_back(m_activeNode);
	}

	foreach(Node *node, nodeList)
		node->setPos(node->pos() + event->scenePos() - event->lastScenePos());
}

void GraphLogic::nodeLostFocus()
{
	if (m_editingNode)
	{
		m_editingNode = false;
		if (m_activeNode)
		{
			m_activeNode->setEditable(false);
			m_activeNode->update();
		}
		return;
	}

	if (m_edgeAdding)
	{
		m_edgeAdding = false;
		emit notification(tr("Edge adding cancelled."));
		return;
	}

	if (m_edgeDeleting)
	{
		m_edgeDeleting = false;
		emit notification(tr("Edge deleting cancelled."));
		return;
	}
}

void GraphLogic::moveNodeUp()
{
	QApplication::keyboardModifiers() & Qt::ControlModifier ?
		moveNode(qreal(0),qreal(-20)) :
		m_graphWidget->verticalScrollBar()->setValue(
					m_graphWidget->verticalScrollBar()->value()-20);
}

void GraphLogic::moveNodeDown()
{
	QApplication::keyboardModifiers() & Qt::ControlModifier ?
		moveNode(qreal(0),qreal(20)) :
				m_graphWidget->verticalScrollBar()->setValue(
							m_graphWidget->verticalScrollBar()->value()+20);
}

void GraphLogic::moveNodeLeft()
{
	QApplication::keyboardModifiers() & Qt::ControlModifier ?
		moveNode(qreal(-20),qreal(0)) :
				m_graphWidget->horizontalScrollBar()->setValue(
							m_graphWidget->horizontalScrollBar()->value()-20);
}

void GraphLogic::moveNodeRight()
{
	QApplication::keyboardModifiers() & Qt::ControlModifier ?
		moveNode(qreal(20),qreal(0)) :
				m_graphWidget->horizontalScrollBar()->setValue(
							m_graphWidget->horizontalScrollBar()->value()+20);
}

void GraphLogic::moveNode(qreal x, qreal y)
{
	if (!m_activeNode)
	{
		emit notification(tr("No active node."));
		return;
	}

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_x = x;
	context.m_y = y;

	QUndoCommand *moveCommand = new MoveCommand(context);
	m_undoStack->push(moveCommand);

	emit contentChanged();
}

void GraphLogic::selectNode(Node *node)
{
	if (m_edgeAdding && m_activeNode != 0)
	{
		addEdge(m_activeNode, node);
		m_edgeAdding = false;
	}
	else if (m_edgeDeleting && m_activeNode != 0)
	{
		removeEdge(m_activeNode, node);
		m_edgeDeleting = false;
	}
	else
	{
		setActiveNode(node);
	}
}

QList<Edge *> GraphLogic::allEdges() const
{
	QList<Edge *> list;

	// GraphWidget has a list of Nodes only.
	// Each Node maintains a list of it's own Edges.
	// We iterate through the list of Nodes and call Node::edgesFrom() on them.
	// edgesFrom(exludeSecundaries=false) return a list of edges (including
	// secondary edges) which starts from this Node.
	foreach(Node * node, m_nodeList)
		list.append(node->edgesFrom(false));

	return list;
}


void GraphLogic::addEdge(Node *source, Node *destination)
{
	if (destination == m_nodeList.first())
	{
		emit notification(tr("Base node cannot be a target."));
		return;
	}

	if (source->isConnected(destination))
	{
		emit notification(tr("There is already an edge between these two nodes."));
		return;
	}

	// aviod the graph beeing acyclic. (ok, Nodes having multiple parents)
	bool sec(false);
	if (!destination->edgesToThis().empty())
	{
		emit notification(
		   QObject::tr("The graph is acyclic, edge added as secondary edge."));
		sec = true;
	}

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_source = source;
	context.m_destination = destination;
	context.m_secondary = sec;

	QUndoCommand *addEdgeCommand = new AddEdgeCommand(context);
	m_undoStack->push(addEdgeCommand);

	emit contentChanged();
}

void GraphLogic::removeEdge(Node *source, Node *destination)
{
	if (!source->isConnected(destination))
	{
		emit notification(tr("There is no edge between these two nodes."));
		return;
	}

	UndoContext context;
	context.m_graphLogic = this;
	context.m_nodeList = &m_nodeList;
	context.m_activeNode = m_activeNode;
	context.m_source = source;
	context.m_destination = destination;

	QUndoCommand *removeEdgeCommand = new RemoveEdgeCommand(context);
	m_undoStack->push(removeEdgeCommand);

	emit contentChanged();
}

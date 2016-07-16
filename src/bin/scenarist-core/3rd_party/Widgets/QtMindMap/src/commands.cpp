#include "../include/commands.h"

#include <QDebug>
#include <QApplication>

#include <math.h>


BaseUndoClass::BaseUndoClass(UndoContext context)
    : m_done(false)
    , m_context(context)
    , m_activeNode(context.m_activeNode)
    , m_subtree(false)
{
    // remove just the active Node or it's subtree too?
    if (m_context.m_subtree ||
        (QApplication::keyboardModifiers() & Qt::ControlModifier &&
         QApplication::keyboardModifiers() & Qt::ShiftModifier))
    {
        m_nodeList = m_activeNode->subtree();
        m_subtree = true;
    }
    else
    {
        m_nodeList.push_back(m_activeNode);
    }
}



InsertNodeCommand::InsertNodeCommand(UndoContext context)
    : BaseUndoClass(context)
{
    setText(QObject::tr("Node added to \"").append(
            m_activeNode == m_context.m_nodeList->first() ?
                QObject::tr("Base node") :
                m_activeNode->toPlainText()).append("\""));

    m_context.m_graphLogic->nodeLostFocus();

    // create new node which inherits the color and textColor
    m_node = m_context.m_graphLogic->nodeFactory();
    m_node->setColor(m_activeNode->color());
    m_node->setTextColor(m_activeNode->textColor());
    m_node->setHtml(QString(""));

    // new edge inherits colors and size from target
    m_edge = new Edge(m_activeNode, m_node);
    m_edge->setColor(m_node->color());
    m_edge->setWidth(m_node->scale()*2 + 1);
    m_edge->setSecondary(false);
}

InsertNodeCommand::~InsertNodeCommand()
{
    if (!m_done)
    {
        delete m_edge;
        delete m_node;
    }
}

void InsertNodeCommand::undo()
{
    // remove node
    m_context.m_nodeList->removeAll(m_node);
    m_context.m_graphLogic->graphWidget()->scene()->removeItem(m_node);
    m_context.m_graphLogic->setActiveNode(m_activeNode);

    // remove edge
    m_edge->sourceNode()->removeEdge(m_edge);
    m_edge->destNode()->removeEdge(m_edge);
    m_context.m_graphLogic->graphWidget()->scene()->removeItem(m_edge);

    m_done = false;
}

void InsertNodeCommand::redo()
{
    // add node
    m_context.m_graphLogic->graphWidget()->scene()->addItem(m_node);
    m_context.m_nodeList->append(m_node);
    m_node->setPos(m_context.m_pos);
    m_context.m_graphLogic->setActiveNode(m_node);

    if (m_context.m_graphLogic->graphWidget()->hasFocus())
        m_context.m_graphLogic->nodeEdited();

    // add edge
    m_edge->sourceNode()->addEdge(m_edge,true);
    m_edge->destNode()->addEdge(m_edge,false);
    m_context.m_graphLogic->graphWidget()->scene()->addItem(m_edge);

    m_done = true;
}

InsertRootNodeCommand::InsertRootNodeCommand(UndoContext context)
    : BaseUndoClass(context)
{
    setText(QObject::tr("Node added to scene"));

    m_context.m_graphLogic->nodeLostFocus();

    m_node = m_context.m_graphLogic->nodeFactory(true);
    m_node->setHtml(QString(""));
}

InsertRootNodeCommand::~InsertRootNodeCommand()
{
    if (!m_done)
    {
        delete m_node;
    }
}

void InsertRootNodeCommand::undo()
{
    // remove node
    m_context.m_nodeList->removeAll(m_node);
    m_context.m_graphLogic->graphWidget()->scene()->removeItem(m_node);
    m_context.m_graphLogic->setActiveNode(m_activeNode);

    m_done = false;
}

void InsertRootNodeCommand::redo()
{
    // add node
    m_context.m_graphLogic->graphWidget()->scene()->addItem(m_node);
    m_context.m_nodeList->append(m_node);
    m_node->setPos(m_context.m_pos);
    m_context.m_graphLogic->setActiveNode(m_node);

    if (m_context.m_graphLogic->graphWidget()->hasFocus())
        m_context.m_graphLogic->nodeEdited();

    m_done = true;
}

RemoveNodeCommand::RemoveNodeCommand(UndoContext context)
    : BaseUndoClass(context)
{
    setText(QObject::tr("Node deleted \"").append(
                m_activeNode->toPlainText().append("\"")).append(
                m_subtree ?
                        QObject::tr(" with subtree") : QString("")));

    // collect affected edges
    foreach(Node *node, m_nodeList)
        foreach(Edge *edge, node->edges())
            if (m_edgeList.indexOf(edge) == -1)
                m_edgeList.push_back(edge);
}

void RemoveNodeCommand::undo()
{
    // add nodes
    foreach (Node *node, m_nodeList)
    {
        m_context.m_graphLogic->graphWidget()->scene()->addItem(node);
        m_context.m_nodeList->append(node);
    }

    // add edges
    foreach (Edge *edge, m_edgeList)
    {
        edge->sourceNode()->addEdge(edge,true);
        edge->destNode()->addEdge(edge,false);
        m_context.m_graphLogic->graphWidget()->scene()->addItem(edge);
    }

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

void RemoveNodeCommand::redo()
{
    foreach(Node *node, m_nodeList)
    {
        m_context.m_nodeList->removeAll(node);
        m_context.m_graphLogic->graphWidget()->scene()->removeItem(node);
    }

    foreach(Edge *edge, m_edgeList)
    {
        edge->sourceNode()->removeEdge(edge);
        edge->destNode()->removeEdge(edge);
        m_context.m_graphLogic->graphWidget()->scene()->removeItem(edge);
    }

    m_context.m_graphLogic->setActiveNode(0);
}

AddEdgeCommand::AddEdgeCommand(UndoContext context)
    : BaseUndoClass(context)
{
    setText(QObject::tr("Edge added between \"").append(
                m_context.m_source == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_source->toPlainText()).append(
            QObject::tr("\" and \"").append(
                m_context.m_destination == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_destination->toPlainText()).append("\"")));

    m_edge = new Edge(m_context.m_source, m_context.m_destination);
    m_edge->setColor(m_context.m_destination->color());
    m_edge->setWidth(m_context.m_destination->scale()*2 + 1);

    // The Edge is secondary, because the Node already has a parent
    // (it is already a destination of another Edge)
    m_edge->setSecondary(m_context.m_secondary);
}

void AddEdgeCommand::undo()
{
    m_context.m_source->removeEdge(m_edge);
    m_context.m_destination->removeEdge(m_edge);
    m_context.m_graphLogic->graphWidget()->scene()->removeItem(m_edge);

    m_context.m_graphLogic->setActiveNode(m_activeNode);
    m_done = false;
}

void AddEdgeCommand::redo()
{
    m_context.m_source->addEdge(m_edge, true);
    m_context.m_destination->addEdge(m_edge, false);

    m_context.m_graphLogic->graphWidget()->scene()->addItem(m_edge);

    m_context.m_graphLogic->setActiveNode(m_context.m_destination);
    m_done = true;
}

AddEdgeCommand::~AddEdgeCommand()
{
    if (!m_done)
        delete m_edge;
}

RemoveEdgeCommand::RemoveEdgeCommand(UndoContext context)
    : BaseUndoClass(context)
{
    setText(QObject::tr("Edge deleted between \"").append(
                m_context.m_source == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_source->toPlainText()).append(
            QObject::tr("\" and \"").append(
                m_context.m_destination == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_destination->toPlainText()).append("\"")));

    m_edge = m_context.m_source->edgeTo(m_context.m_destination);
}

void RemoveEdgeCommand::undo()
{
    m_context.m_source->addEdge(m_edge, true);
    m_context.m_destination->addEdge(m_edge, false);

    m_context.m_graphLogic->graphWidget()->scene()->addItem(m_edge);

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

void RemoveEdgeCommand::redo()
{
    m_context.m_source->removeEdge(m_edge);
    m_context.m_destination->removeEdge(m_edge);
    m_context.m_graphLogic->graphWidget()->scene()->removeItem(m_edge);

    m_context.m_graphLogic->setActiveNode(m_context.m_activeNode);
}

MoveCommand::MoveCommand(UndoContext context)
    : BaseUndoClass(context)
{
    setText(QObject::tr("Node \"").append(
                m_context.m_activeNode == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_activeNode->toPlainText()).
            append("\" moved (%1, %2)").arg(m_context.m_x).arg(m_context.m_y).
            append(m_subtree ? QObject::tr(" with subtree") : QString("")));
}

void MoveCommand::undo()
{
    foreach(Node *node, m_nodeList)
        node->moveBy(-m_context.m_x, -m_context.m_y);

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

void MoveCommand::redo()
{
    foreach(Node *node, m_nodeList)
        node->moveBy(m_context.m_x, m_context.m_y);

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

bool MoveCommand::mergeWith(const QUndoCommand *command)
{
    if (command->id() != id())
        return false;

    const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);

    if (m_context.m_activeNode != moveCommand->m_context.m_activeNode)
        return false;

    if (m_subtree != moveCommand->m_subtree)
        return false;

    m_context.m_x += moveCommand->m_context.m_x;
    m_context.m_y += moveCommand->m_context.m_y;

    setText(QObject::tr("Node \"").append(
                m_context.m_activeNode == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_activeNode->toPlainText()).
            append("\" moved (%1, %2)").arg(m_context.m_x).arg(m_context.m_y).
            append(m_subtree ? QObject::tr(" with subtree") : QString("")));

    return true;
}

int MoveCommand::id() const
{
    return MoveCommandId;
}

NodeColorCommand::NodeColorCommand(UndoContext context)
    : BaseUndoClass(context)
{
    setText(QObject::tr("Changing color of node: \"").append(
                m_context.m_activeNode == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_activeNode->toPlainText()).append("\"").
                append(m_subtree ? QObject::tr(" with subtree") : QString("")));

    foreach(Node *node, m_nodeList)
        m_colorMap[node] = node->color();
}

void NodeColorCommand::undo()
{
    foreach(Node *node, m_nodeList)
    {
        node->setColor(m_colorMap[node]);
        foreach (Edge * edge, node->edgesToThis(false))
            edge->setColor(m_colorMap[node]);
    }

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

void NodeColorCommand::redo()
{
    foreach(Node *node, m_nodeList)
    {
        node->setColor(m_context.m_color);
        foreach (Edge * edge, node->edgesToThis(false))
            edge->setColor(m_context.m_color);
    }

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

NodeTextColorCommand::NodeTextColorCommand(UndoContext context)
    : BaseUndoClass(context)
{
    setText(QObject::tr("Changing textcolor of node: \"").append(
                m_context.m_activeNode == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_activeNode->toPlainText()).append("\"").
                append(m_subtree ? QObject::tr(" with subtree") : QString("")));

    foreach(Node *node, m_nodeList)
        m_colorMap[node] = node->textColor();
}

void NodeTextColorCommand::undo()
{
    foreach(Node *node, m_nodeList)
        node->setTextColor(m_colorMap[node]);

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

void NodeTextColorCommand::redo()
{
    foreach(Node *node, m_nodeList)
        node->setTextColor(m_context.m_color);

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

ScaleNodeCommand::ScaleNodeCommand(UndoContext context)
    : BaseUndoClass(context)
{
    setText(QObject::tr("Node \"").append(
                m_context.m_activeNode == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_activeNode->toPlainText()).
            append("\" scaled (%1%)").arg(int((1+m_context.m_scale)*100)).
            append(m_subtree ? QObject::tr(" with subtree") : QString("")));
}

void ScaleNodeCommand::undo()
{
    foreach(Node *node, m_nodeList)
        node->setScale(qreal(-m_context.m_scale), m_context.m_graphLogic->graphWidget()->sceneRect());

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

void ScaleNodeCommand::redo()
{
    foreach(Node *node, m_nodeList)
        node->setScale(m_context.m_scale, m_context.m_graphLogic->graphWidget()->sceneRect());

    m_context.m_graphLogic->setActiveNode(m_activeNode);
}

bool ScaleNodeCommand::mergeWith(const QUndoCommand *command)
{
    if (command->id() != id())
        return false;

    const ScaleNodeCommand *scaleNodeCommand = static_cast<const ScaleNodeCommand *>(command);

    if (m_context.m_activeNode != scaleNodeCommand->m_context.m_activeNode)
        return false;

    if (m_subtree != scaleNodeCommand->m_subtree)
        return false;

    m_context.m_scale += scaleNodeCommand->m_context.m_scale;

    setText(QObject::tr("Node \"").append(
                m_context.m_activeNode == m_context.m_nodeList->first() ?
                    QObject::tr("Base node") :
                    m_context.m_activeNode->toPlainText()).
            append("\" scaled (%1%)").arg(int((1+m_context.m_scale)*100)).
            append(m_subtree ? QObject::tr(" with subtree") : QString("")));

    return true;
}

int ScaleNodeCommand::id() const
{
    return ScaleCommandId;
}

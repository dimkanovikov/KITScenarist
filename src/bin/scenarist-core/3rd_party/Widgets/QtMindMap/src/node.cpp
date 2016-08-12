#include "../include/node.h"

#include <QApplication>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>

const QPointF Node::newNodeCenter = QPointF(4, 11.5);
const QPointF Node::newNodeBottomRigth = QPointF(8, 23);

const double Node::m_pi = 3.14159265358979323846264338327950288419717;
const double Node::m_halfPi = Node::m_pi * 0.5;
const double Node::m_oneAndHalfPi = Node::m_pi * 1.5;
const double Node::m_twoPi = Node::m_pi * 2.0;

const QColor Node::m_defaultBackgroundColor(255, 135, 0);

Node::Node(GraphLogic *graphLogic, bool isRoot)
    : m_graphLogic(graphLogic)
    , m_isRoot(isRoot)
    , m_hasBorder(false)
    , m_color(m_defaultBackgroundColor)
    , m_textColor(Qt::white)
    , m_effect(new QGraphicsDropShadowEffect(this))
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setDefaultTextColor(Qt::white);
    if (m_isRoot) {
        QFont textFont = font();
        textFont.setBold(true);
        setFont(textFont);
        setHtml(QApplication::translate("QtMindMap::Node", "<b>Root node</b>"));
    }
    setZValue(2);
    setGraphicsEffect(m_effect);
    m_effect->setEnabled(false);
    m_effect->setOffset(qreal(4.0));
}

Node::~Node()
{
    deleteEdges();
}

void Node::addEdge(Edge *edge, bool startsFromThisNode)
{
    m_edgeList.push_back(EdgeElement(edge, startsFromThisNode));
    edge->adjust();
}

void Node::deleteEdge(Node *otherEnd)
{
    for(QList<EdgeElement>::iterator it = m_edgeList.begin();
        it != m_edgeList.end(); it++)
    {
        if ((it->edge->sourceNode() == otherEnd &&
             it->edge->destNode() == this)
         || (it->edge->sourceNode() == this &&
             it->edge->destNode() == otherEnd))
        {
            delete it->edge;
            return;
        }
    }
}

void Node::deleteEdges()
{
    foreach (EdgeElement element, m_edgeList)
        delete element.edge;
}

void Node::removeEdge(Edge *edge)
{
    for(QList<EdgeElement>::iterator it = m_edgeList.begin();
        it != m_edgeList.end(); it++)
    {
        if (it->edge == edge)
        {
            m_edgeList.erase(it);
            return;
        }
    }
}

void Node::removeEdges()
{
    for(QList<EdgeElement>::iterator it = m_edgeList.begin();
        it != m_edgeList.end(); it++)
    {
        m_edgeList.erase(it);
    }
}

QList<Edge *> Node::edges() const
{
    QList<Edge *> list;

    foreach(EdgeElement element, m_edgeList)
        list.push_back(element.edge);

    return list;
}

// edges from this Node. Exclude secondaries if needed (calc subtree)
QList<Edge *> Node::edgesFrom(const bool &excludeSecondaries) const
{
    QList<Edge *> list;

    foreach(EdgeElement element, m_edgeList)
        if (element.startsFromThisNode &&
                (!element.edge->secondary() || !excludeSecondaries))
            list.push_back(element.edge);

    return list;
}

// edges to this node (max 1 primary and any number of secondaries)
QList<Edge *> Node::edgesToThis(const bool &excludeSecondaries) const
{
    QList<Edge *> list;

    foreach(EdgeElement element, m_edgeList)
        if (!element.startsFromThisNode &&
                (!element.edge->secondary() || !excludeSecondaries))
            list.push_back(element.edge);

    return list;
}

// the edge from this Node to the parameter Node
Edge * Node::edgeTo(const Node *node) const
{
    foreach(EdgeElement element, m_edgeList)
        if ((element.edge->sourceNode() == node  ||
             element.edge->destNode() == node))
            return element.edge;

    return 0;
}

QList<Node *> Node::subtree() const
{
    /** @note QList crashes if modified while traversal,
      * QMutableListIterator lacks push_back, using good old std::list
      */

    std::list<Node *> list;
    list.push_back(const_cast<Node *>(this));

    // inorder: push_back the list of children Nodes of iterator
    for(std::list<Node *>::const_iterator it = list.begin();
        it != list.end();
        it++)
    {
        QList<Edge *> edges = (*it)->edgesFrom();
        foreach(Edge *edge, edges)
            if (!edge->secondary())
                list.push_back( edge->destNode() != this ?
                                   edge->destNode():
                                   edge->sourceNode());
    }

    return QList<Node *>::fromStdList(list);
}

// return thue if this and the parameter Node is connected with an edge
bool Node::isConnected(const Node *node) const
{
    foreach (EdgeElement element, m_edgeList)
        if (element.edge->sourceNode() == node ||
                element.edge->destNode() == node)
            return true;

    return false;
}

bool Node::isRoot() const
{
    return m_isRoot;
}

void Node::setBorder(const bool &hasBorder)
{
   m_hasBorder = hasBorder;
   m_effect->setColor(QApplication::palette().text().color());
   m_effect->setEnabled(hasBorder);

   update();
}

void Node::setEditable(const bool &editable)
{
    if (!editable) {
        QTextCursor cursor = textCursor();
        cursor.clearSelection();
        setTextCursor(cursor);
        setTextInteractionFlags(Qt::NoTextInteraction);
        return;
    }

    if (editable && textInteractionFlags() != Qt::TextEditorInteraction) {
        setTextInteractionFlags(Qt::TextEditorInteraction);

        // set cursor to the end
        QTextCursor c = textCursor();
        c.setPosition(c.document()->toPlainText().length());
        setTextCursor(c);
    }
}

void Node::setColor(const QColor &color)
{
    m_color = color;
    update();
}

QColor Node::color() const
{
    return m_color;
}

void Node::setTextColor(const QColor &color)
{
    m_textColor = color;
    update();
}

QColor Node::textColor() const
{
    return m_textColor;
}

void Node::setScale(const qreal &factor,const QRectF &sceneRect)
{
    // cannot scale out the Node from the scene
    if (!sceneRect.contains(pos() + boundingRect().bottomRight() * scale() * factor))
        return;

    prepareGeometryChange();
    QGraphicsTextItem::setScale(factor);

    // scale edges to this Node too
    foreach(EdgeElement element, m_edgeList)
    {
        if (!element.startsFromThisNode)
            element.edge->setWidth(factor * 4);

        element.edge->adjust();
    }
}

void Node::insertPicture(const QString &picture)
{
    QTextCursor c = textCursor();

    // strange, picture looks bad when node is scaled up
    c.insertHtml(QString("<img src=").append(picture).
                 append(" width=15 height=15></img>"));

    foreach (EdgeElement element, m_edgeList) element.edge->adjust();
    emit nodeChanged();
}

QPointF Node::intersection(const QLineF &line, const bool &reverse) const
{

    /// @note What a pity, the following does not work,
    /// doing it with brute (unaccurate) force

    //    QPainterPath nodeShape(shape());
    //    nodeShape.translate(pos());

    //    QPainterPath l;
    //    l.moveTo(line.p1());
    //    l.lineTo(line.p2());

    //    return nodeShape.intersected(l);


    QPainterPath path;
    path.addRoundedRect(sceneBoundingRect(), 6.0, 6.0);
    const qreal stepSize = 1 / line.length();
    if (reverse)
    {
        for (qreal t = 1.0; t >= 0; t -= stepSize)
            if (!path.contains(line.pointAt(t)))
                return line.pointAt(t - stepSize);
    }
    else
    {
        for (qreal t = 0.0; t <= 1; t += stepSize)
            if (!path.contains(line.pointAt(t)))
                return line.pointAt(t + stepSize);
    }

    return QPointF(0,0);
}

double Node::calculateBiggestAngle() const
{
    // in no edge, return with 3 o'clock
    if (m_edgeList.empty())
        return Node::m_twoPi;

    // if there is only one edge, return with it's extension
    if (m_edgeList.size()==1)
        return m_edgeList.first().startsFromThisNode ?
                    Node::m_pi - m_edgeList.first().edge->angle() :
                    Node::m_twoPi - m_edgeList.first().edge->angle();

    // put angles of every edges from this node to a list
    QList<double> tmp;
    for(QList<EdgeElement>::const_iterator it = m_edgeList.begin();
        it != m_edgeList.end(); it++)
    {
        tmp.push_back(it->startsFromThisNode ?
                 it->edge->angle() :
                 doubleModulo(Node::m_pi + it->edge->angle(), Node::m_twoPi));
    }
    qSort(tmp.begin(), tmp.end());

    // find the biggest diffrence, store prev angle
    double prev(tmp.first());
    double max_prev(tmp.last());
    double max(Node::m_twoPi - tmp.last() + tmp.first());

    for(QList<double>::const_iterator it = ++tmp.begin(); it!=tmp.end(); it++)
    {
        if (*it - prev > max )
        {
            max = *it - prev;
            max_prev = prev;
        }
        prev = *it;
    }

    // return with prev angle + max diff / 2
    return Node::m_twoPi - doubleModulo(max_prev + max / 2, Node::m_twoPi);
}

void Node::keyPressEvent(QKeyEvent *event)
{
    // cursor movements
    const QTextCursor::MoveMode moveMode =
        event->modifiers() & Qt::ShiftModifier
        ? QTextCursor::KeepAnchor
        : QTextCursor::MoveAnchor;
    switch (event->key()) {

    case Qt::Key_Left:
    {
        QTextCursor c = textCursor(); // textcursor() return just a copy
        c.movePosition(
                    event->modifiers() == Qt::ControlModifier ?
                    QTextCursor::PreviousWord :
                    QTextCursor::PreviousCharacter, moveMode);
        setTextCursor(c);
        break;
    }
    case Qt::Key_Right:
    {
        QTextCursor c = textCursor();
        c.movePosition(
                    event->modifiers() == Qt::ControlModifier ?
                    QTextCursor::NextWord :
                    QTextCursor::NextCharacter, moveMode);
        setTextCursor(c);
        break;
    }
    case Qt::Key_Up:
    {
        QTextCursor c = textCursor();
        c.movePosition(QTextCursor::Up, moveMode);
        setTextCursor(c);
        break;
    }
    case Qt::Key_Down:
    {
        QTextCursor c = textCursor();
        c.movePosition(QTextCursor::Down, moveMode);
        setTextCursor(c);
        break;
    }

    default:

        // not cursor movement: editing
        QGraphicsTextItem::keyPressEvent(event);

        foreach (EdgeElement element, m_edgeList) element.edge->adjust();
        emit nodeChanged();
    }

    if (!event->text().isEmpty()) {
        update();
    }

    ///@note leaving editing mode is done with esc, handled by graphwidget
}

QRectF Node::boundingRect() const
{
    QRectF result = QGraphicsTextItem::boundingRect();
    if (m_isRoot) {
        result.adjust(-10, -10, 10, 10);
    } else {
        result.adjust(-3, -3, 3, 3);
    }
    return result;
}

void Node::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *w)
{
    painter->setPen(Qt::transparent);
    if (m_hasBorder) {
        //
        // Рисуем рамку областью
        //
        painter->setBrush(QApplication::palette().text());
        QRectF borderRect = boundingRect();
        painter->drawRect(borderRect);

        //
        // Сужаем область отрисовки фона
        //
        painter->setBrush(m_color);
        QRectF bodyRect = borderRect.adjusted(1, 1, -1, -1);
        if (m_isRoot) {
            painter->drawRect(bodyRect);
        } else {
            painter->setBrush(QApplication::palette().base());
            painter->drawRect(bodyRect.adjusted(1, 1, -1, -1));
        }
    } else {
        painter->setBrush(m_color);
        if (m_isRoot) {
            painter->drawRect(boundingRect());
        } else {
            painter->drawRect(boundingRect());
            painter->setBrush(QApplication::palette().base());
            painter->drawRect(boundingRect().adjusted(2, 2, -2, -2));
        }
    }
    painter->setBrush(Qt::NoBrush);

    // the text itself
    if (m_isRoot) {
        setDefaultTextColor(m_textColor);
    } else {
        setDefaultTextColor(m_color);
    }
    QGraphicsTextItem::paint(painter, option, w);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {

    case ItemPositionChange:
    {
        // Node is about to move, check borders
        QPointF newPos = value.toPointF();

        // the fence is reduced with the size of the node
        QRectF rect (scene()->sceneRect().topLeft(),
                     scene()->sceneRect().bottomRight() -
                     boundingRect().bottomRight() * scale() );

        if (!rect.contains(newPos))
        {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }

         /// undo call here

        break;
    }
    case ItemPositionHasChanged:

        // Notify parent, adjust edges that a move has happended.
        foreach (EdgeElement element, m_edgeList) element.edge->adjust();
        emit nodeChanged();
        break;

    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit nodeSelected();

    QGraphicsTextItem::mousePressEvent(event);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit nodeEdited();

    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsTextItem::mouseReleaseEvent(event);
}

// notify parent so subtree can be moved too if necessary
void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF diff(event->scenePos() - event->lastScenePos());
    m_graphLogic->moveNode(diff.x(), diff.y());
}

QPainterPath Node::shape () const
{
    QPainterPath path;
    path.addRoundedRect(boundingRect(), 20.0, 15.0);
    return path;
}

// leave editing mode when user clicks on the view elsewhere for example
void Node::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    setEditable(false);

    emit nodeLostFocus();
}

// there is no such thing as modulo operator for double :P
double Node::doubleModulo(const double &devided, const double &devisor) const
{
    return devided - static_cast<double>(devisor * static_cast<int>(devided
                                                                  / devisor));
}

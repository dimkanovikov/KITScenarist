#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <exception>

#include "graphlogic.h"

class GraphLogic;


struct UndoContext
{
    GraphLogic *m_graphLogic;
    Node *m_activeNode;
    QList <Node *> *m_nodeList;
    QPointF m_pos;
    QColor m_color;
    Node *m_source;
    Node *m_destination;
    bool m_secondary;
    qreal m_x;
    qreal m_y;
    bool m_subtree;
    qreal m_scale;

    UndoContext(GraphLogic *graphLogic = 0,
                Node *activeNode = 0,
                Node *hintNode = 0,
                QList <Node *> *nodeList = 0,
                QPointF pos = QPointF(),
                QColor color = QColor(),
                Node *source = 0,
                Node *destination = 0,
                bool secondary = false,
                qreal x = 0,
                qreal y = 0,
                bool subtree = false,
                qreal scale = 0)
        : m_graphLogic(graphLogic)
        , m_activeNode(activeNode)
        , m_nodeList(nodeList)
        , m_pos(pos)
        , m_color(color)
        , m_source(source)
        , m_destination(destination)
        , m_secondary(secondary)
        , m_x(x)
        , m_y(y)
        , m_subtree(subtree)
        , m_scale(scale)
    {};
};

class BaseUndoClass : public QUndoCommand
{
public:

    enum MergeableCommandId
    {
        MoveCommandId = 0,
        ScaleCommandId
    };

    BaseUndoClass(UndoContext context);

protected:

    bool m_done;
    UndoContext m_context;
    Node *m_activeNode;
    QList <Node *> m_nodeList;
    bool m_subtree;
};


class InsertNodeCommand : public BaseUndoClass
{

public:

    InsertNodeCommand(UndoContext context);
    ~InsertNodeCommand();

    void undo();
    void redo();

private:

    Node *m_node;
    Edge *m_edge;
};

class InsertRootNodeCommand : public BaseUndoClass
{

public:

    InsertRootNodeCommand(UndoContext context);
    ~InsertRootNodeCommand();

    void undo();
    void redo();

private:

    Node *m_node;
};

class RemoveNodeCommand : public BaseUndoClass
{

public:

    RemoveNodeCommand(UndoContext context);

    void undo();
    void redo();

private:

    Node *m_hintNode;
    QList <Edge *> m_edgeList;
};

class AddEdgeCommand : public BaseUndoClass
{

public:

    AddEdgeCommand(UndoContext context);
    ~AddEdgeCommand();

    void undo();
    void redo();

private:

    Edge *m_edge;
};

class RemoveEdgeCommand : public BaseUndoClass
{

public:

    RemoveEdgeCommand(UndoContext context);

    void undo();
    void redo();

private:

    Edge *m_edge;
};

class MoveCommand : public BaseUndoClass
{

public:

    MoveCommand(UndoContext context);

    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);
    int id() const;
};

class NodeColorCommand : public BaseUndoClass
{

public:

    NodeColorCommand(UndoContext context);

    void undo();
    void redo();

private:

    QMap<Node*, QColor> m_colorMap;
};

class NodeTextColorCommand : public BaseUndoClass
{

public:

    NodeTextColorCommand(UndoContext context);

    void undo();
    void redo();

private:

    QMap<Node*, QColor> m_colorMap;
};

class ScaleNodeCommand : public BaseUndoClass
{

public:

    ScaleNodeCommand(UndoContext context);

    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);
    int id() const;
};


#endif // COMMANDS_H

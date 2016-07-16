#ifndef GRAPHLOGIC_H
#define GRAPHLOGIC_H

#include <QObject>
#include <QUndoStack>

#include "node.h"
#include "graphwidget.h"
#include "commands.h"


class GraphWidget;

class InsertNodeCommand;
class RemoveNodeCommand;
class AddEdgeCommand;
class RemoveEdgeCommand;

class GraphLogic : public QObject
{
    Q_OBJECT

public:

    explicit GraphLogic(GraphWidget *parent = 0);
    GraphWidget *graphWidget() const;

    bool processKeyEvent(QKeyEvent *event);
    void addFirstNode();
    void removeAllNodes();

    bool readContentFromXml(const QString& _xml);
    bool readContentFromXmlFile(const QString &fileName);
    QString writeContentToXml();
    void writeContentToXmlFile(const QString &fileName);
    void writeContentToPngFile(const QString &fileName);

    Node *nodeFactory(bool isRootNode = false);
    Node* activeNode() const;
    void setActiveNode(Node *node);

    void moveNode(qreal x, qreal y); // undo command

public slots:

    // commands from toolbars:
    void insertRootNode();  // undo command
    void insertNode();      // undo command
    void insertSiblingNode(); // undo command
    void removeNode();      // undo command
    void nodeEdited();      /// @todo Rewrite as an undo action
    void scaleUp();         // undo command
    void scaleDown();       // undo command
    void nodeColor();       // undo command
    void setNodeColor(const QColor& _color);
    void nodeTextColor();   // undo command
    void setNodeTextColor(const QColor& _color);
    void addEdge();
    void removeEdge();
    void insertPicture(const QString &picture); /// @todo Rewrite as an undo action

    void nodeChanged();
    void nodeSelected();
    void nodeMoved(QGraphicsSceneMouseEvent *event);
    void nodeLostFocus();

signals:

    void activeNodeChanged();
    void contentChanged(const bool& changed = true);
    void notification(const QString &msg);

private:

    void moveNodeUp();
    void moveNodeDown();
    void moveNodeLeft();
    void moveNodeRight();

    void selectNode(Node *node);

    // functions on the edges
    QList<Edge *> allEdges() const;
    void addEdge(Node *source, Node *destination);      // undo command
    void removeEdge(Node* source, Node *destination);   // undo command

    // hint mode's nodenumber handling functions
    void showNodeNumbers();
    void showingAllNodeNumbers(const bool &show = true);

    GraphWidget *m_graphWidget;

    QList<Node *> m_nodeList;
    Node *m_activeNode;
    bool m_showingNodeNumbers;
    QString m_hintNumber;
    Node *m_hintNode;
    bool m_editingNode;
    bool m_edgeAdding;
    bool m_edgeDeleting;

    std::map<int, void(GraphLogic::*)(void)> m_memberMap;
    QUndoStack *m_undoStack;
};

#endif // GRAPHLOGIC_H

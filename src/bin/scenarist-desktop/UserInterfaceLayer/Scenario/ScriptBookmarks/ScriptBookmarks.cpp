#include "ScriptBookmarks.h"

#include <3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QTreeView>
#include <QVBoxLayout>

using UserInterface::ScriptBookmarks;


ScriptBookmarks::ScriptBookmarks(QWidget* _parent)
    : QWidget(_parent),
      m_items(new QTreeView(this))
{
    initView();
    initConnections();
    initStyleSheet();
}

void ScriptBookmarks::setModel(QAbstractItemModel* _model)
{
    m_items->setModel(_model);
}

void ScriptBookmarks::setCommentOnly(bool _isCommentOnly)
{

}

void ScriptBookmarks::initView()
{
    m_items->setHeaderHidden(true);
    TreeViewItemDelegate* delegate = new TreeViewItemDelegate(m_items);
    delegate->setNeedColorize(false);
    m_items->setItemDelegate(delegate);
    m_items->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_items->setSelectionMode(QAbstractItemView::SingleSelection);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(QMargins());
//    topLayout->addWidget(m_types);
//    topLayout->addWidget(m_addItem);
//    topLayout->addWidget(m_removeItem);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(QMargins());
    layout->addLayout(topLayout);
    layout->addWidget(m_items);
    setLayout(layout);
}

void ScriptBookmarks::initConnections()
{

}

void ScriptBookmarks::initStyleSheet()
{
//    m_types->setProperty("inTopPanel", true);
//    m_types->setProperty("topPanelLeftBordered", false);
//    m_types->setProperty("topPanelTopBordered", false);
//    m_addItem->setProperty("inTopPanel", true);
//    m_addItem->setProperty("topPanelTopBordered", false);
//    m_removeItem->setProperty("inTopPanel", true);
//    m_removeItem->setProperty("topPanelTopBordered", false);

    m_items->setProperty("mainContainer", true);
}

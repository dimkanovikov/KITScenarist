#include "ScriptDictionaries.h"

#include <3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QComboBox>
#include <QListView>
#include <QVBoxLayout>

using UserInterface::ScriptDictionaries;


ScriptDictionaries::ScriptDictionaries(QWidget* _parent) :
    QWidget(_parent),
    m_types(new QComboBox(this)),
    m_addItem(new FlatButton(this)),
    m_removeItem(new FlatButton(this)),
    m_items(new QListView(this))
{
    initView();
    initConnections();
    initStyleSheet();
}

void ScriptDictionaries::initView()
{
    m_types->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_types->insertItems(0, { tr("Scene intros"),
                              tr("Scene times"),
                              tr("Scene days"),
                              tr("Character states"),
                              tr("Transitions") });

    m_addItem->setIcons(QIcon(":/Graphics/Icons/Editing/add.png"));
    m_addItem->setToolTip(tr("Add dictionary item"));

    m_removeItem->setIcons(QIcon(":/Graphics/Icons/Editing/delete.png"));
    m_removeItem->setToolTip(tr("Remove selected item"));

    m_items->setItemDelegate(new TreeViewItemDelegate(m_items));
    m_items->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_items->setSelectionMode(QAbstractItemView::SingleSelection);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(QMargins());
    topLayout->addWidget(m_types);
    topLayout->addWidget(m_addItem);
    topLayout->addWidget(m_removeItem);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(QMargins());
    layout->addLayout(topLayout);
    layout->addWidget(m_items);
    setLayout(layout);
}

void ScriptDictionaries::initConnections()
{

}

void ScriptDictionaries::initStyleSheet()
{
    m_types->setProperty("inTopPanel", true);
    m_addItem->setProperty("inTopPanel", true);
    m_removeItem->setProperty("inTopPanel", true);

    m_items->setProperty("mainContainer", true);
}

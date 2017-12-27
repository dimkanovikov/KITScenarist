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

void ScriptDictionaries::setModel(QAbstractItemModel* _model)
{
    m_items->setModel(_model);
}

void ScriptDictionaries::setCommentOnly(bool _isCommentOnly)
{
    m_addItem->setVisible(!_isCommentOnly);
    m_removeItem->setVisible(!_isCommentOnly);
}

void ScriptDictionaries::initView()
{
    m_types->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_types->addItem(tr("Scene intros"), static_cast<int>(Type::SceneIntros));
    m_types->addItem(tr("Scene times"), static_cast<int>(Type::SceneTimes));
    m_types->addItem(tr("Scene days"), static_cast<int>(Type::SceneDays));
    m_types->addItem(tr("Character states"), static_cast<int>(Type::CharacterStates));
    m_types->addItem(tr("Transitions"), static_cast<int>(Type::Transitions));

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
    connect(m_types, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this] (int _index) {
        emit dictionaryChanged(m_types->itemData(_index).toInt());
    });
    connect(m_addItem, &FlatButton::clicked, this, &ScriptDictionaries::addItemRequested);
    connect(m_removeItem, &FlatButton::clicked, [this] { emit removeItemRequested(m_items->currentIndex()); });
}

void ScriptDictionaries::initStyleSheet()
{
    m_types->setProperty("inTopPanel", true);
    m_types->setProperty("topPanelLeftBordered", false);
    m_types->setProperty("topPanelTopBordered", false);
    m_addItem->setProperty("inTopPanel", true);
    m_addItem->setProperty("topPanelTopBordered", false);
    m_removeItem->setProperty("inTopPanel", true);
    m_removeItem->setProperty("topPanelTopBordered", false);

    m_items->setProperty("mainContainer", true);
}

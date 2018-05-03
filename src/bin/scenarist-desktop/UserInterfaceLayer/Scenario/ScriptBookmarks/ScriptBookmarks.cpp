#include "ScriptBookmarks.h"

#include <3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QLabel>
#include <QTreeView>
#include <QVBoxLayout>

using UserInterface::ScriptBookmarks;


ScriptBookmarks::ScriptBookmarks(QWidget* _parent) :
    QWidget(_parent),
    m_topEmptyLabel(new QLabel(this)),
    m_edit(new FlatButton(this)),
    m_remove(new FlatButton(this)),
    m_items(new QTreeView(this))
{
    initView();
    initConnections();
    initStyleSheet();
}

void ScriptBookmarks::setModel(QAbstractItemModel* _model)
{
    m_items->setModel(_model);
    connect(m_items->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [this] (const QModelIndex& _currentIndex) {
        m_edit->setEnabled(_currentIndex.isValid());
        m_remove->setEnabled(_currentIndex.isValid());

        emit bookmarkSelected(_currentIndex);
    });
}

void ScriptBookmarks::setCurrentIndex(const QModelIndex& _index)
{
    m_items->setCurrentIndex(_index);
}

void ScriptBookmarks::setCommentOnly(bool _isCommentOnly)
{
    m_edit->setVisible(!_isCommentOnly);
    m_remove->setVisible(!_isCommentOnly);
}

void ScriptBookmarks::initView()
{
    m_topEmptyLabel->setText(tr("Bookmarks"));

    m_edit->setIcons(QIcon(":/Graphics/Iconset/pencil.svg"));
    m_edit->setToolTip(tr("Edit selected bookmark"));

    m_remove->setIcons(QIcon(":/Graphics/Iconset/delete.svg"));
    m_remove->setToolTip(tr("Remove selected bookmark"));

    m_items->setHeaderHidden(true);
    TreeViewItemDelegate* delegate = new TreeViewItemDelegate(m_items);
    delegate->setNeedColorize(false);
    m_items->setItemDelegate(delegate);
    m_items->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_items->setSelectionMode(QAbstractItemView::SingleSelection);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(QMargins());
    topLayout->addWidget(m_topEmptyLabel);
    topLayout->addWidget(m_edit);
    topLayout->addWidget(m_remove);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(QMargins());
    layout->addLayout(topLayout);
    layout->addWidget(m_items);
    setLayout(layout);
}

void ScriptBookmarks::initConnections()
{
    connect(m_edit, &FlatButton::clicked, this, &ScriptBookmarks::notifyBookmarkEditPressed);
    connect(m_remove, &FlatButton::clicked, this, [this] {
        if (!m_items->currentIndex().isValid()) {
            return;
        }

        emit removeBookmarkPressed(m_items->currentIndex());
    });
    connect(m_items, &QTreeView::doubleClicked, this, &ScriptBookmarks::notifyBookmarkEditPressed);
}

void ScriptBookmarks::initStyleSheet()
{
    m_topEmptyLabel->setProperty("inTopPanel", true);
    m_topEmptyLabel->setProperty("topPanelLeftBordered", false);
    m_topEmptyLabel->setProperty("topPanelTopBordered", false);
    m_edit->setProperty("inTopPanel", true);
    m_edit->setProperty("topPanelTopBordered", false);
    m_remove->setProperty("inTopPanel", true);
    m_remove->setProperty("topPanelTopBordered", false);

    m_items->setProperty("mainContainer", true);
}

void ScriptBookmarks::notifyBookmarkEditPressed()
{
    if (!m_items->currentIndex().isValid()) {
        return;
    }

    emit editBookmarkPressed(m_items->currentIndex());
}

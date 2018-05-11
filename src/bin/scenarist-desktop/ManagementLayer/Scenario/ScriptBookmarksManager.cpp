#include "ScriptBookmarksManager.h"

#include <BusinessLayer/ScenarioDocument/ScriptBookmarksModel.h>

#include <UserInterfaceLayer/Scenario/ScriptBookmarks/BookmarkDialog.h>
#include <UserInterfaceLayer/Scenario/ScriptBookmarks/ScriptBookmarks.h>

using ManagementLayer::ScriptBookmarksManager;
using UserInterface::BookmarkDialog;
using UserInterface::ScriptBookmarks;


ScriptBookmarksManager::ScriptBookmarksManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new ScriptBookmarks(_parentWidget))
{
    initView();
    initConnection();
}

QWidget* ScriptBookmarksManager::view() const
{
    return m_view;
}

void ScriptBookmarksManager::setBookmarksModel(BusinessLogic::ScriptBookmarksModel* _model)
{
    m_model = _model;
    m_view->setModel(m_model);
}

void ScriptBookmarksManager::setCommentOnly(bool _isCommentOnly)
{
    m_view->setCommentOnly(_isCommentOnly);
}

void ScriptBookmarksManager::addBookmark(int _position)
{
    BookmarkDialog dlg(m_view, true);
    if (dlg.exec() == QLightBoxDialog::Rejected) {
        return;
    }

    emit addBookmarkRequested(_position, dlg.bookmarkText(), dlg.bookmarkColor());
    selectBookmark(_position);
}

void ScriptBookmarksManager::editBookmark(const QModelIndex& _index)
{
    if (m_model == nullptr
        || !_index.isValid()) {
        return;
    }

    BookmarkDialog dlg(m_view, false);
    dlg.setBookmarkText(_index.data(Qt::DisplayRole).toString());
    dlg.setBookmarkColor(_index.data(BusinessLogic::ScriptBookmarksModel::ColorRole).value<QColor>());
    if (dlg.exec() == QLightBoxDialog::Rejected) {
        return;
    }

    const int position = m_model->positionForIndex(_index);
    emit addBookmarkRequested(position, dlg.bookmarkText(), dlg.bookmarkColor());
}

void ScriptBookmarksManager::removeBookmark(const QModelIndex& _index)
{
    //
    // Снимаем выделение в списке, чтобы после удаления, текст не перескачил к следующей закладке
    //
    m_view->setCurrentIndex(QModelIndex());
    //
    // и удаляем заданный элемент
    //
    emit removeBookmarkRequested(m_model->positionForIndex(_index));
}

void ScriptBookmarksManager::selectBookmark(const QModelIndex& _index)
{
    if (m_model == nullptr) {
        return;
    }

    emit bookmarkSelected(m_model->positionForIndex(_index));
}

void ScriptBookmarksManager::selectBookmark(int _position)
{
    QSignalBlocker blocker(this);
    m_view->setCurrentIndex(m_model->indexForPosition(_position));
}

void ScriptBookmarksManager::initView()
{
    m_view->setObjectName("scenarioScriptBookmarksView");
}

void ScriptBookmarksManager::initConnection()
{
    connect(m_view, &ScriptBookmarks::editBookmarkPressed, this, &ScriptBookmarksManager::editBookmark);
    connect(m_view, &ScriptBookmarks::removeBookmarkPressed, this, &ScriptBookmarksManager::removeBookmark);
    connect(m_view, &ScriptBookmarks::bookmarkSelected, this, static_cast<void (ScriptBookmarksManager::*)(const QModelIndex&)>(&ScriptBookmarksManager::selectBookmark));
}

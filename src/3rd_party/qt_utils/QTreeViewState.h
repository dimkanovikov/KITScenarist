#ifndef QTREEVIEWSTATE_H
#define QTREEVIEWSTATE_H

#include <QTreeView>
#include <QDebug>
#include <QScrollBar>

/**
 * Stores the expanded and selected state of items in a QTreeView.
 * The class can be used with any TreeView.
 * Usage:
 * \code
 *  TreeViewState state = TreeViewState::storeState(myview);
 *  ...
 *  TreeViewState::restoreState(myview,state);
 *  \endcode
 */
class QTreeViewState
{
 public:
	static QTreeViewState storeState(QTreeView* view);
	static void restoreState(QTreeView* view, QTreeViewState& state);

	QTreeViewState() {}
	QTreeViewState& operator=(const QTreeViewState &other)
	{
		expandedChildren = other.expandedChildren;
		selectedChildren = other.selectedChildren;
		scrollValue = other.scrollValue;
		return *this;
	}
	QTreeViewState(const QTreeViewState& other)
	{
		this->expandedChildren = other.expandedChildren;
		this->selectedChildren = other.selectedChildren;
		this->scrollValue = other.scrollValue;
	}

private:
	QHash<QString,QTreeViewState> expandedChildren;
	QHash<QString,QTreeViewState> selectedChildren;
	int scrollValue;

	QString pathOfIndex(const QModelIndex& _index) const;
	void saveIndex(QTreeView* view,QModelIndex parent);
	void resetSubTree(QTreeView* view, QModelIndex ind);
};

inline QString QTreeViewState::pathOfIndex(const QModelIndex& _index) const
{
	QString parentPath;
	QString selfPath;
	if (_index.isValid()) {
		parentPath = pathOfIndex(_index.parent());
		selfPath = QString("%1:%2:%3").arg(parentPath).arg(_index.row()).arg(_index.column());
	}
	return selfPath;
}

/**
 * saves the expanded and selected state of an index
 */
inline void QTreeViewState::saveIndex(QTreeView* view,QModelIndex parent)
{
	for(int i =0 ; i< view->model()->rowCount(parent); ++i )
	{
		QModelIndex p = view->model()->index(i,0,parent);
		if(view->isExpanded(p))
		{
			QTreeViewState child;
//			ERS_DEBUG(0,"This is expanded : " << view->model()->data(p).toString().toStdString());
			child.saveIndex(view,p);
			this->expandedChildren.insert(pathOfIndex(p),child);

		}
		if(view->selectionModel()->isSelected(p))
		{
			QTreeViewState child;
//			ERS_DEBUG(0,"This is selected : " << view->model()->data(p).toString().toStdString());
			this->selectedChildren.insert(pathOfIndex(p),child);
		}

	}
}

/**
 * This method will save the entire layout of the TreeView.
 * To be used to keep state between model rebuilds
 */
inline QTreeViewState QTreeViewState::storeState(QTreeView* view)
{
	QAbstractItemModel* model = view->model();
	QTreeViewState state;
	if(model)
	{
		state.saveIndex(view,view->rootIndex());
	}

	state.scrollValue = view->verticalScrollBar()->value();
	return state;
}

/**
 * recursive method for restoring all expanded items and reselecting selected ones
 */
inline void QTreeViewState::resetSubTree(QTreeView* view, QModelIndex ind)
{
	if(ind.isValid())
	{
		view->setExpanded(ind,true);
	}
	for(int i =0 ; i< view->model()->rowCount(ind); ++i )
	{
		QModelIndex p = view->model()->index(i,0,ind);

		if(this->expandedChildren.contains(pathOfIndex(p)))
		{
			QTreeViewState s = this->expandedChildren.value(pathOfIndex(p));
			s.resetSubTree(view,p);
		}
		if(this->selectedChildren.contains(pathOfIndex(p)))
		{
			view->selectionModel()->select(p,QItemSelectionModel::Select);
			view->scrollTo(p);
		}
	}

}
/**
 * resets the treeview to the provided state
 */
inline void QTreeViewState::restoreState(QTreeView* view,QTreeViewState& state)
{
	view->clearSelection();
	view->collapseAll();
	if(state.expandedChildren.size() > 0 || state.selectedChildren.size() > 0)
		state.resetSubTree(view,view->rootIndex());

	if (state.scrollValue != 0) {
		view->verticalScrollBar()->setValue(state.scrollValue);
	}
}

#endif /*QTREEVIEWSTATE_H*/

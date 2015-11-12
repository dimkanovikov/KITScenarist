#ifndef HIERARCHICALTABLEMODEL_H
#define HIERARCHICALTABLEMODEL_H

#include "HierarchicalHeaderView.h"

#include <QSortFilterProxyModel>
#include <QPointer>


class HierarchicalTableModel: public QSortFilterProxyModel
{
	Q_OBJECT

public:
	HierarchicalTableModel(QObject* _parent = 0);

	QVariant data(const QModelIndex& _index, int _role = Qt::DisplayRole) const;

	void setHorizontalHeaderModel(QAbstractItemModel* _model);

	void setVerticalHeaderModel(QAbstractItemModel* _model);

private:
	QPointer<QAbstractItemModel> m_horizontalHeaderModel;
	QPointer<QAbstractItemModel> m_verticalHeaderModel;
};

#endif // HIERARCHICALTABLEMODEL_H

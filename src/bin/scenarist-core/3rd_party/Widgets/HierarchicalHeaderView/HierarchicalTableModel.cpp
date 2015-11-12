#include "HierarchicalTableModel.h"


HierarchicalTableModel::HierarchicalTableModel(QObject* _parent)
	: QSortFilterProxyModel(_parent)
{}

QVariant HierarchicalTableModel::data(const QModelIndex& _index, int _role) const
{
	QVariant result;
	if (m_horizontalHeaderModel && _role == HierarchicalHeaderView::HorizontalHeaderDataRole) {
		result.setValue((QObject*)m_horizontalHeaderModel.data());
	} else if (m_verticalHeaderModel && _role == HierarchicalHeaderView::VerticalHeaderDataRole) {
		result.setValue((QObject*)m_verticalHeaderModel.data());
	} else {
		result = QSortFilterProxyModel::data(_index, _role);
	}

	return result;
}

void HierarchicalTableModel::setHorizontalHeaderModel(QAbstractItemModel* _model)
{
	m_horizontalHeaderModel = _model;
	if (sourceModel()->columnCount()) {
		emit headerDataChanged(Qt::Horizontal, 0, sourceModel()->columnCount() - 1);
	}
}

void HierarchicalTableModel::setVerticalHeaderModel(QAbstractItemModel* _model)
{
	m_verticalHeaderModel = _model;
	if (sourceModel()->rowCount()) {
		emit headerDataChanged(Qt::Vertical, 0, sourceModel()->rowCount() - 1);
	}
}

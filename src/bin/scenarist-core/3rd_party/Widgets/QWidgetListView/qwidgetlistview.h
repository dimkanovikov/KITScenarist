#ifndef QWIDGETLISTVIEW_H
#define QWIDGETLISTVIEW_H

#include <QModelIndex>
#include <QScrollArea>

class QAbstractItemModel;
class QVBoxLayout;
class QtModelWidget;


class QWidgetListView : public QScrollArea
{
	Q_OBJECT

public:
	explicit QWidgetListView(QWidget* _parent = 0);

	void setMetaObject(const QMetaObject* _metaObject);

	void setModel(QAbstractItemModel* _model);
	QAbstractItemModel* model() const;


	void setSelectedItem(const QModelIndex& _index);

	void scrollTo(const QModelIndex& _index);

signals:
	void clicked(const QModelIndex& _index);

private slots:
	void onDataChanged(const QModelIndex& _topLeft, const QModelIndex& _bottomRight);
	void populateModel();

	void aboutModelRowsInserted(const QModelIndex& _parent, int _first, int _last);

	void aboutModelRowsRemoved(const QModelIndex& _parent, int _first, int _last);

	/**
	 * @brief Щелчёк мышью на элементе
	 */
	void aboutItemClicked();

private:
	QVBoxLayout* m_layout;

	const QMetaObject* m_metaObject;
	QList<QtModelWidget*> m_widgets;
	QAbstractItemModel* m_model;
	QModelIndex m_lastSelected;
};

#endif // QWIDGETLISTVIEW_H

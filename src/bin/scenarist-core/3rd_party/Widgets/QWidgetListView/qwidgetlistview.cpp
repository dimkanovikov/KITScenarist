#include "qwidgetlistview.h"
#include "qtmodelwidget.h"

#include <QEvent>
#include <QScrollBar>
#include <QVBoxLayout>


QWidgetListView::QWidgetListView(QWidget* _parent) :
	QScrollArea(_parent),
	m_layout(new QVBoxLayout),
	m_metaObject(0),
	m_model(0)
{
	m_layout->setContentsMargins(QMargins());
	m_layout->setSpacing(0);
	m_layout->addStretch();

	QWidget* contentsWidget = new QWidget(this);
	contentsWidget->setLayout(m_layout);

	setWidget(contentsWidget);
	setWidgetResizable(true);
}

void QWidgetListView::setMetaObject(const QMetaObject* _metaObject)
{
	m_metaObject = _metaObject;
}

void QWidgetListView::setModel(QAbstractItemModel* _model)
{
	if (m_model != _model) {
		// disconnect signals from old model
		if (m_model != 0) {
			disconnect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
			disconnect(m_model, SIGNAL(layoutChanged()), this, SLOT(populateModel())); // cheat: just redo the whole view if layout changes
			disconnect(m_model, SIGNAL(modelReset()), this, SLOT(populateModel())); // not sure precisely what this is supposed to do; so we cheat
			disconnect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(aboutModelRowsInserted(QModelIndex,int,int)));
			disconnect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(aboutModelRowsRemoved(QModelIndex,int,int)));
		}

		// connect signals to new model
		connect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
		connect(_model, SIGNAL(layoutChanged()), this, SLOT(populateModel())); // cheat: just redo the whole view if layout changes
		connect(_model, SIGNAL(modelReset()), this, SLOT(populateModel())); // not sure precisely what this is supposed to do; so we cheat
		connect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(aboutModelRowsInserted(QModelIndex,int,int)));
		connect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(aboutModelRowsRemoved(QModelIndex,int,int)));


		m_model = _model;
		populateModel();
	}
}

QAbstractItemModel* QWidgetListView::model() const
{
	return m_model;
}

void QWidgetListView::setSelectedItem(const QModelIndex& _index)
{
	if (m_lastSelected.isValid()
		&& m_widgets.size() > m_lastSelected.row()) {
		m_widgets.at(m_lastSelected.row())->setIsSelected(false);
		m_widgets.at(m_lastSelected.row())->dataChanged();
	}

	m_lastSelected = _index;

	if (m_lastSelected.isValid()) {
		m_widgets.at(m_lastSelected.row())->setIsSelected(true);
		m_widgets.at(m_lastSelected.row())->dataChanged();
	}
}

void QWidgetListView::scrollTo(const QModelIndex& _index)
{
	if (_index.isValid()) {
		const QWidget* scrollToWidget = m_widgets.at(_index.row());
		const QPoint widgetPos = scrollToWidget->mapTo(this, scrollToWidget->pos());

		//
		// NOTE: Почему-то при преобразовании координат относительно поля прокрутки y-координата
		//		 вырастает в два раза. По-видимому к y-координате добавляется позиция ползунка.
		//
		const int widgetTop = (widgetPos.y() - verticalScrollBar()->sliderPosition()) / 2;
		const int widgetBottom = widgetTop + scrollToWidget->height();

		// Move slider if scrolling available and widget invisible
		if (verticalScrollBar()->maximum() > 0
			&& (widgetTop < 0 || widgetBottom > height())) {

			const int pixelMax = widget()->height();
			const int logicalMax = verticalScrollBar()->maximum();
			const int sliderStepInPx = pixelMax / logicalMax;

			if (sliderStepInPx > 0) {
				int moveToPx = 0;
				// Move up
				if (widgetPos.y() < 0) {
					moveToPx = widgetTop;
				}
				// ... or down
				else {
					moveToPx = widgetBottom - height();
				}
				const int newSliderPosition = verticalScrollBar()->sliderPosition() + moveToPx;
				verticalScrollBar()->setSliderPosition(newSliderPosition);
			}
		}
	}
}

void QWidgetListView::onDataChanged(const QModelIndex& _topLeft, const QModelIndex& _bottomRight)
{
	// send notifications to all the widgets that their data changed
	int bottomRow = _bottomRight.row() + 1;
	for (int i = _topLeft.row(); i != bottomRow; ++i) {
		QtModelWidget *widget = m_widgets.at(i);

		// tell it its data changed so it can reposition UI
		widget->dataChanged();
	}
}

void QWidgetListView::populateModel()
{
	const int modelSize = m_model->rowCount();
	if (modelSize > 0) {
		aboutModelRowsRemoved(QModelIndex(), 0, modelSize - 1);
		aboutModelRowsInserted(QModelIndex(), 0, modelSize - 1);
	}
}

void QWidgetListView::aboutModelRowsInserted(const QModelIndex& _parent, int _first, int _last)
{
	Q_UNUSED(_parent);

	int itemIndex = _first;
	for (; itemIndex <= _last; ++itemIndex) {
		QtModelWidget* widget =
				static_cast<QtModelWidget *>(
					m_metaObject->newInstance(
						Q_ARG(QAbstractItemModel*, m_model),
						Q_ARG(QModelIndex, m_model->index(itemIndex, 0))
						)
					);
		// if this asserts, you either:
		// 1) have the wrong constructor arguments
		// 2) forgot to mark it Q_INVOKABLE.
		Q_ASSERT(widget);

		// parenting for layout changes
		widget->setParent(this);
		connect(widget, SIGNAL(clicked()), this, SLOT(aboutItemClicked()));

		m_widgets.insert(itemIndex, widget);
		m_layout->insertWidget(itemIndex, widget);
	}

	for (; itemIndex < m_widgets.size(); ++itemIndex) {
		m_widgets[itemIndex]->setIndex(m_model->index(itemIndex, 0));
	}

	onDataChanged(m_model->index(_first, 0), m_model->index(_last, 0));
}

void QWidgetListView::aboutModelRowsRemoved(const QModelIndex& _parent, int _first, int _last)
{
	Q_UNUSED(_parent);

	int itemIndex = _last;
	for (; itemIndex >= _first; --itemIndex) {
		QtModelWidget* widget = m_widgets.takeAt(itemIndex);
		m_layout->removeWidget(widget);
		disconnect(widget, SIGNAL(clicked()), this, SLOT(aboutItemClicked()));
		widget->deleteLater();
	}

	itemIndex = _first;
	for (; itemIndex < m_widgets.size(); ++itemIndex) {
		m_widgets[itemIndex]->setIndex(m_model->index(itemIndex, 0));
	}
}

void QWidgetListView::aboutItemClicked()
{
	if (QtModelWidget* widget = qobject_cast<QtModelWidget*>(sender())) {
		const int row = m_widgets.indexOf(widget);
		const QModelIndex index = m_model->index(row, 0);
		setSelectedItem(index);
		emit clicked(index);
	}
}


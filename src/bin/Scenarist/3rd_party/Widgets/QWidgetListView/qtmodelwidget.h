/*
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2010 Robin Burchell <robin.burchell@collabora.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef AWIDGET_H
#define AWIDGET_H

#include <QModelIndex>
#include <QWidget>
class QAbstractItemModel;

/** QtModelWidget is a simple subclass.
  * It represents the base class of all widgets that may be inserted into an QtWidgetListView.
  *
  * A QtModelWidget is sort of like QWidget and QAbstractItemDelegate eloped, made sweet, sweet love,
  * and had a baby.
  *
  * The difference with QtModelWidget and a delegate is you probably won't be doing the painting yourself:
  * you'll just be altering 'building block' widgets you put on your UI with Designer or something.
  *
  * Things to remember when writing a QtModelWidget subclass:
  *   * You must implement dataChanged().
  *     dataChanged() is automatically invoked when the data for your QModelIndex is changed,
  *     and as such, you need to update your UI as appropriate (and, if you're doing custom painting,
  *     remember to call QWidget::update())
  *   * You must have a constructor with the same signal as QtModelWidget's constructor, that
  *     invokes QtModelWidget's constructor.
  *   * You must mark your constructor Q_INVOKABLE!
  *
  * These are non-negotiable. It's just how the system works.
  */
class QtModelWidget : public QWidget
{
	Q_OBJECT
public:
	Q_INVOKABLE
	explicit QtModelWidget(QAbstractItemModel *model, QModelIndex index);

	/** dataChanged() is called whenever data for this particular item in the model has been altered.
	  * You should get the data from model() (using index()) and set up/alter your GUI using it, much
	  * as you would in a delegate.
	  */
	virtual void dataChanged();

	bool isSelected() const;
	void setIsSelected(bool _selected);

	/** Use this to help locate your data inside the model.
	  * @returns the QModelIndex for this widget in the model.
	  */
	QModelIndex index() const;
	void setIndex(const QModelIndex& _index);

	/** Use this to return or change data.
	  * @returns the model this widget is associated with
	  */
	QAbstractItemModel *model() const;

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent* _event);

private:
	QAbstractItemModel *m_model;
	QModelIndex m_index;
	bool m_isSelected;
};

#endif // AWIDGET_H

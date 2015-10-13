/*
Copyright (c) 2009, Krasnoshchekov Petr
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the <organization> nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Krasnoshchekov Petr ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Krasnoshchekov Petr BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef HIERARCHICAL_HEADER_VIEW_H
#define HIERARCHICAL_HEADER_VIEW_H

#include <QHeaderView>


/**
Hierarchical header view for QTableView.
Header data is described by QAbstractItemModel.
Table model's function "data" should return pointer to QAbstractItemModel, representing header structure,
for horizontal header it uses HorizontalHeaderDataRole, for vertical - VerticalHeaderDataRole.
If header's model function "data" with Qt::UserRole return valid QVariant, text in header cell is rotated.
*/
class HierarchicalHeaderView : public QHeaderView
{
	Q_OBJECT

public:

	enum HeaderDataModelRoles {
		HorizontalHeaderDataRole = Qt::UserRole,
		VerticalHeaderDataRole = Qt::UserRole + 1
	};

	HierarchicalHeaderView(Qt::Orientation orientation, QWidget* parent = 0);
	~HierarchicalHeaderView();

	void setModel(QAbstractItemModel* model);

protected:
	void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;

	QSize sectionSizeFromContents(int logicalIndex) const;

private slots:
	void on_sectionResized(int logicalIndex);

private:
	class private_data;
	private_data* _pd;

	QStyleOptionHeader styleOptionForCell(int logicalIndex) const;
};

#endif

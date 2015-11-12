#include "LocationsNavigatorItemWidget.h"

#include <3rd_party/Widgets/ElidedLabel/ElidedLabel.h>

#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <QHBoxLayout>

using UserInterface::LocationsNavigatorItemWidget;


LocationsNavigatorItemWidget::LocationsNavigatorItemWidget(QWidget* _parent) :
	QWidget(_parent)
{
	m_name = new ElidedLabel(this);
	m_name->setMinimumSize(0, 24);
	m_name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_name->setElideMode(Qt::ElideRight);
	m_name->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(m_name);
	layout->setContentsMargins(QMargins(4, 0, 0, 0));

	this->setLayout(layout);
}

void LocationsNavigatorItemWidget::setName(const QString& _name)
{
	m_name->setText(_name.toUpper());
}

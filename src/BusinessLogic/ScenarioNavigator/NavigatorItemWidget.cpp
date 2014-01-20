#include "NavigatorItemWidget.h"

#include <UserInterface/Widgets/ElidedLabel/ElidedLabel.h>
#include <BusinessLogic/Chronometry/ChronometerFacade.h>

#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QTime>


NavigatorItemWidget::NavigatorItemWidget(QWidget *parent) :
	QWidget(parent)
{
	m_icon = new QLabel(this);
	m_icon->setFixedSize(32, 32);

	m_header = new ElidedLabel(this);
	m_header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	QFont headerFont = m_header->font();
	headerFont.setBold(true);
	m_header->setFont(headerFont);
	m_header->setElideMode(Qt::ElideRight);

	m_description = new ElidedLabel(this);
	m_description->setElideMode(Qt::ElideRight);
	m_description->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_timing = new QLabel(this);
	m_timing->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QFont timingFont = m_timing->font();
	timingFont.setPointSize(timingFont.pointSize() - 2);
	m_timing->setFont(timingFont);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->addWidget(m_header);
	topLayout->addWidget(m_timing);
	topLayout->setContentsMargins(QMargins());

	QVBoxLayout* rightLayout = new QVBoxLayout;
	rightLayout->addLayout(topLayout);
	rightLayout->addWidget(m_description);
	rightLayout->setContentsMargins(QMargins());
	rightLayout->setSpacing(1);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(m_icon);
	layout->addLayout(rightLayout);
	layout->setContentsMargins(QMargins());

	this->setLayout(layout);
}

void NavigatorItemWidget::setIcon(const QPixmap& _icon)
{
	m_icon->setPixmap(_icon);
}

void NavigatorItemWidget::setHeader(const QString& _header)
{
	m_header->setText(_header.toUpper());
}

void NavigatorItemWidget::setDescription(const QString& _description)
{
	m_description->setText(_description);
}

void NavigatorItemWidget::setTiming(int _timing)
{
	m_timing->setText(
				"("
				+ ChronometerFacade::secondsToTime(_timing)
				+ ")");
}

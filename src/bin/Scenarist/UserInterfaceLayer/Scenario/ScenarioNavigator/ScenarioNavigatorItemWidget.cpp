#include "ScenarioNavigatorItemWidget.h"

#include <3rd_party/Widgets/ElidedLabel/ElidedLabel.h>

#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QTime>

using UserInterface::ScenarioNavigatorItemWidget;


ScenarioNavigatorItemWidget::ScenarioNavigatorItemWidget(QWidget *parent) :
	QWidget(parent)
{
	m_icon = new QLabel(this);
	m_icon->setFixedSize(32, 32);
	m_icon->setScaledContents(true);

	m_header = new ElidedLabel(this);
	m_header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	QFont headerFont = m_header->font();
	headerFont.setBold(true);
	m_header->setFont(headerFont);
	m_header->setElideMode(Qt::ElideRight);
	m_header->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	m_description = new ElidedLabel(this);
	m_description->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_description->setElideMode(Qt::ElideRight);
	m_description->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	m_duration = new QLabel(this);
	m_duration->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->addWidget(m_header);
	topLayout->addWidget(m_duration);
	topLayout->setContentsMargins(QMargins());

	QVBoxLayout* rightLayout = new QVBoxLayout;
	rightLayout->addLayout(topLayout);
	rightLayout->addWidget(m_description);
	rightLayout->setContentsMargins(QMargins());
	rightLayout->setSpacing(1);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(m_icon);
	layout->addLayout(rightLayout);
	layout->setContentsMargins(QMargins(0, 2, 0, 2));

	this->setLayout(layout);
}

void ScenarioNavigatorItemWidget::setIcon(const QPixmap& _icon)
{
	m_icon->setPixmap(_icon);
}

void ScenarioNavigatorItemWidget::setHeader(const QString& _header)
{
	m_header->setText(_header.toUpper());
}

void ScenarioNavigatorItemWidget::setDescription(const QString& _description)
{
	m_description->setText(_description);
}

void ScenarioNavigatorItemWidget::setDuration(int _duration)
{
	QString duration;
	if (BusinessLogic::ChronometerFacade::chronometryUsed()) {
		duration = "(" + BusinessLogic::ChronometerFacade::secondsToTime(_duration)+ ") ";
	}

	m_duration->setText(duration);
}

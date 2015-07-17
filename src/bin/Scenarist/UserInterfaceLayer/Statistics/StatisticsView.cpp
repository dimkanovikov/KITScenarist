#include "StatisticsView.h"

#include "ChevronButton.h"

#include <3rd_party/Widgets/Ctk/ctkCollapsibleButton.h>
#include <3rd_party/Widgets/Ctk/ctkPopupWidget.h>

#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>

#include <QVariant>
#include <QVBoxLayout>

using UserInterface::StatisticsView;


StatisticsView::StatisticsView(QWidget* _parent) :
	QWidget(_parent)
{
	initView();
	initConnections();
	initStyleSheet();
}

void StatisticsView::initView()
{

	ctkCollapsibleButton* btn = new ctkCollapsibleButton("Reports", this);
	btn->setIndicatorAlignment(Qt::AlignRight);
	btn->setCollapsed(true);
	btn->setProperty("reportButton", true);

	ChevronButton* rb1 = new ChevronButton("rb 1", btn);
	ChevronButton* rb2 = new ChevronButton("rb 2", btn);
	ChevronButton* rb3 = new ChevronButton("rb 3", btn);



	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(rb1);
	layout->addWidget(rb2);
	layout->addWidget(rb3);

	btn->setLayout(layout);


	ctkCollapsibleButton* btn1 = new ctkCollapsibleButton("Reports", this);
	btn1->setIndicatorAlignment(Qt::AlignRight);
	btn1->setCollapsed(true);
	btn1->setProperty("reportButton", true);

	QRadioButton* rb11 = new QRadioButton("rb 11", btn1);
	QRadioButton* rb21 = new QRadioButton("rb 21", btn1);
	QRadioButton* rb31 = new QRadioButton("rb 31", btn1);

	QVBoxLayout* layout1 = new QVBoxLayout;
	layout1->setContentsMargins(QMargins());
	layout1->setSpacing(0);
	layout1->addWidget(rb11);
	layout1->addWidget(rb21);
	layout1->addWidget(rb31);

	btn1->setLayout(layout1);


	QButtonGroup* group = new QButtonGroup(this);
	group->addButton(rb1);
	group->addButton(rb2);
	group->addButton(rb3);
	group->addButton(rb11);
	group->addButton(rb21);
	group->addButton(rb31);


	QPushButton* btn2 = new QPushButton("popup", this);
	ctkPopupWidget* popup = new ctkPopupWidget(btn2);
//	popup->setAlignment(Qt::AlignRight | Qt::AlignTop | Qt::AlignBottom);
//	popup->setOrientation(Qt::Horizontal);
//	QHBoxLayout* popupLayout = new QHBoxLayout(popup);
//	QSlider* popupSlider = new QSlider(popup);
//	popupLayout->addWidget(popupSlider);

	QHBoxLayout* popupLayout = new QHBoxLayout(popup);
	// populate the popup with a vertical QSlider:
	QSlider* popupSlider = new QSlider(Qt::Vertical, popup);
	// add here the signal/slot connection between the slider and the spinbox
	popupLayout->addWidget(popupSlider); // Control where to display the the popup relative to the parent
	popup->setAlignment(Qt::AlignBottom | Qt::AlignLeft); // at the top left corner

	popup->setHorizontalDirection( Qt::LeftToRight ); // open outside the parent
	popup->setVerticalDirection(ctkBasePopupWidget::TopToBottom); // at the left of the spinbox sharing the top border
	// Control the animation
	popup->setAnimationEffect(ctkBasePopupWidget::ScrollEffect); // could also be FadeEffect
	popup->setOrientation(Qt::Vertical); // how to animate, could be Qt::Vertical or Qt::Horizontal|Qt::Vertical
//	popup->setEasingCurve(QEasingCurve::OutQuart); // how to accelerate the animation, QEasingCurve::Type
//	popup->setEffectDuration(100); // how long in ms.
	// Control the behavior
	popup->setAutoShow(false); // automatically open when the mouse is over the spinbox
	popup->setAutoHide(true); // automatically hide when the mouse leaves the popup or the spinbox.
	connect(btn2, SIGNAL(clicked(bool)), popup, SLOT(showPopup()));

	QVBoxLayout* ml = new QVBoxLayout;
	ml->setContentsMargins(QMargins());
	ml->setSpacing(0);
	ml->addWidget(btn);
	ml->addWidget(btn1);
	ml->addWidget(btn2);
	ml->addStretch();
	setLayout(ml);
}

void StatisticsView::initConnections()
{

}

void StatisticsView::initStyleSheet()
{

}


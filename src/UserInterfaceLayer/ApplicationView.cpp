#include "ApplicationView.h"

using UserInterface::ApplicationView;


ApplicationView::ApplicationView(QWidget *_parent) :
	QWidget(_parent)
{
}

void ApplicationView::closeEvent(QCloseEvent* _event)
{
	emit wantToClose();
}

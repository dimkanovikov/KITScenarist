#include "ApplicationView.h"

#include <QCloseEvent>
#include <QIcon>

using UserInterface::ApplicationView;


ApplicationView::ApplicationView(QWidget *_parent) :
	QWidget(_parent, Qt::Window)
{
	setWindowIcon(QIcon(":/Graphics/Icons/logo.png"));
	setWindowTitle(tr("KIT Scenarist"));
}

void ApplicationView::closeEvent(QCloseEvent* _event)
{
	//
	// Вместо реального закрытия формы испускаем сигнал сигнализирующий об этом намерении
	//

	_event->ignore();
	emit wantToClose();
}

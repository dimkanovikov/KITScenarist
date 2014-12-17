#include "ApplicationView.h"

#include <QCloseEvent>
#include <QIcon>

using UserInterface::ApplicationView;


ApplicationView::ApplicationView(QWidget *_parent) :
	QWidget(_parent, Qt::Window)
{
	setWindowIcon(QIcon(":/Graphics/Icons/logo.png"));
	setWindowTitle(tr("Scenarist"));
}

void ApplicationView::keyPressEvent(QKeyEvent* _event)
{
	//
	// Для клавиши F5 вызываем отображение в полноэкранном режиме
	//
	if (_event->key() == Qt::Key_F5) {
		_event->ignore();
		emit wantFullscreen();
	}
	//
	// Остальные клавиши обрабатываем в штатном режиме
	//
	else {
		QWidget::keyPressEvent(_event);
	}
}

void ApplicationView::closeEvent(QCloseEvent* _event)
{
	//
	// Вместо реального закрытия формы испускаем сигнал сигнализирующий об этом намерении
	//

	_event->ignore();
	emit wantToClose();
}

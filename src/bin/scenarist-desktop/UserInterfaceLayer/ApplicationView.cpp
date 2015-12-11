#include "ApplicationView.h"

#include <QCloseEvent>
#include <QIcon>
#include <QMouseEvent>
#include <QSplitter>
#include <QSplitterHandle>

using UserInterface::ApplicationView;


ApplicationView::ApplicationView(QWidget *_parent) :
	QWidget(_parent, Qt::Window)
{
	setWindowIcon(QIcon(":/Graphics/Icons/logo.png"));
	setWindowTitle(tr("KIT Scenarist"));
}

void ApplicationView::initSplittersRightClick()
{
	//
	// Для всех сплитеров добавляем функциональность - щелчок правой кнопкой, разворачивает панели
	//
	foreach (QSplitter* splitter, findChildren<QSplitter*>()) {
		if (splitter->objectName() != "mainWindowSplitter"
			&& splitter->objectName() != "draftScenarioEditSplitter") {
			splitter->handle(1)->installEventFilter(this);
		}
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

bool ApplicationView::eventFilter(QObject* _object, QEvent* _event)
{
	if (QSplitterHandle* splitterHandle = qobject_cast<QSplitterHandle*>(_object)) {
		if (_event->type() == QEvent::MouseButtonPress) {
			QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(_event);
			if (mouseEvent->button() == Qt::RightButton) {
				//
				// Меняем виджеты в сплитере местами, это приводит к созданию нового хэндла
				// для виджетов и поэтому мы ловим и его события тоже
				//
				QSplitter* splitter = splitterHandle->splitter();
				splitter->addWidget(splitter->widget(0));
				splitter->handle(splitter->indexOf(splitterHandle) + 1)->installEventFilter(this);
			}
		}
	}

	return QWidget::eventFilter(_object, _event);
}

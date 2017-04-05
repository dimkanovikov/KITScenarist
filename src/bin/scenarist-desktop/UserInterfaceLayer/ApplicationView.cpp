#include "ApplicationView.h"

#include "_tools/UIConfigurator.h"

#include <QCloseEvent>
#include <QIcon>
#include <QMouseEvent>
#include <QScrollBar>
#include <QShortcut>
#include <QSplitter>
#include <QSplitterHandle>
#include <QTimeLine>

using UserInterface::ApplicationView;


ApplicationView::ApplicationView(QWidget *_parent) :
	QWidget(_parent, Qt::Window)
{
	setWindowIcon(QIcon(":/Graphics/Icons/logo.png"));
	setWindowTitle(tr("KIT Scenarist"));

#ifdef Q_OS_WIN
	static UIConfigurator* s_uiConfigurator = new UIConfigurator(this);
    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+Shift+1"), this);
	connect(shortcut, &QShortcut::activated, s_uiConfigurator, &UIConfigurator::show);
#endif
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

void ApplicationView::initScrollBarsWidthChanges()
{
    //
    // Для всех полос прокрутки добавляем функциональность - при наведении они расширяются
    //
    foreach (QScrollBar* scrollBar, findChildren<QScrollBar*>()) {
        scrollBar->installEventFilter(this);
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
    } else if (QScrollBar* scrollBar = qobject_cast<QScrollBar*>(_object)) {
        static QTimeLine* timeline = nullptr;
        if (timeline == nullptr) {
            timeline = new QTimeLine(60, this);
            timeline->setFrameRange(96, 150);
            timeline->setCurveShape(QTimeLine::LinearCurve);
        }

        if (_event->type() == QEvent::Enter) {
            timeline->stop();
            timeline->setDirection(QTimeLine::Forward);
            timeline->disconnect();
            const QString scaleProperty = scrollBar->orientation() == Qt::Vertical ? "width" : "height";
            const QString styleSheetTemplate =
                    "QScrollBar { " + scaleProperty + ": %1em; border-radius: %2em; }"
                    "QScrollBar::handle { border-radius: %2em;}";
            connect(timeline, &QTimeLine::frameChanged, [=] (int _frame) {
                scrollBar->setStyleSheet(styleSheetTemplate.arg(qreal(_frame)/100.).arg(qreal(_frame/2 - 12)/100.));
            });
            timeline->start();
        } else if (_event->type() == QEvent::Leave) {
            const bool resume = timeline->state() == QTimeLine::Running;
            timeline->stop();
            timeline->setDirection(QTimeLine::Backward);
            if (resume) {
                timeline->resume();
            } else {
                timeline->start();
            }
        }
    }

	return QWidget::eventFilter(_object, _event);
}

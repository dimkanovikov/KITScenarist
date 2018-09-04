#include "ApplicationView.h"

#include <UserInterfaceLayer/_tools/UIConfigurator.h>

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
    setWindowTitle(tr("Digipitch Screenwriter"));

    static UIConfigurator* s_uiConfigurator = new UIConfigurator(this);
    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+Shift+1"), this);
    connect(shortcut, &QShortcut::activated, s_uiConfigurator, &UIConfigurator::show);
}

void ApplicationView::initSplittersRightClick()
{
    //
    // Для всех сплитеров добавляем функциональность - щелчок правой кнопкой, разворачивает панели
    //
    foreach (QSplitter* splitter, findChildren<QSplitter*>()) {
        if (splitter->objectName() != "mainWindowSplitter"
            && splitter->objectName() != "navigatorScriptEditSplitter") {
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

void ApplicationView::setUseDarkTheme(bool _use)
{
    if (m_useDarkTheme != _use) {
        m_useDarkTheme = _use;
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
        //
        // Анимируем ширину полосы прокрутки, если есть возможность скроллинга
        //
        if (scrollBar->minimum() < scrollBar->maximum()) {
            static QTimeLine* timeline = nullptr;
            const int startFrame = 10;
            const int lastFrame = 20;
            if (timeline == nullptr) {
                timeline = new QTimeLine(60, this);
                timeline->setFrameRange(startFrame, lastFrame);
                timeline->setCurveShape(QTimeLine::LinearCurve);
            }

            if (_event->type() == QEvent::Enter) {
                timeline->stop();
                timeline->setDirection(QTimeLine::Forward);
                timeline->disconnect();
                const QString scaleProperty = scrollBar->orientation() == Qt::Vertical ? "width" : "height";
                const QString styleSheetTemplate ="QScrollBar { " + scaleProperty + ": %1px; }";
                connect(timeline, &QTimeLine::frameChanged, [=] (int _frame) {
                    QString styleSheet = styleSheetTemplate.arg(_frame);
                    if (_frame == lastFrame) {
                        styleSheet.append(
                                    QString("QScrollBar::add-line:vertical { padding: 0px 5px 0px 5px; image: url(:/Interface/UI/downarrow%1.png); }"
                                            "QScrollBar::sub-line:vertical { padding: 0px 5px 0px 5px; image: url(:/Interface/UI/uparrow%1.png) };")
                                    .arg(m_useDarkTheme ? "-dark" : ""));
                    }
                    scrollBar->setStyleSheet(styleSheet);
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
    }

    return QWidget::eventFilter(_object, _event);
}

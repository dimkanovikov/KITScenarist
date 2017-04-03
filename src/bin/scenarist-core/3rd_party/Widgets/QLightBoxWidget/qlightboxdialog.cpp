#include "qlightboxdialog.h"

#include <QEventLoop>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>


QLightBoxDialog::QLightBoxDialog(QWidget *parent, bool _followToHeadWidget, bool _isContentStretchable) :
    QLightBoxWidget(parent, _followToHeadWidget),
    m_initialized(false),
    m_title(new QLabel(this)),
    m_centralWidget(nullptr),
    m_progress(new QProgressBar(this)),
    m_isContentStretchable(_isContentStretchable),
    m_execResult(Rejected)
{
    updateTitle();
}

int QLightBoxDialog::exec()
{
    m_execResult = Rejected;

    show();

    QEventLoop dialogEventLoop;
    connect(this, SIGNAL(accepted()), &dialogEventLoop, SLOT(quit()));
    connect(this, SIGNAL(rejected()), &dialogEventLoop, SLOT(quit()));
    connect(this, SIGNAL(finished(int)), &dialogEventLoop, SLOT(quit()));
    dialogEventLoop.exec();

    hide();

    return m_execResult;
}

void QLightBoxDialog::setVisible(bool _visible)
{
    init();

    if (_visible) {
        focusedOnExec()->setFocus();
    }

    QLightBoxWidget::setVisible(_visible);
}

void QLightBoxDialog::accept()
{
    m_execResult = Accepted;
    emit accepted();
}

void QLightBoxDialog::reject()
{
    m_execResult = Rejected;
    emit rejected();
}

void QLightBoxDialog::done(int _result)
{
    m_execResult = _result;
    emit finished(_result);
}

bool QLightBoxDialog::isProressVisible() const
{
    //
    // Определяем скрыт ли прогрессбар по его стилю, т.к. для сокрытия используетс маскировка
    //
    return m_progress->styleSheet().isEmpty();
}

void QLightBoxDialog::showProgress(int _minimumValue, int _maximumValue)
{
    //
    // Убираем маскирующий стиль
    //
    m_progress->setStyleSheet(QString::null);

    m_progress->setRange(_minimumValue, _maximumValue);
}

void QLightBoxDialog::setProgressValue(int _value)
{
    m_progress->setValue(_value);
}

void QLightBoxDialog::hideProgress()
{
    //
    // Тут мы просто применяем маскирующий стиль для виджета
    // Делается это для того, чтобы не происходило скачков интерфейса при отображении/скрытии
    // виджета прогресса внутри компоновщика интерфейса
    //
    m_progress->setStyleSheet("QProgressBar {border: none; background-color: transparent;} QProgressBar::chunk {background-color: transparent;}");
}

bool QLightBoxDialog::event(QEvent* _event)
{
    bool result = true;
    bool needHandle = true;
    if (_event->type() == QEvent::KeyPress
        && !isProressVisible()) {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(_event);
        if (keyEvent->key() == Qt::Key_Enter
            || keyEvent->key() == Qt::Key_Return) {
            accept();
            needHandle = false;
        } else if (keyEvent->key() == Qt::Key_Escape) {
            reject();
            needHandle = false;
        }
    }

    if (needHandle) {
        result = QLightBoxWidget::event(_event);
    }

    return result;
}

QWidget* QLightBoxDialog::titleWidget() const
{
    return m_title;
}

QWidget* QLightBoxDialog::focusedOnExec() const
{
    return m_centralWidget;
}

void QLightBoxDialog::init()
{
    if (m_initialized == false) {
        m_initialized = true;

        initView();
        initConnections();

        //
        // Настраиваем заголовок
        //
        updateTitle();

        //
        // Настраиваем прогресс
        //
        m_progress->setFixedHeight(5);
        m_progress->setValue(0);
        m_progress->setRange(0, 0);
        m_progress->setFormat(QString::null);
        hideProgress();

        //
        // Настраиваем компоновщик
        //
        if (layout() != 0) {
            m_centralWidget = new QFrame(this);
            m_centralWidget->setProperty("lightBoxDialogCentralWidget", true);
            m_centralWidget->setMinimumSize(minimumSize());
            m_centralWidget->setMaximumSize(maximumSize());
            m_centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            QLayout* centralWidgetLayout = layout();
            m_centralWidget->setLayout(centralWidgetLayout);

            setMinimumSize(QSize(0, 0));
            setMaximumSize(QSize(16777215, 16777215));

            QGridLayout* newLayout = new QGridLayout;
            newLayout->setContentsMargins(QMargins());
            newLayout->setSpacing(0);
            newLayout->addWidget(titleWidget(), 1, 1);
            newLayout->addWidget(m_centralWidget, 2, 1);
            newLayout->setRowStretch(0, 1);
            if (m_isContentStretchable) {
                newLayout->setRowStretch(2, 8);
            }
            newLayout->addWidget(m_progress, 3, 1);
            newLayout->setRowStretch(4, 1);
            newLayout->setColumnStretch(0, 1);
            newLayout->setColumnStretch(2, 1);
            setLayout(newLayout);
        }
    }
}

void QLightBoxDialog::updateTitle()
{
    if (titleWidget() == m_title) {
        m_title->setProperty("lightBoxDialogTitle", true);
        m_title->setWordWrap(true);
#ifdef Q_OS_MAC
        m_title->setAlignment(Qt::AlignHCenter);
#endif
        m_title->setText(windowTitle());
        m_title->setVisible(!m_title->text().isEmpty());
    } else {
        m_title->hide();
    }
}

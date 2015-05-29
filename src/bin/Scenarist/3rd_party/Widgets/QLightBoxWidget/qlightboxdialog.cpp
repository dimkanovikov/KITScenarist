#include "qlightboxdialog.h"

#include <QEventLoop>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>


QLightBoxDialog::QLightBoxDialog(QWidget *parent, bool _followToHeadWidget) :
	QLightBoxWidget(parent, _followToHeadWidget),
	m_title(new QLabel(this)),
	m_centralWidget(0),
	m_execResult(Rejected)
{
	initView();
	initConnections();
}

int QLightBoxDialog::exec()
{
	m_execResult = Rejected;

	m_title->setText(windowTitle());
	if (m_title->text().isEmpty()) {
		m_title->hide();
	}

	show();

	m_centralWidget->setFocus();

	QEventLoop dialogEventLoop;
	connect(this, SIGNAL(accepted()), &dialogEventLoop, SLOT(quit()));
	connect(this, SIGNAL(rejected()), &dialogEventLoop, SLOT(quit()));
	connect(this, SIGNAL(finished(int)), &dialogEventLoop, SLOT(quit()));
	dialogEventLoop.exec();

	hide();

	return m_execResult;
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

bool QLightBoxDialog::event(QEvent* _event)
{
	bool result = true;
	bool needHandle = true;
	if (_event->type() == QEvent::KeyPress) {
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

void QLightBoxDialog::initView()
{
	m_title->setProperty("lightBoxDialogTitle", true);
#ifdef Q_OS_MAC
	m_title->setAlignment(Qt::AlignHCenter);
#endif

	if (layout() != 0) {
		m_centralWidget = new QWidget(this);
		m_centralWidget->setProperty("lightBoxDialogCentralWidget", true);
		m_centralWidget->setMinimumSize(minimumSize());
		m_centralWidget->setMaximumSize(maximumSize());

		QLayout* centralWidgetLayout = layout();
		centralWidgetLayout->setContentsMargins(20, 8, 20, 10);
		m_centralWidget->setLayout(centralWidgetLayout);

		setMinimumSize(QSize(0, 0));

		QGridLayout* newLayout = new QGridLayout;
		newLayout->setContentsMargins(QMargins());
		newLayout->setSpacing(0);
		newLayout->addWidget(m_title, 1, 1);
		newLayout->addWidget(m_centralWidget, 2, 1);
		newLayout->setRowStretch(0, 1);
		newLayout->setRowStretch(3, 1);
		newLayout->setColumnStretch(0, 1);
		newLayout->setColumnStretch(2, 1);
		setLayout(newLayout);
	}
}

void QLightBoxDialog::initConnections()
{
}

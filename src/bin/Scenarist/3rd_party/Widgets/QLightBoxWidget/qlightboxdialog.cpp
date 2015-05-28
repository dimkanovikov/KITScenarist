#include "qlightboxdialog.h"

#include <QEventLoop>
#include <QGridLayout>
#include <QKeyEvent>


QLightBoxDialog::QLightBoxDialog(QWidget *parent, bool _followToHeadWidget) :
	QLightBoxWidget(parent, _followToHeadWidget),
	m_centralWidget(0),
	m_execResult(Rejected)
{
	initView();
	initConnections();
}

int QLightBoxDialog::exec()
{
	m_execResult = Rejected;

	show();

	m_centralWidget->setFocus();

	QEventLoop e;
	connect(this, SIGNAL(accepted()), &e, SLOT(quit()));
	connect(this, SIGNAL(rejected()), &e, SLOT(quit()));
	connect(this, SIGNAL(finished(int)), &e, SLOT(quit()));
	e.exec();

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
	if (_event->type() == QEvent::KeyPress) {
		QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(_event);
		if (keyEvent->key() == Qt::Key_Enter
			|| keyEvent->key() == Qt::Key_Return) {
			accept();
		} else if (keyEvent->key() == Qt::Key_Escape) {
			reject();
		}
	} else {
		result = QLightBoxWidget::event(_event);
	}

	return result;
}

void QLightBoxDialog::mousePressEvent(QMouseEvent* _event)
{
	QLightBoxWidget::mousePressEvent(_event);

	if (m_centralWidget != 0) {
		if (!m_centralWidget->rect().contains(m_centralWidget->mapFromParent(_event->pos()))) {
			reject();
		}
	}
}

void QLightBoxDialog::initView()
{
	setStyleSheet("QWidget[lightBoxDialogCentralWidget=true] { background-color: palette(button); }");

	if (layout() != 0) {
		m_centralWidget = new QWidget(this);
		m_centralWidget->setProperty("lightBoxDialogCentralWidget", true);
		m_centralWidget->setMinimumSize(minimumSize());
		m_centralWidget->setMaximumSize(maximumSize());

		QLayout* centralWidgetLayout = layout();
		centralWidgetLayout->setContentsMargins(centralWidgetLayout->contentsMargins() += 20);
		m_centralWidget->setLayout(centralWidgetLayout);
		setMinimumSize(QSize(0, 0));

		QGridLayout* newLayout = new QGridLayout;
		newLayout->setContentsMargins(QMargins());
		newLayout->setSpacing(0);
		newLayout->addWidget(m_centralWidget, 1, 1);
		newLayout->setRowStretch(0, 1);
		newLayout->setRowStretch(2, 1);
		newLayout->setColumnStretch(0, 1);
		newLayout->setColumnStretch(2, 1);
		setLayout(newLayout);
	}
}

void QLightBoxDialog::initConnections()
{
}

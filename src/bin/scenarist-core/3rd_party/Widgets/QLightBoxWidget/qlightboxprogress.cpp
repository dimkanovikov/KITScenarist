#include "qlightboxprogress.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMovie>
#include <QProgressBar>


QLightBoxProgress::QLightBoxProgress(QWidget* _parent, bool _folowToHeadWidget) :
    QLightBoxWidget(_parent, _folowToHeadWidget),
    m_title(new QLabel("Title", this)),
    m_description(new QLabel("Description.")),
    m_progress(new QProgressBar(this))
{
	m_title->setProperty("lightBoxProgressTitle", true);
    m_title->setAlignment(Qt::AlignCenter);

    m_progress->setValue(0);
    m_progress->setRange(0, 0);

	m_description->setProperty("lightBoxProgressDescription", true);
    m_description->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
#ifndef MOBILE_OS
	m_description->setFixedWidth(500);
#endif
	m_description->setWordWrap(true);

	QGridLayout* layout = new QGridLayout;
    layout->setContentsMargins(QMargins());
    layout->addWidget(m_progress, 0, 0, 1, 3);
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(0, 1);
    layout->addWidget(m_title, 2, 1);
    layout->setColumnStretch(2, 1);
    layout->addWidget(m_description, 3, 1);
    layout->setRowMinimumHeight(3, 50);
    layout->setRowStretch(4, 1);
	setLayout(layout);
}

QLightBoxProgress::~QLightBoxProgress()
{
	finish();
	if (s_lastUsedWidget == this) {
		s_lastUsedWidget = 0;
	}
}

void QLightBoxProgress::showProgress(const QString& _title, const QString& _description)
{
	s_lastUsedWidget = this;

	m_title->setText(_title);
	m_description->setText(_description);

	show();
}

void QLightBoxProgress::setProgressText(const QString& _title, const QString& _description)
{
    if (!_title.isEmpty()) {
		m_title->setText(_title);
	}
	if (!_description.isEmpty()) {
		m_description->setText(_description);
	}
}

void QLightBoxProgress::setProgressValue(int _value)
{
	if (s_lastUsedWidget != 0) {
        if (s_lastUsedWidget->m_progress->maximum() == 0) {
            s_lastUsedWidget->m_progress->setMaximum(100);
        }
        s_lastUsedWidget->m_progress->setValue(_value);
	}
}

void QLightBoxProgress::finish()
{
	close();
	QApplication::alert(parentWidget());
}

QLightBoxProgress* QLightBoxProgress::s_lastUsedWidget = 0;

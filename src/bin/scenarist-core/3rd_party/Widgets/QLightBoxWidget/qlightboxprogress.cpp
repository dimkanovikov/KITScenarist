#include "qlightboxprogress.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMovie>
#include <QProgressBar>

namespace {
	/**
	 * @brief Ключ для доступа к свойству, в котором храним исходный текст заголовка
	 */
	const char* TITLE_SOURCE_TEXT = "sourceText";
}


QLightBoxProgress::QLightBoxProgress(QWidget* _parent, bool _folowToHeadWidget) :
	QLightBoxWidget(_parent, _folowToHeadWidget)
{
	m_title = new QLabel("Title");
	m_title->setProperty("lightBoxProgressTitle", true);

	QLabel* progress = new QLabel;
	progress->setProperty("lightBoxProgressIndicator", true);
	QMovie* progressMovie = new QMovie(":/Interface/UI/loader.gif");
	progress->setMovie(progressMovie);
	progressMovie->start();

	m_description = new QLabel("Description.");
	m_description->setProperty("lightBoxProgressDescription", true);
#ifndef MOBILE_OS
	m_description->setFixedWidth(500);
#endif
	m_description->setWordWrap(true);

	QGridLayout* layout = new QGridLayout;
	layout->setRowStretch(0, 1);
	layout->setColumnStretch(0, 5);
	layout->addWidget(progress, 1, 1, 2, 1, Qt::AlignCenter);
	layout->addWidget(m_title, 1, 2, Qt::AlignLeft);
	layout->setColumnStretch(3, 4);
	layout->addWidget(m_description, 2, 2, Qt::AlignLeft | Qt::AlignTop);
	layout->setRowMinimumHeight(2, 50);

	layout->setRowStretch(3, 1);
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

	m_title->setProperty(TITLE_SOURCE_TEXT, _title);
	m_title->setText(_title);
	m_description->setText(_description);

	show();
}

void QLightBoxProgress::setProgressText(const QString& _title, const QString& _description)
{
	if (!_title.isEmpty()) {
		m_title->setProperty(TITLE_SOURCE_TEXT, _title);
		m_title->setText(_title);
	}
	if (!_description.isEmpty()) {
		m_description->setText(_description);
	}
}

void QLightBoxProgress::setProgressValue(int _value)
{
	if (s_lastUsedWidget != 0) {
		QString title = s_lastUsedWidget->m_title->property(TITLE_SOURCE_TEXT).toString();
		if (_value >= 0 && _value <= 100) {
			title += " " + QString::number(_value) + "%";
		}
		s_lastUsedWidget->m_title->setText(title);
	}
}

void QLightBoxProgress::finish()
{
	close();
	QApplication::alert(parentWidget());
}

QLightBoxProgress* QLightBoxProgress::s_lastUsedWidget = 0;

#include "ProgressWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QMovie>


ProgressWidget::ProgressWidget(QWidget* _parent) :
	QLightBoxWidget(_parent, true)
{
	m_title = new QLabel("Title");
	m_title->setStyleSheet("font-size: 28px; font-weight: bold; color: white");

	QLabel* progress = new QLabel;
	QMovie* progressMovie = new QMovie(":/Interface/UI/loader.gif");
	progress->setMovie(progressMovie);
	progressMovie->start();

	m_description = new QLabel("Description.");
	m_description->setStyleSheet("color: white");

	QGridLayout* layout = new QGridLayout;
	layout->setRowStretch(0, 1);
	layout->setColumnStretch(0, 1);
	layout->addWidget(m_title, 1, 1);
	layout->addWidget(progress, 1, 2, Qt::AlignRight);
	layout->setColumnStretch(3, 1);
	layout->addWidget(m_description, 2, 1, 1, 2);
	layout->setRowStretch(3, 1);
	setLayout(layout);
}

void ProgressWidget::showProgress(const QString& _title, const QString& _description)
{
	m_title->setText(_title);
	m_description->setText(_description);

	show();
}

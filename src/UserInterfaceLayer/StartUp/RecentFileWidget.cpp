#include "RecentFileWidget.h"

#include <3rd_party/Widgets/ElidedLabel/ElidedLabel.h>

#include <QLabel>
#include <QVBoxLayout>

using UserInterface::RecentFileWidget;


RecentFileWidget::RecentFileWidget(QWidget *parent) :
	QWidget(parent)
{
	m_projectName = new QLabel(this);
	QFont projectNameFont = m_projectName->font();
	projectNameFont.setBold(true);
	m_projectName->setFont(projectNameFont);

	m_filePath = new ElidedLabel(this);
	m_filePath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_filePath->setElideMode(Qt::ElideLeft);
	m_filePath->setStyleSheet("color: grey;");

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addSpacing(4);
	layout->addWidget(m_projectName);
	layout->addWidget(m_filePath);
	layout->addSpacing(4);
	layout->setContentsMargins(QMargins());

	this->setLayout(layout);
}

void RecentFileWidget::setProjectName(const QString& _projectName)
{
	m_projectName->setText(_projectName);
}

void RecentFileWidget::setFilePath(const QString& _filePath)
{
	m_filePath->setText(_filePath);
}

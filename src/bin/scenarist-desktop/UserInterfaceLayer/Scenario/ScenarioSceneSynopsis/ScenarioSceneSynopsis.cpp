#include "ScenarioSceneSynopsis.h"

#include <3rd_party/Widgets/ElidedLabel/ElidedLabel.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.h>

#include <QVBoxLayout>

using namespace UserInterface;


ScenarioSceneSynopsis::ScenarioSceneSynopsis(QWidget* _parent) :
	QWidget(_parent),
	m_title(new ElidedLabel(this)),
	m_synopsis(new SimpleTextEditor(this))
{
	initView();
	initConnections();
	initStyleSheet();
}

void ScenarioSceneSynopsis::setHeader(const QString& _header)
{
	QString newHeader = _header.toUpper();
	m_title->setText(QString(" %1: %2").arg(tr("Notes of"), newHeader));
	m_title->setToolTip(newHeader);
}



void ScenarioSceneSynopsis::setSynopsis(const QString& _synopsis)
{
	if (m_synopsis->toHtml() != _synopsis) {
		disconnect(m_synopsis, SIGNAL(textChanged()), this, SLOT(aboutSynopsisChanged()));

		//
		// Сформируем значение синопсиса, для корректности последующих сравнений
		//
		QTextDocument synopsisDoc;
		synopsisDoc.setHtml(_synopsis);
		m_sourceSynopsis = synopsisDoc.toHtml();

		m_synopsis->setHtml(_synopsis);

		connect(m_synopsis, SIGNAL(textChanged()), this, SLOT(aboutSynopsisChanged()));
	}
}

void ScenarioSceneSynopsis::setCommentOnly(bool _isCommentOnly)
{
	m_synopsis->setReadOnly(_isCommentOnly);
}

void ScenarioSceneSynopsis::aboutSynopsisChanged()
{
	if (m_sourceSynopsis != m_synopsis->toHtml()) {
		emit synopsisChanged(m_synopsis->toHtml());
	}
}

void ScenarioSceneSynopsis::initView()
{
	m_title->setText(" ");

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_title);
	layout->addWidget(m_synopsis);

	setLayout(layout);
}

void ScenarioSceneSynopsis::initConnections()
{
	connect(m_synopsis, SIGNAL(textChanged()), this, SLOT(aboutSynopsisChanged()));
}

void ScenarioSceneSynopsis::initStyleSheet()
{
	m_title->setProperty("inTopPanel", true);
	m_title->setProperty("topPanelTopBordered", true);
	m_title->setProperty("topPanelRightBordered", true);

	m_synopsis->setProperty("mainContainer", true);
}

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
	m_title->setText(QString(" %1: %2").arg(tr("Synopsis of"), _header.toUpper()));
}

namespace {
	QString toHtmlEscaped(const QString& _text) {
		QString escapedText = _text;
		escapedText =
				escapedText
				.replace("<", "&lt;")
				.replace(">", "&gt;")
				.replace("\"", "&quot;");
		return escapedText;
	}

	QString fromHtmlEscaped(const QString& _escapedText) {
		QString text = _escapedText;
		text =  text
				.replace("&lt;", "<")
				.replace("&gt;", ">")
				.replace("&quot;", "\"");
		return text;
	}
}

void ScenarioSceneSynopsis::setSynopsis(const QString& _synopsis)
{
	if (::toHtmlEscaped(m_synopsis->toHtml()) != _synopsis) {
		//
		// Текст нужно преобразовать из html-последовательности
		//
		QString htmlSynopsis = ::fromHtmlEscaped(_synopsis);
		m_synopsis->setHtml(htmlSynopsis);
	}
}

void ScenarioSceneSynopsis::aboutSynopsisChanged()
{
	emit synopsisChanged(::toHtmlEscaped(m_synopsis->toHtml()));
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

#include "ScenarioSceneDescription.h"

#include <3rd_party/Widgets/ElidedLabel/ElidedLabel.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.h>

#include <3rd_party/Helpers/TextEditHelper.h>

#include <QVBoxLayout>

using namespace UserInterface;


ScenarioSceneDescription::ScenarioSceneDescription(QWidget* _parent) :
	QWidget(_parent),
	m_title(new ElidedLabel(this)),
	m_description(new SimpleTextEditor(this))
{
	initView();
	initConnections();
	initStyleSheet();
}

void ScenarioSceneDescription::setHeader(const QString& _header)
{
	QString newHeader = _header.toUpper();
	m_title->setText(QString(" %1: %2").arg(tr("Notes of"), newHeader));
	m_title->setToolTip(newHeader);
}

void ScenarioSceneDescription::setDescription(const QString& _description)
{
	if (currentDescription() != _description) {
		disconnect(m_description, SIGNAL(textChanged()), this, SLOT(aboutDescriptionChanged()));

		//
		// Сформируем значение описания, для корректности последующих сравнений
		//
		m_sourceDescription = _description;

		m_description->setHtml(_description);

		connect(m_description, SIGNAL(textChanged()), this, SLOT(aboutDescriptionChanged()));
	}
}

void ScenarioSceneDescription::setCommentOnly(bool _isCommentOnly)
{
	m_description->setReadOnly(_isCommentOnly);
}

void ScenarioSceneDescription::aboutDescriptionChanged()
{
	if (m_sourceDescription != currentDescription()
		|| m_description->toPlainText().isEmpty()) {
		emit descriptionChanged(m_description->toHtml());
	}
}

QString ScenarioSceneDescription::currentDescription() const
{
	return TextEditHelper::removeDocumentTags(m_description->toHtml());
}

void ScenarioSceneDescription::initView()
{
	m_title->setText(" ");

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_title);
	layout->addWidget(m_description);

	setLayout(layout);
}

void ScenarioSceneDescription::initConnections()
{
	connect(m_description, SIGNAL(textChanged()), this, SLOT(aboutDescriptionChanged()));
}

void ScenarioSceneDescription::initStyleSheet()
{
	m_title->setProperty("inTopPanel", true);
	m_title->setProperty("topPanelTopBordered", true);
	m_title->setProperty("topPanelRightBordered", true);

	m_description->setProperty("mainContainer", true);
}

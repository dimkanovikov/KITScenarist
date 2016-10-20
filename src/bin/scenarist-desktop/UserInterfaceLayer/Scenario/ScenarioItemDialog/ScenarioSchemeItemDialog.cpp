#include "ScenarioSchemeItemDialog.h"
#include "ui_ScenarioSchemeItemDialog.h"

using UserInterface::ScenarioSchemeItemDialog;


ScenarioSchemeItemDialog::ScenarioSchemeItemDialog(QWidget* _parent) :
	QLightBoxDialog(_parent),
	m_ui(new Ui::ScenarioSchemeItemDialog)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

ScenarioSchemeItemDialog::~ScenarioSchemeItemDialog()
{
	delete m_ui;
}

void ScenarioSchemeItemDialog::clear()
{
	m_ui->content->setCurrentWidget(m_ui->cardPage);

	m_ui->cardTitle->clear();
	m_ui->cardDescription->clear();

	m_ui->noteText->clear();

	m_ui->edgeText->clear();
}

BusinessLogic::ScenarioModelItem::Type ScenarioSchemeItemDialog::cardType() const
{
	if (m_ui->cardIsScene->isChecked()) {
		return BusinessLogic::ScenarioModelItem::Scene;
	} else if (m_ui->cardIsScenesGroup->isChecked()) {
		return BusinessLogic::ScenarioModelItem::SceneGroup;
	} else {
		return BusinessLogic::ScenarioModelItem::Folder;
	}
}

void ScenarioSchemeItemDialog::setCardType(BusinessLogic::ScenarioModelItem::Type _type)
{
	switch (_type) {
		default:
		case BusinessLogic::ScenarioModelItem::Scene: {
			m_ui->cardIsScene->setChecked(true);
			break;
		}

		case BusinessLogic::ScenarioModelItem::SceneGroup: {
			m_ui->cardIsScenesGroup->setChecked(true);
			break;
		}

		case BusinessLogic::ScenarioModelItem::Folder: {
			m_ui->cardIsFolder->setChecked(true);
			break;
		}
	}
}

QString ScenarioSchemeItemDialog::cardTitle() const
{
	return m_ui->cardTitle->text();
}

void ScenarioSchemeItemDialog::setCardTitle(const QString& _title)
{
	m_ui->cardTitle->setText(_title);
}

QString ScenarioSchemeItemDialog::cardDescription() const
{
	return m_ui->cardDescription->toPlainText();
}

void ScenarioSchemeItemDialog::setCardDescription(const QString& _description)
{
	m_ui->cardDescription->setHtml(_description);
}

void ScenarioSchemeItemDialog::initView()
{
	m_ui->content->setCurrentWidget(m_ui->cardPage);

	m_ui->cardDescription->setToolbarVisible(false);
	m_ui->noteText->setToolbarVisible(false);
	m_ui->edgeText->setToolbarVisible(false);

	m_ui->buttons->addButton(tr("Add"), QDialogButtonBox::AcceptRole);

	QLightBoxDialog::initView();
}

void ScenarioSchemeItemDialog::initConnections()
{
	connect(m_ui->buttons, &QDialogButtonBox::accepted, this, &ScenarioSchemeItemDialog::accept);
	connect(m_ui->buttons, &QDialogButtonBox::rejected, this, &ScenarioSchemeItemDialog::reject);

	QLightBoxDialog::initConnections();
}

void ScenarioSchemeItemDialog::initStyleSheet()
{

}

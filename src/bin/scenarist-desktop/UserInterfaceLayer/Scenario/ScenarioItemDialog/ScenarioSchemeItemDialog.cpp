#include "ScenarioSchemeItemDialog.h"
#include "ui_ScenarioSchemeItemDialog.h"

#include <QPushButton>

using UserInterface::ScenarioSchemeItemDialog;


ScenarioSchemeItemDialog::ScenarioSchemeItemDialog(QWidget* _parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::ScenarioSchemeItemDialog)
{
    m_ui->setupUi(this);
    m_saveButton = m_ui->buttons->addButton(tr("Add"), QDialogButtonBox::AcceptRole);

    initStyleSheet();
}

ScenarioSchemeItemDialog::~ScenarioSchemeItemDialog()
{
    delete m_ui;
}

void ScenarioSchemeItemDialog::clear()
{
    m_ui->cardIsScene->setEnabled(true);
    m_ui->cardIsFolder->setEnabled(true);
    m_ui->cardTitle->clear();
    m_ui->cardDescription->clear();
    m_saveButton->setText(tr("Add"));
}

BusinessLogic::ScenarioModelItem::Type ScenarioSchemeItemDialog::cardType() const
{
    if (m_ui->cardIsScene->isChecked()) {
        return BusinessLogic::ScenarioModelItem::Scene;
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

        case BusinessLogic::ScenarioModelItem::Folder: {
            m_ui->cardIsFolder->setChecked(true);
            break;
        }
    }

    m_ui->cardIsScene->setEnabled(false);
    m_ui->cardIsFolder->setEnabled(false);
    m_saveButton->setText(tr("Save"));
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
    return m_ui->cardDescription->toHtml();
}

void ScenarioSchemeItemDialog::setCardDescription(const QString& _description)
{
    m_ui->cardDescription->setPlainText(_description);
}

QString ScenarioSchemeItemDialog::cardColor() const
{
    if (m_ui->cardColor->currentColor().isValid()) {
        return m_ui->cardColor->currentColor().name();
    }

    return QString::null;
}

void ScenarioSchemeItemDialog::setCardColor(const QString& _color)
{
    m_ui->cardColor->setColor(QColor(_color));
}

QWidget* ScenarioSchemeItemDialog::focusedOnExec() const
{
    return m_ui->cardTitle;
}

void ScenarioSchemeItemDialog::initView()
{
    m_ui->cardColor->setColorsPane(ColoredToolButton::Google);

    m_ui->cardDescription->setToolbarVisible(false);
}

void ScenarioSchemeItemDialog::initConnections()
{
    connect(m_ui->buttons, &QDialogButtonBox::accepted, this, &ScenarioSchemeItemDialog::accept);
    connect(m_ui->buttons, &QDialogButtonBox::rejected, this, &ScenarioSchemeItemDialog::reject);
}

void ScenarioSchemeItemDialog::initStyleSheet()
{
    m_ui->cardColor->setProperty("itemDialog", true);
}

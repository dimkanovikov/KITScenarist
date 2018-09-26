#include "ScenarioItemDialog.h"
#include "ui_ScenarioItemDialog.h"

#include <QPushButton>

namespace {
    const int kSceneIndex = 0;
    const int kFolderIndex = 1;
}

using UserInterface::ScenarioItemDialog;


ScenarioItemDialog::ScenarioItemDialog(QWidget* _parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::ScenarioItemDialog)
{
    m_ui->setupUi(this);
    m_saveButton = m_ui->buttons->addButton(tr("Add"), QDialogButtonBox::AcceptRole);

    initStyleSheet();
}

ScenarioItemDialog::~ScenarioItemDialog()
{
    delete m_ui;
}

void ScenarioItemDialog::prepareForAdding(bool useFolders)
{
    clear();
    m_ui->tabs->setTabText(kSceneIndex, tr("Add Scene"));
    m_ui->tabs->setTabText(kFolderIndex, useFolders ? tr("Add Folder") : tr("Add Act"));
    m_saveButton->setText(tr("Add"));
}

void ScenarioItemDialog::prepareForEditing()
{
    clear();
    m_ui->tabs->setTabText(kSceneIndex, tr("Edit Scene"));
    m_ui->tabs->setTabText(kFolderIndex, tr("Edit Act"));
    m_saveButton->setText(tr("Save"));
}

void ScenarioItemDialog::clear()
{
    initTabs();
    m_ui->tabs->setTabEnabled(kSceneIndex, true);
    m_ui->tabs->setTabEnabled(kFolderIndex, true);
    m_ui->name->clear();
    m_ui->header->clear();
    m_ui->description->clear();
    m_saveButton->setEnabled(false);
}

BusinessLogic::ScenarioModelItem::Type ScenarioItemDialog::itemType() const
{
    if (m_ui->tabs->currentIndex() == kSceneIndex) {
        return BusinessLogic::ScenarioModelItem::Scene;
    } else {
        return BusinessLogic::ScenarioModelItem::Folder;
    }
}

void ScenarioItemDialog::setItemType(BusinessLogic::ScenarioModelItem::Type _type)
{
    switch (_type) {
        default:
        case BusinessLogic::ScenarioModelItem::Scene: {
            m_ui->tabs->setCurrentIndex(kSceneIndex);
            m_ui->tabs->setTabText(kFolderIndex, QString{});
            m_ui->tabs->setTabEnabled(kFolderIndex, false);
            break;
        }

        case BusinessLogic::ScenarioModelItem::Folder: {
            m_ui->tabs->setCurrentIndex(kFolderIndex);
            m_ui->tabs->setTabText(kSceneIndex, QString{});
            m_ui->tabs->setTabEnabled(kSceneIndex, false);
            break;
        }
    }
}

QString ScenarioItemDialog::itemName() const
{
    return m_ui->name->text();
}

void ScenarioItemDialog::setItemName(const QString& _name)
{
    m_ui->name->setText(_name);
}

QString ScenarioItemDialog::itemHeader() const
{
    return m_ui->header->text();
}

void ScenarioItemDialog::setItemHeader(const QString& _header)
{
    m_ui->header->setText(_header);
}

QString ScenarioItemDialog::itemDescription() const
{
    return m_ui->description->toPlainText();
}

void ScenarioItemDialog::setItemDescription(const QString& _description)
{
    m_ui->description->setPlainText(_description);
}

QString ScenarioItemDialog::itemColor() const
{
    if (m_ui->color->currentColor().isValid()) {
        return m_ui->color->currentColor().name();
    }

    return QString::null;
}

void ScenarioItemDialog::setItemColor(const QString& _color)
{
    m_ui->color->setColor(QColor(_color));
}

QWidget* ScenarioItemDialog::focusedOnExec() const
{
    return m_ui->name;
}

QWidget* ScenarioItemDialog::titleWidget() const
{
    return m_ui->tabs;
}

void ScenarioItemDialog::initTabs()
{
    if (m_ui->tabs->count() > 0) {
        return;
    }

    m_ui->tabs->addTab(tr("Scene"));
    m_ui->tabs->addTab(tr("Folder"));
}

void ScenarioItemDialog::initView()
{
    initTabs();
    m_ui->color->setColorsPane(ColoredToolButton::Google);
    m_ui->description->setToolbarVisible(false);
}

void ScenarioItemDialog::initConnections()
{
    connect(m_ui->name, &QLineEdit::textChanged, this, &ScenarioItemDialog::checkSavingAvailable);
    connect(m_ui->header, &QLineEdit::textChanged, this, &ScenarioItemDialog::checkSavingAvailable);
    connect(m_ui->description, &SimpleTextEditorWidget::textChanged, this, &ScenarioItemDialog::checkSavingAvailable);
    connect(m_ui->buttons, &QDialogButtonBox::accepted, this, &ScenarioItemDialog::accept);
    connect(m_ui->buttons, &QDialogButtonBox::rejected, this, &ScenarioItemDialog::reject);
}

void ScenarioItemDialog::initStyleSheet()
{
    m_ui->tabs->setProperty("inTopPanel", true);
    m_ui->color->setProperty("itemDialog", true);
}

void ScenarioItemDialog::checkSavingAvailable()
{
    m_saveButton->setEnabled(!m_ui->name->text().isEmpty()
                             || ! m_ui->header->text().isEmpty()
                             || !m_ui->description->toPlainText().isEmpty());
}

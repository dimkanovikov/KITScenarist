#include "ProjectVersionDialog.h"
#include "ui_ProjectVersionDialog.h"

#include <Domain/ScriptVersion.h>

#include <QCalendarWidget>
#include <QPushButton>

using UserInterface::ProjectVersionDialog;


ProjectVersionDialog::ProjectVersionDialog(QWidget *parent) :
    QLightBoxDialog(parent),
    m_ui(new Ui::ProjectVersionDialog)
{
    m_ui->setupUi(this);

    m_ui->color->setColorsPane(ColoredToolButton::Google);

    initStyleSheet();
}

ProjectVersionDialog::~ProjectVersionDialog()
{
    delete m_ui;
}

void ProjectVersionDialog::setPreviousVersions(Domain::ScriptVersionsTable* _versions)
{
    //
    // Если это не первая версия, запрещаем цвета, которые уже были установлены ранее
    //
    for (const auto& versionObject : _versions->toList()) {
        const auto version = dynamic_cast<Domain::ScriptVersion*>(versionObject);
        m_ui->color->disableColor(version->color());
        m_ui->dateTime->setMinimumDateTime(version->datetime());
    }

    //
    // Устанавливаем следующий разрешённый цвет
    //
    m_ui->color->selectFirstEnabledColor();
}

QDateTime ProjectVersionDialog::versionDateTime() const
{
    return m_ui->dateTime->dateTime();
}

QString ProjectVersionDialog::versionName() const
{
    return m_ui->name->text();
}

QColor ProjectVersionDialog::versionColor() const
{
    return m_ui->color->currentColor();
}

QString ProjectVersionDialog::versionDescription() const
{
    return m_ui->description->toPlainText();
}

QWidget* ProjectVersionDialog::focusedOnExec() const
{
    return m_ui->name;
}

void ProjectVersionDialog::initView()
{
    m_ui->dateTime->setDateTime(QDateTime::currentDateTime());
    m_ui->dateTime->calendarWidget()->setFixedSize(m_ui->dateTime->calendarWidget()->sizeHint() * 1.2);

    m_ui->buttons->button(QDialogButtonBox::Save)->setEnabled(false);
    m_ui->description->setToolbarVisible(false);
}

void ProjectVersionDialog::initConnections()
{
    connect(m_ui->name, &QLineEdit::textChanged, this, [this] (const QString& _name) {
        m_ui->buttons->button(QDialogButtonBox::Save)->setEnabled(!_name.isEmpty());
    });
    connect(m_ui->buttons, &QDialogButtonBox::accepted, this, &ProjectVersionDialog::accept);
    connect(m_ui->buttons, &QDialogButtonBox::rejected, this, &ProjectVersionDialog::reject);
}

void ProjectVersionDialog::initStyleSheet()
{
    m_ui->color->setProperty("itemDialog", true);
}

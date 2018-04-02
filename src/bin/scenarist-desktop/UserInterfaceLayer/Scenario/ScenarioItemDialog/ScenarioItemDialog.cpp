#include "ScenarioItemDialog.h"

#include <3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditorWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

using UserInterface::ScenarioItemDialog;
using BusinessLogic::ScenarioBlockStyle;


ScenarioItemDialog::ScenarioItemDialog(QWidget *_parent) :
    QLightBoxDialog(_parent),
    m_folder(new QRadioButton(this)),
    m_scene(new QRadioButton(this)),
    m_header(new QLineEdit(this)),
    m_color(new ColoredToolButton(QIcon(":/Graphics/Iconset/square.svg"), this)),
    m_description(new SimpleTextEditorWidget(this)),
    m_buttons(new QDialogButtonBox(this)),
    m_saveButton(m_buttons->addButton(tr("Add"), QDialogButtonBox::AcceptRole))
{
    initStyleSheet();
}

void ScenarioItemDialog::clear()
{
    m_header->clear();
    m_color->setColor(QColor());
    m_description->clear();
    m_saveButton->setEnabled(false);
}

ScenarioBlockStyle::Type ScenarioItemDialog::itemType() const
{
    //
    // Определим выбранный стиль
    //
    ScenarioBlockStyle::Type currentType = ScenarioBlockStyle::SceneHeading;
    if (m_folder->isChecked()) {
        currentType = ScenarioBlockStyle::FolderHeader;
    }

    return currentType;
}

QString ScenarioItemDialog::header() const
{
    return m_header->text();
}

QColor ScenarioItemDialog::color() const
{
    return m_color->currentColor();
}

QString ScenarioItemDialog::description() const
{
    return m_description->toPlainText();
}

QWidget*ScenarioItemDialog::focusedOnExec() const
{
    return m_header;
}

void ScenarioItemDialog::initView()
{
    setWindowTitle(tr("Add scenario item"));

    setMinimumSize(500, 400);

    m_scene->setText(tr("Scene"));
    m_scene->setChecked(true);
    m_scene->setFocusPolicy(Qt::NoFocus);
    m_folder->setText(tr("Folder"));
    m_folder->setFocusPolicy(Qt::NoFocus);

    m_color->setColorsPane(ColoredToolButton::Google);

    m_description->setToolbarVisible(false);

    m_buttons->addButton(QDialogButtonBox::Cancel);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addWidget(m_scene);
    topLayout->addWidget(m_folder);
    topLayout->addStretch();
    topLayout->addWidget(m_color);

    QHBoxLayout* headerLayout = new QHBoxLayout;
    headerLayout->addWidget(m_header);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(topLayout);
    layout->addLayout(headerLayout);
    layout->addWidget(new QLabel(tr("Scene Description"), this));
    layout->addWidget(m_description);
    layout->addWidget(m_buttons);

    setLayout(layout);
}

void ScenarioItemDialog::initConnections()
{
    connect(m_header, &QLineEdit::textChanged, [=] (const QString& _text) {
        m_saveButton->setEnabled(!_text.isEmpty());
    });
    connect(m_saveButton, &QPushButton::clicked, this, &ScenarioItemDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &ScenarioItemDialog::reject);
}

void ScenarioItemDialog::initStyleSheet()
{
    m_color->setProperty("itemDialog", true);
}

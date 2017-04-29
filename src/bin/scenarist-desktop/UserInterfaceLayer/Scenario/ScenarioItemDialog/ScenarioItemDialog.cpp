#include "ScenarioItemDialog.h"

#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioLineEdit.h>

#include <3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditorWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

using UserInterface::ScenarioItemDialog;
using UserInterface::ScenarioLineEdit;
using BusinessLogic::ScenarioBlockStyle;


ScenarioItemDialog::ScenarioItemDialog(QWidget *_parent) :
    QLightBoxDialog(_parent),
    m_folder(new QRadioButton(this)),
    m_scene(new QRadioButton(this)),
    m_header(new ScenarioLineEdit(this)),
    m_color(new ColoredToolButton(QIcon(":/Graphics/Icons/Editing/rect.png"), this)),
    m_description(new SimpleTextEditorWidget(this)),
    m_buttons(new QDialogButtonBox(this))
{
    initStyleSheet();
}

void ScenarioItemDialog::clear()
{
    QTextCursor cursor(m_header->document());
    cursor.select(QTextCursor::Document);
    cursor.removeSelectedText();

    m_color->setColor(QColor());
    m_description->clear();

    aboutUpdateCurrentTextStyle();
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
    return m_header->toPlainText();
}

QColor ScenarioItemDialog::color() const
{
    return m_color->currentColor();
}

QString ScenarioItemDialog::description() const
{
    return m_description->toPlainText();
}

void ScenarioItemDialog::aboutUpdateCurrentTextStyle()
{
    //
    // Определим выбранный стиль
    //
    // Для папок и групп сцен используется стиль примечания чтобы не смешивать стиль отображения
    // и избежать возни с концами групп
    //
    ScenarioBlockStyle::Type currentType = ScenarioBlockStyle::Undefined;
    if (m_folder->isChecked()) {
        currentType = ScenarioBlockStyle::Note;
    } else {
        currentType = ScenarioBlockStyle::SceneHeading;
    }

    //
    // Обновим стиль текущего параграфа для редактора
    //
    m_header->changeScenarioBlockType(currentType);
}

void ScenarioItemDialog::initView()
{
    setWindowTitle(tr("Add scenario item"));

    setMinimumSize(500, 400);

    m_scene->setText(tr("Scene"));
    m_scene->setChecked(true);
    m_folder->setText(tr("Folder"));

    m_color->setColorsPane(ColoredToolButton::Google);

    m_description->setToolbarVisible(false);

    m_buttons->addButton(QDialogButtonBox::Ok);
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
    connect(m_folder, SIGNAL(clicked()), this, SLOT(aboutUpdateCurrentTextStyle()));
    connect(m_scene, SIGNAL(clicked()), this, SLOT(aboutUpdateCurrentTextStyle()));

    connect(m_buttons, &QDialogButtonBox::accepted, [=] {
        if (m_header->toPlainText().isEmpty()) {
            m_header->setFocus();
        } else {
            accept();
        }
    });
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void ScenarioItemDialog::initStyleSheet()
{
    m_color->setProperty("itemDialog", true);
}

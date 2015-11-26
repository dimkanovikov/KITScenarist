#include "ScenarioItemDialog.h"

#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioLineEdit.h>

#include <3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

using UserInterface::ScenarioItemDialog;
using UserInterface::ScenarioLineEdit;
using BusinessLogic::ScenarioBlockStyle;


ScenarioItemDialog::ScenarioItemDialog(QWidget *_parent) :
	QLightBoxDialog(_parent),
	m_folder(new QRadioButton(this)),
	m_scenesGroup(new QRadioButton(this)),
	m_scene(new QRadioButton(this)),
	m_header(new ScenarioLineEdit(this)),
	m_color(new ColoredToolButton(QIcon(":/Graphics/Icons/Editing/rect.png"), this)),
	m_description(new SimpleTextEditor(this)),
	m_buttons(new QDialogButtonBox(this))
{
	initView();
	initConnections();
}

void ScenarioItemDialog::clearText()
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
	} else if (m_scenesGroup->isChecked()) {
		currentType = ScenarioBlockStyle::SceneGroupHeader;
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
	return m_description->toHtml();
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
	if (m_folder->isChecked()
		|| m_scenesGroup->isChecked()) {
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

	m_folder->setText(tr("Folder"));
	m_folder->setChecked(true);
	m_scenesGroup->setText(tr("Scenes group"));
	m_scene->setText(tr("Scene"));

	m_color->setColorsPane(ColoredToolButton::Google);

	m_buttons->addButton(QDialogButtonBox::Ok);
	m_buttons->addButton(QDialogButtonBox::Cancel);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->addWidget(m_folder);
	topLayout->addWidget(m_scenesGroup);
	topLayout->addWidget(m_scene);
	topLayout->addStretch();

	QHBoxLayout* headerLayout = new QHBoxLayout;
	headerLayout->addWidget(m_header);
	headerLayout->addWidget(m_color);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addLayout(topLayout);
	layout->addLayout(headerLayout);
	layout->addWidget(m_description);
	layout->addWidget(m_buttons);

	setLayout(layout);

	QLightBoxDialog::initView();
}

void ScenarioItemDialog::initConnections()
{
	connect(m_folder, SIGNAL(clicked()), this, SLOT(aboutUpdateCurrentTextStyle()));
	connect(m_scenesGroup, SIGNAL(clicked()), this, SLOT(aboutUpdateCurrentTextStyle()));
	connect(m_scene, SIGNAL(clicked()), this, SLOT(aboutUpdateCurrentTextStyle()));

	connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QLightBoxDialog::initConnections();
}

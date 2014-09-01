#include "ScenarioItemDialog.h"

#include "../ScenarioTextEdit/ScenarioLineEdit.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

using UserInterface::ScenarioItemDialog;
using UserInterface::ScenarioLineEdit;
using BusinessLogic::ScenarioBlockStyle;


ScenarioItemDialog::ScenarioItemDialog(QWidget *_parent) :
	QDialog(_parent),
	m_folder(new QRadioButton(this)),
	m_scenesGroup(new QRadioButton(this)),
	m_scene(new QRadioButton(this)),
	m_itemEditor(new ScenarioLineEdit(this)),
	m_buttons(new QDialogButtonBox(this))
{
	initView();
	initConnections();
}

void ScenarioItemDialog::clearText()
{
    QTextCursor cursor(m_itemEditor->document());
    cursor.select(QTextCursor::Document);
    cursor.removeSelectedText();

	aboutUpdateCurrentTextStyle();
}

ScenarioBlockStyle::Type ScenarioItemDialog::itemType() const
{
	//
	// Определим выбранный стиль
	//
	ScenarioBlockStyle::Type currentType = ScenarioBlockStyle::TimeAndPlace;
	if (m_folder->isChecked()) {
		currentType = ScenarioBlockStyle::FolderHeader;
	} else if (m_scenesGroup->isChecked()) {
		currentType = ScenarioBlockStyle::SceneGroupHeader;
	}

	return currentType;
}

QString ScenarioItemDialog::itemHeader() const
{
    return m_itemEditor->toPlainText();
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
		currentType = ScenarioBlockStyle::TimeAndPlace;
	}

	//
	// Обновим стиль текущего параграфа для редактора
	//
	m_itemEditor->changeScenarioBlockType(currentType);
}

void ScenarioItemDialog::initView()
{
	setWindowTitle(tr("Add scenario item"));

	m_folder->setText(tr("Folder"));
	m_folder->setChecked(true);
	m_scenesGroup->setText(tr("Scenes group"));
	m_scene->setText(tr("Scene"));

	m_buttons->addButton(QDialogButtonBox::Ok);
	m_buttons->addButton(QDialogButtonBox::Cancel);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->addWidget(m_folder);
	topLayout->addWidget(m_scenesGroup);
	topLayout->addWidget(m_scene);
	topLayout->addStretch();

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addLayout(topLayout);
	layout->addWidget(m_itemEditor);
	layout->addWidget(m_buttons);

	setLayout(layout);
}

void ScenarioItemDialog::initConnections()
{
	connect(m_folder, SIGNAL(clicked()), this, SLOT(aboutUpdateCurrentTextStyle()));
	connect(m_scenesGroup, SIGNAL(clicked()), this, SLOT(aboutUpdateCurrentTextStyle()));
	connect(m_scene, SIGNAL(clicked()), this, SLOT(aboutUpdateCurrentTextStyle()));

	connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

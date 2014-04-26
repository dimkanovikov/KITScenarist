#include "ScenarioItemDialog.h"

#include "../ScenarioTextEdit/ScenarioLineEdit.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QVBoxLayout>

using UserInterface::ScenarioItemDialog;
using UserInterface::ScenarioLineEdit;
using BusinessLogic::ScenarioTextBlockStyle;


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
	m_itemEditor->clear();

	aboutUpdateCurrentTextStyle();
}

ScenarioTextBlockStyle::Type ScenarioItemDialog::itemType() const
{
	//
	// Определим выбранный стиль
	//
	ScenarioTextBlockStyle::Type currentType = ScenarioTextBlockStyle::TimeAndPlace;
	if (m_folder->isChecked()) {
		currentType = ScenarioTextBlockStyle::FolderHeader;
	} else if (m_scenesGroup->isChecked()) {
		currentType = ScenarioTextBlockStyle::SceneGroupHeader;
	}

	return currentType;
}

QString ScenarioItemDialog::itemHeader() const
{
	return m_itemEditor->toPlainText();
}

int ScenarioItemDialog::exec()
{
	clearText();

	//
	// Ищем центр главного виджета приложения
	//
	QWidget* parent = QApplication::activeWindow();

	QPoint position(
				parent->pos().x() + ((parent->width() - width()) / 2),
				parent->pos().y() + ((parent->height() - height()) / 2)
				);
	move(position);

	return QDialog::exec();
}

void ScenarioItemDialog::aboutUpdateCurrentTextStyle()
{
	//
	// Определим выбранный стиль
	//
	// Для папок и групп сцен используется стиль примечания чтобы не смешивать стиль отображения
	// и избежать возни с концами групп
	//
	ScenarioTextBlockStyle::Type currentType = ScenarioTextBlockStyle::Undefined;
	if (m_folder->isChecked()
		|| m_scenesGroup->isChecked()) {
		currentType = ScenarioTextBlockStyle::Note;
	} else {
		currentType = ScenarioTextBlockStyle::TimeAndPlace;
	}

	//
	// Обновим стиль текущего параграфа для редактора
	//
	m_itemEditor->changeScenarioBlockType(currentType);

	m_itemEditor->clearCharFormat();
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

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(m_folder);
	layout->addWidget(m_scenesGroup);
	layout->addWidget(m_scene);
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

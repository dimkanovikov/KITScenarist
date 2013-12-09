#include <QApplication>

#include <QFontDatabase>

#include <QWidget>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h"
#include <QTextCodec>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//
	// Загрузим шрифт Courier New в базу шрифтов программы, если его там ещё нет
	//
	QFontDatabase fontDatabase;
	fontDatabase.addApplicationFont(":/Fonts/Courier New");

	QRadioButton* cbSceneHeading = new QRadioButton("Scene Heading");
	cbSceneHeading->setChecked(true);
	QRadioButton* cbAction = new QRadioButton("Action");
	QRadioButton* cbCharacter = new QRadioButton("Character");
	QRadioButton* cbDialog = new QRadioButton("Dialog");
	QRadioButton* cbParethentcial = new QRadioButton("Parethentcial");
	QRadioButton* cbTitle = new QRadioButton("Title");
	QRadioButton* cbNote = new QRadioButton("Note");
	QRadioButton* cbTransition = new QRadioButton("Transition");
	QRadioButton* cbText = new QRadioButton("Text");
	QPushButton* testBtn = new QPushButton("Test");

	QVBoxLayout* stylesLayout = new QVBoxLayout;
	stylesLayout->addWidget(cbSceneHeading);
	stylesLayout->addWidget(cbAction);
	stylesLayout->addWidget(cbCharacter);
	stylesLayout->addWidget(cbDialog);
	stylesLayout->addWidget(cbParethentcial);
	stylesLayout->addWidget(cbTitle);
	stylesLayout->addWidget(cbNote);
	stylesLayout->addWidget(cbTransition);
	stylesLayout->addWidget(cbText);
	stylesLayout->addWidget(testBtn);
	stylesLayout->addStretch();

	ScenarioTextEdit* screenEdit = new ScenarioTextEdit;
	QObject::connect(testBtn, SIGNAL(clicked()), screenEdit, SLOT(test()));

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addLayout(stylesLayout);
	layout->addWidget(screenEdit);

	QWidget w;
	w.setLayout(layout);
	w.resize(600,400);
	w.show();

	return a.exec();
}

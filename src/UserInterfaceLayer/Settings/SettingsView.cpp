#include "SettingsView.h"
#include "ui_SettingsView.h"

using UserInterface::SettingsView;


SettingsView::SettingsView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SettingsView)
{
	ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

SettingsView::~SettingsView()
{
	delete ui;
}

QSplitter* SettingsView::splitter() const
{
	return ui->splitter;
}

int SettingsView::chronometryCurrentType() const
{
	int type = 0;
	if (ui->pagesChronometry->isChecked()) {
		type = 0;
	} else if (ui->charactersChronometry->isChecked()) {
		type = 1;
	} else {
		type = 2;
	}
	return type;
}

void SettingsView::setTextSpellCheck(bool _value)
{
	ui->spellChecking->setChecked(_value);
}

void SettingsView::setNavigatorShowScenesNumbers(bool _value)
{
	ui->showScenesNumbers->setChecked(_value);
}

void SettingsView::setChronometryCurrentType(int _value)
{
	switch (_value) {
		case 0: {
			ui->pagesChronometry->setChecked(true);;
			break;
		}

		case 1: {
			ui->charactersChronometry->setChecked(true);
			break;
		}

		case 2: {
			ui->configurableChronometry->setChecked(true);
			break;
		}
	}
}

void SettingsView::setChronometryPagesSeconds(int _value)
{
	ui->pagesChronometrySeconds->setValue(_value);
}

void SettingsView::setChronometryCharactersCharacters(int _value)
{
	ui->charactersChronometryCharacters->setValue(_value);
}

void SettingsView::setChronometryCharactersSeconds(int _value)
{
	ui->charactersChronometrySeconds->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsForParagraphTimeAndPlace(double _value)
{
	ui->configurableChronometrySecondsForParagraphTimeAndPlace->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsFor50TimeAndPlace(double _value)
{
	ui->configurableChronometrySecondsPer50CharactersTimeAndPlace->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsForParagraphAction(double _value)
{
	ui->configurableChronometrySecondsForParagraphAction->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsFor50Action(double _value)
{
	ui->configurableChronometrySecondsPer50CharactersAction->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsForParagraphDialog(double _value)
{
	ui->configurableChronometrySecondsForParagraphDialog->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsFor50Dialog(double _value)
{
	ui->configurableChronometrySecondsPer50CharactersDialog->setValue(_value);
}

void SettingsView::initView()
{
	ui->categories->setCurrentRow(0);
	ui->categoriesWidgets->setCurrentIndex(0);

	ui->splitter->setHandleWidth(1);
}

void SettingsView::initConnections()
{
	//
	// Настроим соединения формы
	//
	connect(ui->categories, SIGNAL(currentRowChanged(int)), ui->categoriesWidgets, SLOT(setCurrentIndex(int)));
	connect(ui->pagesChronometry, SIGNAL(toggled(bool)), ui->pagesChronometryGroup, SLOT(setEnabled(bool)));
	connect(ui->charactersChronometry, SIGNAL(toggled(bool)), ui->charactersChronometryGroup, SLOT(setEnabled(bool)));
	connect(ui->configurableChronometry, SIGNAL(toggled(bool)), ui->configurableChronometryGroup, SLOT(setEnabled(bool)));

	//
	// Сигналы об изменении параметров
	//
	// ... текстовый редактор
	connect(ui->spellChecking, SIGNAL(toggled(bool)), this, SIGNAL(textSpellCheckChanged(bool)));
	// ... навигатор
	connect(ui->showScenesNumbers, SIGNAL(toggled(bool)), this, SIGNAL(navigatorShowScenesNumbersChanged(bool)));
	// ... хронометраж
	connect(ui->pagesChronometry, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCurrentTypeChanged()));
	connect(ui->charactersChronometry, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCurrentTypeChanged()));
	connect(ui->configurableChronometry, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCurrentTypeChanged()));
	connect(ui->pagesChronometrySeconds, SIGNAL(valueChanged(int)), this, SIGNAL(chronometryPagesSecondsChanged(int)));
	connect(ui->charactersChronometryCharacters, SIGNAL(valueChanged(int)), this, SIGNAL(chronometryCharactersCharactersChanged(int)));
	connect(ui->charactersChronometrySeconds, SIGNAL(valueChanged(int)), this, SIGNAL(chronometryCharactersSecondsChanged(int)));
	connect(ui->configurableChronometrySecondsForParagraphTimeAndPlace, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double)));
	connect(ui->configurableChronometrySecondsPer50CharactersTimeAndPlace, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double)));
	connect(ui->configurableChronometrySecondsForParagraphAction, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsForParagraphActionChanged(double)));
	connect(ui->configurableChronometrySecondsPer50CharactersAction, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsFor50ActionChanged(double)));
	connect(ui->configurableChronometrySecondsForParagraphDialog, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsForParagraphDialogChanged(double)));
	connect(ui->configurableChronometrySecondsPer50CharactersDialog, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsFor50DialogChanged(double)));
}

void SettingsView::initStyleSheet()
{
	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->categories->setProperty("mainContainer", true);
	ui->categoriesWidgets->setProperty("mainContainer", true);
}

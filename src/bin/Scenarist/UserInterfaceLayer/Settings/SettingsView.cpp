#include "SettingsView.h"
#include "ui_SettingsView.h"

#include <QColorDialog>
#include <QSignalMapper>

#include <3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.h>

using UserInterface::SettingsView;


SettingsView::SettingsView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SettingsView)
{
	ui->setupUi(this);

	initData();
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

void SettingsView::setStylesModel(QAbstractItemModel* _model)
{
	ui->currentScenarioStyle->setModel(_model);

	ui->styles->setModel(_model);
	ui->styles->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
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

void SettingsView::setApplicationUseDarkTheme(bool _value)
{
	ui->useDarkTheme->setChecked(_value);
}

void SettingsView::setApplicationAutosave(bool _value)
{
	ui->autosave->setChecked(_value);
}

void SettingsView::setApplicationAutosaveInterval(int _value)
{
	ui->autosaveInterval->setValue(_value);
}

void SettingsView::setScenarioEditPageView(bool _value)
{
	ui->pageView->setChecked(_value);
}

void SettingsView::setScenarioEditSpellCheck(bool _value)
{
	ui->spellChecking->setChecked(_value);
}

void SettingsView::setScenarioEditSpellCheckLanguage(int _value)
{
	//
	// Выбираем язык проверки
	//
	for (int index = 0; index < ui->spellCheckingLanguage->count(); ++index) {
		if (ui->spellCheckingLanguage->itemData(index).toInt() == _value) {
			ui->spellCheckingLanguage->setCurrentIndex(index);
			break;
		}
	}
}

void SettingsView::setScenarioEditTextColor(const QColor& _color)
{
	setColorFor(ui->textColor, _color);
}

void SettingsView::setScenarioEditBackgroundColor(const QColor& _color)
{
	setColorFor(ui->backgroundColor, _color);
}

void SettingsView::setScenarioEditNonprintableTexColor(const QColor& _color)
{
	setColorFor(ui->nonprintableTextColor, _color);
}

void SettingsView::setScenarioEditFolderTextColor(const QColor& _color)
{
	setColorFor(ui->folderTextColor, _color);
}

void SettingsView::setScenarioEditFolderBackgroundColor(const QColor& _color)
{
	setColorFor(ui->folderBackgroundColor, _color);
}

void SettingsView::setScenarioEditCurrentStyle(const QString& _styleName)
{
	ui->currentScenarioStyle->setCurrentText(_styleName);
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

void SettingsView::aboutScenarioEditSpellCheckLanguageChanged()
{
    emit scenarioEditSpellCheckLanguageChanged(ui->spellCheckingLanguage->currentData().toInt());
}

void SettingsView::aboutScenarioEditChooseTextColor()
{
	setColorFor(ui->textColor);
	emit scenarioEditTextColorChanged(ui->textColor->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseBackgroundColor()
{
	setColorFor(ui->backgroundColor);
	emit scenarioEditBackgroundColorChanged(ui->backgroundColor->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseNonprintableTextColor()
{
	setColorFor(ui->nonprintableTextColor);
	emit scenarioEditNonprintableTextColorChanged(ui->nonprintableTextColor->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseFolderTextColor()
{
	setColorFor(ui->folderTextColor);
	emit scenarioEditFolderTextColorChanged(ui->folderTextColor->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseFolderBackgroundColor()
{
	setColorFor(ui->folderBackgroundColor);
	emit scenarioEditFolderBackgroundColorChanged(ui->folderBackgroundColor->palette().button().color());
}

void SettingsView::aboutEditStylePressed()
{
	if (!ui->styles->selectionModel()->selectedIndexes().isEmpty()) {
		QModelIndex selected = ui->styles->selectionModel()->selectedIndexes().first();
		if (selected.isValid()) {
			emit styleLibraryEditPressed(selected);
		}
	}
}

void SettingsView::aboutRemoveStylePressed()
{
	if (!ui->styles->selectionModel()->selectedIndexes().isEmpty()) {
		QModelIndex selected = ui->styles->selectionModel()->selectedIndexes().first();
		if (selected.isValid()) {
			emit styleLibraryRemovePressed(selected);
		}
	}
}

void SettingsView::aboutSaveStylePressed()
{
	if (!ui->styles->selectionModel()->selectedIndexes().isEmpty()) {
		QModelIndex selected = ui->styles->selectionModel()->selectedIndexes().first();
		if (selected.isValid()) {
			emit styleLibrarySavePressed(selected);
		}
	}
}

void SettingsView::setColorFor(QWidget* _colorPicker)
{
	QColor oldColor = _colorPicker->palette().button().color();
	QColor newColor = QColorDialog::getColor(oldColor, this);
	if (newColor.isValid()) {
		setColorFor(_colorPicker, newColor);
	}
}

void SettingsView::setColorFor(QWidget* _colorPicker, const QColor& _newColor)
{
	_colorPicker->setStyleSheet("background-color: " + _newColor.name());
}

void SettingsView::initData()
{
	ui->spellCheckingLanguage->addItem(tr("Russian"), SpellChecker::Russian);
	ui->spellCheckingLanguage->addItem(tr("Russian with Yo"), SpellChecker::RussianWithYo);
	ui->spellCheckingLanguage->addItem(tr("Ukrainian"), SpellChecker::Ukrainian);
	ui->spellCheckingLanguage->addItem(tr("Belorussian"), SpellChecker::Belorussian);
}

void SettingsView::initView()
{
	ui->categories->setCurrentRow(0);
	ui->categoriesWidgets->setCurrentIndex(0);

	ui->splitter->setHandleWidth(1);
	ui->splitter->setStretchFactor(1, 1);

	ui->spellCheckingLanguage->setCurrentIndex(0);
}

void SettingsView::initConnections()
{
	//
	// Настроим соединения формы
	//
	connect(ui->categories, SIGNAL(currentRowChanged(int)), ui->categoriesWidgets, SLOT(setCurrentIndex(int)));
    // ... активация автосохранения
    connect(ui->autosave, SIGNAL(toggled(bool)), ui->autosaveInterval, SLOT(setEnabled(bool)));
	// ... активация проверки орфографии
	connect(ui->spellChecking, SIGNAL(toggled(bool)), ui->spellCheckingLanguage, SLOT(setEnabled(bool)));
	// ... выбор цвета элементов редактора сценария
	connect(ui->textColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseTextColor()));
	connect(ui->backgroundColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseBackgroundColor()));
	connect(ui->nonprintableTextColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseNonprintableTextColor()));
	connect(ui->folderTextColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderTextColor()));
	connect(ui->folderBackgroundColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderBackgroundColor()));
	// ... смена текущей системы хронометража
	connect(ui->pagesChronometry, SIGNAL(toggled(bool)), ui->pagesChronometryGroup, SLOT(setEnabled(bool)));
	connect(ui->charactersChronometry, SIGNAL(toggled(bool)), ui->charactersChronometryGroup, SLOT(setEnabled(bool)));
	connect(ui->configurableChronometry, SIGNAL(toggled(bool)), ui->configurableChronometryGroup, SLOT(setEnabled(bool)));

	//
	// Сигналы об изменении параметров
	//
	// ... приложение
	connect(ui->useDarkTheme, SIGNAL(toggled(bool)), this, SIGNAL(applicationUseDarkThemeChanged(bool)));
	connect(ui->autosave, SIGNAL(toggled(bool)), this, SIGNAL(applicationAutosaveChanged(bool)));
	connect(ui->autosaveInterval, SIGNAL(valueChanged(int)), this, SIGNAL(applicationAutosaveIntervalChanged(int)));
	// ... текстовый редактор
	connect(ui->pageView, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditPageViewChanged(bool)));
	connect(ui->spellChecking, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditSpellCheckChanged(bool)));
	connect(ui->spellCheckingLanguage, SIGNAL(currentIndexChanged(int)), this, SLOT(aboutScenarioEditSpellCheckLanguageChanged()));
	connect(ui->currentScenarioStyle, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(scenarioEditCurrentStyleChanged(QString)));
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

	//
	// Библиотека стилей
	//
	connect(ui->newStyle, SIGNAL(clicked()), this, SIGNAL(styleLibraryNewPressed()));
	connect(ui->editStyle, SIGNAL(clicked()), this, SLOT(aboutEditStylePressed()));
	connect(ui->styles, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(styleLibraryEditPressed(QModelIndex)));
	connect(ui->removeStyle, SIGNAL(clicked()), this, SLOT(aboutRemoveStylePressed()));
	connect(ui->loadStyle, SIGNAL(clicked()), this, SIGNAL(styleLibraryLoadPressed()));
	connect(ui->saveStyle, SIGNAL(clicked()), this, SLOT(aboutSaveStylePressed()));
}

void SettingsView::initStyleSheet()
{
	//
	// Верхние метки для заполнения пространства
	//
	QList<QWidget*> topEmptyList;
	topEmptyList << ui->topLeftEmptyLabel
				 << ui->topRightEmptyLabel_1
				 << ui->topRightEmptyLabel_2
				 << ui->topRightEmptyLabel_3
				 << ui->topRightEmptyLabel_4
				 << ui->topRightEmptyLabel_5;

	foreach (QWidget* topEmpty, topEmptyList) {
		topEmpty->setProperty("inTopPanel", true);
		topEmpty->setProperty("topPanelTopBordered", true);
		topEmpty->setProperty("topPanelRightBordered", true);
	}

	//
	// Основные контейнеры с содержимым
	//
	QList<QWidget*> mainList;
	mainList << ui->categories
			 << ui->applicationPageWidget
			 << ui->scenarioEditPageWidget
			 << ui->navigatorPageWidget
			 << ui->chronometryPageWidget
			 << ui->stylesLibraryPageWidget;

	foreach (QWidget* main, mainList) {
		main->setProperty("mainContainer", true);
	}

	//
	// Кнопки панели инструментов
	//
	QList<QWidget*> topButtonsList;
	topButtonsList << ui->newStyle
				   << ui->editStyle
				   << ui->removeStyle
				   << ui->loadStyle
				   << ui->saveStyle;
	foreach (QWidget* topButton, topButtonsList) {
		topButton->setProperty("inTopPanel", true);
	}
}

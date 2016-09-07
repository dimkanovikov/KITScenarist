#include "SettingsView.h"
#include "ui_SettingsView.h"

#include "LanguageDialog.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QSignalMapper>

#include <3rd_party/Widgets/HierarchicalHeaderView/HierarchicalHeaderView.h>
#include <3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.h>
#include <3rd_party/Widgets/TabBar/TabBar.h>

#include <3rd_party/Delegates/KeySequenceDelegate/KeySequenceDelegate.h>
#include <3rd_party/Delegates/ComboBoxItemDelegate/ComboBoxItemDelegate.h>

using UserInterface::SettingsView;

namespace {
	/**
	 * @brief Индексы столбцов в табице смены типа
	 */
	/** @{ */
	const int NAMES_COLUMN = 0;
	const int SHORTCUT_COLUMN = 1;
	const int JUMP_TAB_COLUMN = 2;
	const int JUMP_ENTER_COLUMN = 3;
	const int CHANGE_TAB_COLUMN = 4;
	const int CHANGE_ENTER_COLUMN = 5;
	/** @} */
}


SettingsView::SettingsView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SettingsView),
	m_appLanguage(-1),
	m_applicationTabs(new TabBar(this)),
	m_scenarioEditorTabs(new TabBar(this)),
	m_jumpsTableHeader(new HierarchicalHeaderView(Qt::Horizontal, this))
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
	return ui->settingsSplitter;
}

void SettingsView::setBlocksSettingsModel(QAbstractItemModel* _model, QAbstractItemModel* _modelForDelegate)
{
	ui->scenarioEditBlockSettingsTable->setModel(_model);
	connect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(aboutBlockJumpChanged(QModelIndex,QModelIndex)));

	//
	// Настроим делегат
	//

	//
	// ... при необходимости удалим старый делегат
	//
	if (ui->scenarioEditBlockSettingsTable->itemDelegateForColumn(NAMES_COLUMN)
		!= ui->scenarioEditBlockSettingsTable->itemDelegateForColumn(JUMP_TAB_COLUMN)) {
		ui->scenarioEditBlockSettingsTable->itemDelegateForColumn(SHORTCUT_COLUMN)->deleteLater();
		ui->scenarioEditBlockSettingsTable->itemDelegateForColumn(JUMP_TAB_COLUMN)->deleteLater();
	}

	ComboBoxItemDelegate* delegate = new ComboBoxItemDelegate(ui->scenarioEditBlockSettingsTable, _modelForDelegate);
	ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(SHORTCUT_COLUMN,
		new KeySequenceDelegate(ui->scenarioEditBlockSettingsTable));
	ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(JUMP_TAB_COLUMN, delegate);
	ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(JUMP_ENTER_COLUMN, delegate);
	ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(CHANGE_TAB_COLUMN, delegate);
	ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(CHANGE_ENTER_COLUMN, delegate);
}

void SettingsView::setTemplatesModel(QAbstractItemModel* _model)
{
	ui->currentScenarioTemplate->setModel(_model);

	ui->templates->setModel(_model);
	ui->templates->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
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

void SettingsView::setApplicationLanguage(int _value)
{
	m_appLanguage = _value;

	switch (_value) {
		case -1: {
			ui->applicationLanguage->setText(tr("System"));
			break;
		}

		case 0: {
			ui->applicationLanguage->setText(tr("Russian"));
			break;
		}

		case 1: {
			ui->applicationLanguage->setText(tr("Spanish"));
			break;
		}

		case 2: {
			ui->applicationLanguage->setText(tr("English"));
			break;
		}

		case 3: {
			ui->applicationLanguage->setText(tr("French"));
			break;
		}

		case 4: {
			ui->applicationLanguage->setText(tr("Kazakh"));
			break;
		}
	}
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

void SettingsView::setApplicationSaveBackups(bool _save)
{
	ui->saveBackups->setChecked(_save);
}

void SettingsView::setApplicationSaveBackupsFolder(const QString& _folder)
{
	ui->saveBackupsFolder->setText(_folder);
}

void SettingsView::setApplicationTwoPanelMode(bool _use)
{
	ui->applicationTwoPanelMode->setChecked(_use);
}

void SettingsView::setApplicationModuleResearch(bool _use)
{
	ui->applicationModuleResearch->setChecked(_use);
}

void SettingsView::setApplicationModuleScenario(bool _use)
{
	ui->applicationModuleScenario->setChecked(_use);
}

void SettingsView::setApplicationModuleCharacters(bool _use)
{
	ui->applicationModuleCharacters->setChecked(_use);
}

void SettingsView::setApplicationModuleLocations(bool _use)
{
	ui->applicationModuleLocations->setChecked(_use);
}

void SettingsView::setApplicationModuleStatistics(bool _use)
{
	ui->applicationModuleStatistics->setChecked(_use);
}

void SettingsView::setScenarioEditPageView(bool _value)
{
	ui->pageView->setChecked(_value);
}

void SettingsView::setScenarioEditShowScenesNumbers(bool _value)
{
	ui->showScenesNumbersInEditor->setChecked(_value);
}

void SettingsView::setScenarioEditHighlightCurrentLine(bool _value)
{
	ui->highlightCurrentLine->setChecked(_value);
}

void SettingsView::setScenarioEditCapitalizeFirstWord(bool _value)
{
	ui->capitalizeFirstWord->setChecked(_value);
}

void SettingsView::setScenarioEditCorrectDoubleCapitals(bool _value)
{
	ui->correctDoubleCapitals->setChecked(_value);
}

void SettingsView::setScenarioEditReplaceThreeDots(bool _value)
{
	ui->replaceThreeDots->setChecked(_value);
}

void SettingsView::setScenarioEditSmartQuotes(bool _value)
{
	ui->smartQuotes->setChecked(_value);
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

void SettingsView::setScenarioEditTextColorDark(const QColor& _color)
{
	setColorFor(ui->textColorDark, _color);
}

void SettingsView::setScenarioEditBackgroundColorDark(const QColor& _color)
{
	setColorFor(ui->backgroundColorDark, _color);
}

void SettingsView::setScenarioEditNonprintableTexColorDark(const QColor& _color)
{
	setColorFor(ui->nonprintableTextColorDark, _color);
}

void SettingsView::setScenarioEditFolderTextColorDark(const QColor& _color)
{
	setColorFor(ui->folderTextColorDark, _color);
}

void SettingsView::setScenarioEditFolderBackgroundColorDark(const QColor& _color)
{
	setColorFor(ui->folderBackgroundColorDark, _color);
}

void SettingsView::setScenarioEditCurrentTemplate(const QString& _templateName)
{
	ui->currentScenarioTemplate->setCurrentText(_templateName);
}

void SettingsView::setScenarioEditAutoJumpToNextBlock(bool _value)
{
	ui->autoJumpToNextBlock->setChecked(_value);
}

void SettingsView::setScenarioEditShowSuggestionsInEmptyBlocks(bool _value)
{
	ui->showSuggestionsInEmptyBlocks->setChecked(_value);
}

void SettingsView::setScenarioEditReviewUseWordHighlight(bool _value)
{
	ui->reviewUseWordHighlight->setChecked(_value);
}

void SettingsView::setNavigatorShowScenesNumbers(bool _value)
{
	ui->showScenesNumbersInNavigator->setChecked(_value);
}

void SettingsView::setNavigatorShowSceneTitle(bool _value)
{
	ui->showSceneTitle->setChecked(_value);
}

void SettingsView::setNavigatorShowSceneDescription(bool _value)
{
	ui->showSceneDescription->setChecked(_value);
}

void SettingsView::setNavigatorSceneDescriptionIsSceneText(bool _value)
{
	if (_value) {
		ui->sceneDescriptionIsSceneText->setChecked(true);
	} else {
		ui->sceneDescriptionIsSceneSynopsis->setChecked(true);
	}
}

void SettingsView::setNavigatorSceneDescriptionHeight(int _value)
{
	ui->sceneDescriptionHeight->setValue(_value);
}

void SettingsView::setChronometryUsed(bool _value)
{
	ui->chronometryGroup->setChecked(_value);
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

void SettingsView::setChronometryCharactersConsiderSpaces(bool _value)
{
	ui->charactersChronometryConsiderSpaces->setChecked(_value);
}

void SettingsView::setChronometryConfigurableSecondsForParagraphSceneHeading(double _value)
{
	ui->configurableChronometrySecondsForParagraphSceneHeading->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsFor50SceneHeading(double _value)
{
	ui->configurableChronometrySecondsPer50CharactersSceneHeading->setValue(_value);
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

void SettingsView::setPagesCounterUsed(bool _value)
{
	ui->pagesCounter->setChecked(_value);
}

void SettingsView::setWordsCounterUsed(bool _value)
{
	ui->wordsCounter->setChecked(_value);
}

void SettingsView::setSimbolsCounterUsed(bool _value)
{
	ui->simbolsCounter->setChecked(_value);
}

void SettingsView::aboutChooseApplicationLanguage()
{
	UserInterface::LanguageDialog dlg(this, m_appLanguage);
	if (dlg.exec() == QLightBoxDialog::Accepted
		&& dlg.language() != m_appLanguage) {
		emit applicationLanguageChanged(dlg.language());
	}
}

void SettingsView::aboutScenarioEditSpellCheckLanguageChanged()
{
	emit scenarioEditSpellCheckLanguageChanged(ui->spellCheckingLanguage->currentData().toInt());
}

void SettingsView::aboutBrowseSaveBackupsFolder()
{
	const QString folder =
			QFileDialog::getExistingDirectory(this, tr("Choose backups saving folder"),
				ui->saveBackupsFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!folder.isEmpty()) {
		ui->saveBackupsFolder->setText(folder);
	}
}

void SettingsView::aboutBlockJumpChanged(const QModelIndex& _topLeft, const QModelIndex& _bottomRight)
{
	Q_UNUSED(_topLeft);

	if (_bottomRight.isValid()) {
		const QString blockName = _bottomRight.sibling(_bottomRight.row(), NAMES_COLUMN).data().toString();
		const QString shortcut = _bottomRight.sibling(_bottomRight.row(), SHORTCUT_COLUMN).data().toString();
		const QString jumpForTabName = _bottomRight.sibling(_bottomRight.row(), JUMP_TAB_COLUMN).data().toString();
		const QString jumpForEnterName = _bottomRight.sibling(_bottomRight.row(), JUMP_ENTER_COLUMN).data().toString();
		const QString changeForTabName = _bottomRight.sibling(_bottomRight.row(), CHANGE_TAB_COLUMN).data().toString();
		const QString changeForEnterName = _bottomRight.sibling(_bottomRight.row(), CHANGE_ENTER_COLUMN).data().toString();

		emit scenarioEditBlockSettingsChanged(blockName, shortcut, jumpForTabName,
				jumpForEnterName, changeForTabName, changeForEnterName);
	}
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

void SettingsView::aboutScenarioEditChooseTextColorDark()
{
	setColorFor(ui->textColorDark);
	emit scenarioEditTextColorDarkChanged(ui->textColorDark->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseBackgroundColorDark()
{
	setColorFor(ui->backgroundColorDark);
	emit scenarioEditBackgroundColorDarkChanged(ui->backgroundColorDark->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseNonprintableTextColorDark()
{
	setColorFor(ui->nonprintableTextColorDark);
	emit scenarioEditNonprintableTextColorDarkChanged(ui->nonprintableTextColorDark->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseFolderTextColorDark()
{
	setColorFor(ui->folderTextColorDark);
	emit scenarioEditFolderTextColorDarkChanged(ui->folderTextColorDark->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseFolderBackgroundColorDark()
{
	setColorFor(ui->folderBackgroundColorDark);
	emit scenarioEditFolderBackgroundColorDarkChanged(ui->folderBackgroundColorDark->palette().button().color());
}

void SettingsView::aboutEditTemplatePressed()
{
	if (!ui->templates->selectionModel()->selectedIndexes().isEmpty()) {
		QModelIndex selected = ui->templates->selectionModel()->selectedIndexes().first();
		//
		// Если индекс корректен и его можно изменять
		//
		if (selected.isValid()
			&& selected.data(Qt::UserRole).toBool()) {
			emit templateLibraryEditPressed(selected);
		}
	}
}

void SettingsView::aboutRemoveTemplatePressed()
{
	if (!ui->templates->selectionModel()->selectedIndexes().isEmpty()) {
		QModelIndex selected = ui->templates->selectionModel()->selectedIndexes().first();
		if (selected.isValid()) {
			emit templateLibraryRemovePressed(selected);
		}
	}
}

void SettingsView::aboutSaveTemplatePressed()
{
	if (!ui->templates->selectionModel()->selectedIndexes().isEmpty()) {
		QModelIndex selected = ui->templates->selectionModel()->selectedIndexes().first();
		if (selected.isValid()) {
			emit templateLibrarySavePressed(selected);
		}
	}
}

void SettingsView::aboutApplyTemplatePressed()
{
	if (!ui->templates->selectionModel()->selectedIndexes().isEmpty()) {
		QModelIndex selected = ui->templates->selectionModel()->selectedIndexes().first();
		if (selected.isValid()) {
			const QString templateName = selected.data().toString();
			ui->currentScenarioTemplate->setCurrentText(templateName);
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
	ui->spellCheckingLanguage->addItem(tr("Russian with Yo"), SpellChecker::RussianWithYo);
	ui->spellCheckingLanguage->addItem(tr("Russian"), SpellChecker::Russian);
	ui->spellCheckingLanguage->addItem(tr("Armenian (Eastern)"), SpellChecker::ArmenianEastern);
	ui->spellCheckingLanguage->addItem(tr("Armenian (Western)"), SpellChecker::ArmenianWestern);
	ui->spellCheckingLanguage->addItem(tr("Belorussian"), SpellChecker::Belorussian);
	ui->spellCheckingLanguage->addItem(tr("English (GB)"), SpellChecker::EnglishGB);
	ui->spellCheckingLanguage->addItem(tr("English (US)"), SpellChecker::EnglishUS);
	ui->spellCheckingLanguage->addItem(tr("French"), SpellChecker::French);
	ui->spellCheckingLanguage->addItem(tr("Kazakh"), SpellChecker::Kazakh);
	ui->spellCheckingLanguage->addItem(tr("Spanish"), SpellChecker::Spanish);
	ui->spellCheckingLanguage->addItem(tr("Ukrainian"), SpellChecker::Ukrainian);
}

void SettingsView::initView()
{
	ui->categories->setCurrentRow(0);
	ui->categoriesWidgets->setCurrentIndex(0);

	ui->settingsSplitter->setHandleWidth(1);
	ui->settingsSplitter->setOpaqueResize(false);
	ui->settingsSplitter->setStretchFactor(1, 1);

	ui->spellCheckingLanguage->setCurrentIndex(0);
	ui->spellCheckingLanguage->setEnabled(false);
	ui->browseBackupFolder->updateIcons();

	m_applicationTabs->addTab(tr("Common"));
	m_applicationTabs->addTab(tr("Modules"));
	m_applicationTabs->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	ui->applicationPageLayout->addWidget(m_applicationTabs, 0, 0);
	ui->applicationPageLayout->addWidget(ui->topRightEmptyLabel_1, 0, 1);
	ui->applicationPageStack->setCurrentIndex(0);

	m_scenarioEditorTabs->addTab(tr("Common"));
	m_scenarioEditorTabs->addTab(tr("Editing"));
	m_scenarioEditorTabs->addTab(tr("Shortcuts"));
	m_scenarioEditorTabs->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	ui->scenarioEditPageLayout->addWidget(m_scenarioEditorTabs, 0, 0);
	ui->scenarioEditPageLayout->addWidget(ui->topRightEmptyLabel_2, 0, 1);
	ui->scenarioEditPageStack->setCurrentIndex(0);

	m_jumpsTableHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->scenarioEditBlockSettingsTable->setHorizontalHeader(m_jumpsTableHeader);

	ui->newTemplate->updateIcons();
	ui->editTemplate->updateIcons();
	ui->saveTemplate->updateIcons();
	ui->loadTemplate->updateIcons();
	ui->removeTemplate->updateIcons();
	ui->applyTemplate->updateIcons();
	ui->templates->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void SettingsView::initConnections()
{
	//
	// Настроим соединения формы
	//
	connect(ui->categories, SIGNAL(currentRowChanged(int)), ui->categoriesWidgets, SLOT(setCurrentIndex(int)));
	// ... смена вкладок страницы настроек приложения
	connect(m_applicationTabs, &TabBar::currentChanged, ui->applicationPageStack, &QStackedWidget::setCurrentIndex);
	// ... активация автосохранения
	connect(ui->autosave, SIGNAL(toggled(bool)), ui->autosaveInterval, SLOT(setEnabled(bool)));
	// ... активация проверки орфографии
	connect(ui->spellChecking, SIGNAL(toggled(bool)), ui->spellCheckingLanguage, SLOT(setEnabled(bool)));
	// ... выбор папки сохранения резервных копий
	connect(ui->browseBackupFolder, SIGNAL(clicked()), this, SLOT(aboutBrowseSaveBackupsFolder()));
	// ... смена вкладок страницы настройки редактора сценария
	connect(m_scenarioEditorTabs, SIGNAL(currentChanged(int)), ui->scenarioEditPageStack, SLOT(setCurrentIndex(int)));
	// ... выбор цвета элементов редактора сценария
	connect(ui->textColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseTextColor()));
	connect(ui->backgroundColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseBackgroundColor()));
	connect(ui->nonprintableTextColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseNonprintableTextColor()));
	connect(ui->folderTextColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderTextColor()));
	connect(ui->folderBackgroundColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderBackgroundColor()));
	connect(ui->textColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseTextColorDark()));
	connect(ui->backgroundColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseBackgroundColorDark()));
	connect(ui->nonprintableTextColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseNonprintableTextColorDark()));
	connect(ui->folderTextColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderTextColorDark()));
	connect(ui->folderBackgroundColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderBackgroundColorDark()));
	// ... смена текущей системы хронометража
	connect(ui->pagesChronometry, SIGNAL(toggled(bool)), ui->pagesChronometryGroup, SLOT(setEnabled(bool)));
	connect(ui->charactersChronometry, SIGNAL(toggled(bool)), ui->charactersChronometryGroup, SLOT(setEnabled(bool)));
	connect(ui->configurableChronometry, SIGNAL(toggled(bool)), ui->configurableChronometryGroup, SLOT(setEnabled(bool)));

	//
	// Сбросить настройки
	//
	connect(ui->resetSettings, SIGNAL(clicked(bool)), this, SIGNAL(resetSettings()));

	//
	// Сигналы об изменении параметров
	//
	// ... приложение
	connect(ui->changeLanguage, SIGNAL(clicked(bool)), this, SLOT(aboutChooseApplicationLanguage()));
	connect(ui->useDarkTheme, SIGNAL(toggled(bool)), this, SIGNAL(applicationUseDarkThemeChanged(bool)));
	connect(ui->autosave, SIGNAL(toggled(bool)), this, SIGNAL(applicationAutosaveChanged(bool)));
	connect(ui->autosaveInterval, SIGNAL(valueChanged(int)), this, SIGNAL(applicationAutosaveIntervalChanged(int)));
	connect(ui->saveBackups, SIGNAL(toggled(bool)), this, SIGNAL(applicationSaveBackupsChanged(bool)));
	connect(ui->saveBackupsFolder, SIGNAL(textChanged(QString)), this, SIGNAL(applicationSaveBackupsFolderChanged(QString)));
	connect(ui->applicationTwoPanelMode, &QCheckBox::toggled, this, &SettingsView::applicationTwoPanelModeChanged);
	connect(ui->applicationModuleResearch, &QCheckBox::toggled, this, &SettingsView::applicationModuleResearchChanged);
	connect(ui->applicationModuleScenario, &QCheckBox::toggled, this, &SettingsView::applicationModuleScenarioChanged);
	connect(ui->applicationModuleCharacters, &QCheckBox::toggled, this, &SettingsView::applicationModuleCharactersChanged);
	connect(ui->applicationModuleLocations, &QCheckBox::toggled, this, &SettingsView::applicationModuleLocationsChanged);
	connect(ui->applicationModuleStatistics, &QCheckBox::toggled, this, &SettingsView::applicationModuleStatisticsChanged);
	// ... текстовый редактор
	connect(ui->pageView, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditPageViewChanged(bool)));
	connect(ui->showScenesNumbersInEditor, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditShowScenesNumbersChanged(bool)));
	connect(ui->highlightCurrentLine, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditHighlightCurrentLineChanged(bool)));
	connect(ui->capitalizeFirstWord, &QCheckBox::toggled, this, &SettingsView::scenarioEditCapitalizeFirstWordChanged);
	connect(ui->correctDoubleCapitals, &QCheckBox::toggled, this, &SettingsView::scenarioEditCorrectDoubleCapitalsChanged);
	connect(ui->replaceThreeDots, &QCheckBox::toggled, this, &SettingsView::scenarioEditReplaceThreeDotsChanged);
	connect(ui->smartQuotes, &QCheckBox::toggled, this, &SettingsView::scenarioEditSmartQuotesChanged);
	connect(ui->spellChecking, &QCheckBox::toggled, [=] (bool _checked) {
		emit scenarioEditSpellCheckChanged(_checked);
		if (_checked) {
			aboutScenarioEditSpellCheckLanguageChanged();
		}
	});
	connect(ui->spellCheckingLanguage, SIGNAL(currentIndexChanged(int)), this, SLOT(aboutScenarioEditSpellCheckLanguageChanged()));
	connect(ui->currentScenarioTemplate, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(scenarioEditCurrentTemplateChanged(QString)));
	connect(ui->autoJumpToNextBlock, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditAutoJumpToNextBlockChanged(bool)));
	connect(ui->showSuggestionsInEmptyBlocks, &QCheckBox::toggled, this, &SettingsView::scenarioEditShowSuggestionsInEmptyBlocksChanged);
	connect(ui->reviewUseWordHighlight, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditReviewUseWordHighlightChanged(bool)));
	// ... навигатор
	connect(ui->showScenesNumbersInNavigator, SIGNAL(toggled(bool)), this, SIGNAL(navigatorShowScenesNumbersChanged(bool)));
	connect(ui->showSceneTitle, &QCheckBox::toggled, this, &SettingsView::navigatorShowSceneTitleChanged);
	connect(ui->showSceneDescription, SIGNAL(toggled(bool)), this, SIGNAL(navigatorShowSceneDescriptionChanged(bool)));
	connect(ui->sceneDescriptionIsSceneText, SIGNAL(toggled(bool)), this, SIGNAL(navigatorSceneDescriptionIsSceneTextChanged(bool)));
	connect(ui->sceneDescriptionHeight, SIGNAL(valueChanged(int)), this, SIGNAL(navigatorSceneDescriptionHeightChanged(int)));
	// ... хронометраж
	connect(ui->chronometryGroup, SIGNAL(toggled(bool)), this, SIGNAL(chronometryUsedChanged(bool)));
	connect(ui->pagesChronometry, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCurrentTypeChanged()));
	connect(ui->charactersChronometry, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCurrentTypeChanged()));
	connect(ui->configurableChronometry, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCurrentTypeChanged()));
	connect(ui->pagesChronometrySeconds, SIGNAL(valueChanged(int)), this, SIGNAL(chronometryPagesSecondsChanged(int)));
	connect(ui->charactersChronometryCharacters, SIGNAL(valueChanged(int)), this, SIGNAL(chronometryCharactersCharactersChanged(int)));
	connect(ui->charactersChronometrySeconds, SIGNAL(valueChanged(int)), this, SIGNAL(chronometryCharactersSecondsChanged(int)));
	connect(ui->charactersChronometryConsiderSpaces, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCharactersConsiderSpaces(bool)));
	connect(ui->configurableChronometrySecondsForParagraphSceneHeading, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsForParagraphSceneHeadingChanged(double)));
	connect(ui->configurableChronometrySecondsPer50CharactersSceneHeading, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsFor50SceneHeadingChanged(double)));
	connect(ui->configurableChronometrySecondsForParagraphAction, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsForParagraphActionChanged(double)));
	connect(ui->configurableChronometrySecondsPer50CharactersAction, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsFor50ActionChanged(double)));
	connect(ui->configurableChronometrySecondsForParagraphDialog, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsForParagraphDialogChanged(double)));
	connect(ui->configurableChronometrySecondsPer50CharactersDialog, SIGNAL(valueChanged(double)),
			this, SIGNAL(chronometryConfigurableSecondsFor50DialogChanged(double)));
	// ... счётчики
	connect(ui->pagesCounter, SIGNAL(toggled(bool)), this, SIGNAL(pagesCounterUsedChanged(bool)));
	connect(ui->wordsCounter, SIGNAL(toggled(bool)), this, SIGNAL(wordsCounterUsedChanged(bool)));
	connect(ui->simbolsCounter, SIGNAL(toggled(bool)), this, SIGNAL(simbolsCounterUsedChanged(bool)));

	//
	// Библиотека стилей
	//
	connect(ui->newTemplate, SIGNAL(clicked()), this, SIGNAL(templateLibraryNewPressed()));
	connect(ui->editTemplate, SIGNAL(clicked()), this, SLOT(aboutEditTemplatePressed()));
	connect(ui->templates, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(aboutEditTemplatePressed()));
	connect(ui->removeTemplate, SIGNAL(clicked()), this, SLOT(aboutRemoveTemplatePressed()));
	connect(ui->loadTemplate, SIGNAL(clicked()), this, SIGNAL(templateLibraryLoadPressed()));
	connect(ui->saveTemplate, SIGNAL(clicked()), this, SLOT(aboutSaveTemplatePressed()));
	connect(ui->applyTemplate, SIGNAL(clicked()), this, SLOT(aboutApplyTemplatePressed()));
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
				 << ui->topRightEmptyLabel_5
				 << ui->topRightEmptyLabel_7
				 << ui->topRightEmptyLabel_8;

	foreach (QWidget* topEmpty, topEmptyList) {
		topEmpty->setProperty("inTopPanel", true);
		topEmpty->setProperty("topPanelTopBordered", true);
	}

	//
	// Основные контейнеры с содержимым
	//
	QList<QWidget*> mainList;
	mainList << ui->categories
			 << ui->applicationPageStack
			 << ui->scenarioEditPageStack
			 << ui->navigatorPageWidget
			 << ui->chronometryPageWidget
			 << ui->templatesLibraryPageWidget;

	foreach (QWidget* main, mainList) {
		main->setProperty("mainContainer", true);
	}

	//
	// Кнопки панели инструментов
	//
	QList<QWidget*> topButtonsList;
	topButtonsList << ui->newTemplate
				   << ui->editTemplate
				   << ui->removeTemplate
				   << ui->loadTemplate
				   << ui->saveTemplate
				   << ui->applyTemplate;
	foreach (QWidget* topButton, topButtonsList) {
		topButton->setProperty("inTopPanel", true);
	}

	//
	// Вкладки
	//
	m_applicationTabs->setProperty("inTopPanel", true);
	m_scenarioEditorTabs->setProperty("inTopPanel", true);
}

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
    m_ui(new Ui::SettingsView),
    m_appLanguage(-1),
    m_applicationTabs(new TabBar(this)),
    m_scenarioEditorTabs(new TabBar(this)),
    m_jumpsTableHeader(new HierarchicalHeaderView(Qt::Horizontal, this))
{
    m_ui->setupUi(this);

    initData();
    initView();
    initConnections();
    initStyleSheet();
}

SettingsView::~SettingsView()
{
    delete m_ui;
}

QSplitter* SettingsView::splitter() const
{
    return m_ui->settingsSplitter;
}

void SettingsView::setBlocksSettingsModel(QAbstractItemModel* _model, QAbstractItemModel* _modelForDelegate)
{
    m_ui->scenarioEditBlockSettingsTable->setModel(_model);
    connect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(aboutBlockJumpChanged(QModelIndex,QModelIndex)));

    //
    // Настроим делегат
    //

    //
    // ... при необходимости удалим старый делегат
    //
    if (m_ui->scenarioEditBlockSettingsTable->itemDelegateForColumn(NAMES_COLUMN)
        != m_ui->scenarioEditBlockSettingsTable->itemDelegateForColumn(JUMP_TAB_COLUMN)) {
        m_ui->scenarioEditBlockSettingsTable->itemDelegateForColumn(SHORTCUT_COLUMN)->deleteLater();
        m_ui->scenarioEditBlockSettingsTable->itemDelegateForColumn(JUMP_TAB_COLUMN)->deleteLater();
    }

    ComboBoxItemDelegate* delegate = new ComboBoxItemDelegate(m_ui->scenarioEditBlockSettingsTable, _modelForDelegate);
    m_ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(SHORTCUT_COLUMN,
        new KeySequenceDelegate(m_ui->scenarioEditBlockSettingsTable));
    m_ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(JUMP_TAB_COLUMN, delegate);
    m_ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(JUMP_ENTER_COLUMN, delegate);
    m_ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(CHANGE_TAB_COLUMN, delegate);
    m_ui->scenarioEditBlockSettingsTable->setItemDelegateForColumn(CHANGE_ENTER_COLUMN, delegate);
}

void SettingsView::setTemplatesModel(QAbstractItemModel* _model)
{
    m_ui->currentScenarioTemplate->setModel(_model);

    m_ui->templates->setModel(_model);
    m_ui->templates->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

int SettingsView::chronometryCurrentType() const
{
    int type = 0;
    if (m_ui->pagesChronometry->isChecked()) {
        type = 0;
    } else if (m_ui->charactersChronometry->isChecked()) {
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
            m_ui->applicationLanguage->setText(tr("System"));
            break;
        }

        case 0: {
            m_ui->applicationLanguage->setText(tr("Russian"));
            break;
        }

        case 1: {
            m_ui->applicationLanguage->setText(tr("Spanish"));
            break;
        }

        case 2: {
            m_ui->applicationLanguage->setText(tr("English"));
            break;
        }

        case 3: {
            m_ui->applicationLanguage->setText(tr("French"));
            break;
        }

        case 4: {
            m_ui->applicationLanguage->setText(tr("Kazakh"));
            break;
        }

        case 5: {
            m_ui->applicationLanguage->setText(tr("Ukrainian"));
            break;
        }

        case 6: {
            m_ui->applicationLanguage->setText(tr("German"));
            break;
        }

        case 7: {
            m_ui->applicationLanguage->setText(tr("Portuguese"));
            break;
        }
    }
}

void SettingsView::setApplicationUseDarkTheme(bool _value)
{
    m_ui->useDarkTheme->setChecked(_value);
}

void SettingsView::setApplicationAutosave(bool _value)
{
    m_ui->autosave->setChecked(_value);
}

void SettingsView::setApplicationAutosaveInterval(int _value)
{
    m_ui->autosaveInterval->setValue(_value);
}

void SettingsView::setApplicationSaveBackups(bool _save)
{
    m_ui->saveBackups->setChecked(_save);
}

void SettingsView::setApplicationSaveBackupsFolder(const QString& _folder)
{
    m_ui->saveBackupsFolder->setText(_folder);
}

void SettingsView::setApplicationCompactMode(bool _enable)
{
    m_ui->applicationCompactMode->setChecked(_enable);
}

void SettingsView::setApplicationTwoPanelMode(bool _use)
{
    m_ui->applicationTwoPanelMode->setChecked(_use);
}

void SettingsView::setApplicationModuleResearch(bool _use)
{
    m_ui->applicationModuleResearch->setChecked(_use);
}

void SettingsView::setApplicationModuleCards(bool _use)
{
    m_ui->applicationModuleCards->setChecked(_use);
}

void SettingsView::setApplicationModuleScenario(bool _use)
{
    m_ui->applicationModuleScenario->setChecked(_use);
}

void SettingsView::setApplicationModuleStatistics(bool _use)
{
    m_ui->applicationModuleStatistics->setChecked(_use);
}

void SettingsView::setCardsUseCorkboardBackground(bool _use)
{
    if (_use) {
        m_ui->cardsUseCorkboardBackground->setChecked(true);
    } else {
        m_ui->cardsUseColorsBackground->setChecked(true);
    }
}

void SettingsView::setCardsBackgroundColor(const QColor& _color)
{
    setColorFor(m_ui->cardsBackgroundColor, _color);
}

void SettingsView::setCardsBackgroundColorDark(const QColor& _color)
{
    setColorFor(m_ui->cardsBackgroundColorDark, _color);
}

void SettingsView::setScenarioEditPageView(bool _value)
{
    m_ui->pageView->setChecked(_value);
}

void SettingsView::setScenarioEditShowScenesNumbers(bool _value)
{
    m_ui->showScenesNumbersInEditor->setChecked(_value);
}

void SettingsView::setScenarioEditHighlightCurrentLine(bool _value)
{
    m_ui->highlightCurrentLine->setChecked(_value);
}

void SettingsView::setScenarioEditCapitalizeFirstWord(bool _value)
{
    m_ui->capitalizeFirstWord->setChecked(_value);
}

void SettingsView::setScenarioEditCorrectDoubleCapitals(bool _value)
{
    m_ui->correctDoubleCapitals->setChecked(_value);
}

void SettingsView::setScenarioEditReplaceThreeDots(bool _value)
{
    m_ui->replaceThreeDots->setChecked(_value);
}

void SettingsView::setScenarioEditSmartQuotes(bool _value)
{
    m_ui->smartQuotes->setChecked(_value);
}

void SettingsView::setScenarioEditSpellCheck(bool _value)
{
    m_ui->spellChecking->setChecked(_value);
}

void SettingsView::setScenarioEditSpellCheckLanguage(int _value)
{
    //
    // Выбираем язык проверки
    //
    for (int index = 0; index < m_ui->spellCheckingLanguage->count(); ++index) {
        if (m_ui->spellCheckingLanguage->itemData(index).toInt() == _value) {
            m_ui->spellCheckingLanguage->setCurrentIndex(index);
            break;
        }
    }
}

void SettingsView::setScenarioEditTextColor(const QColor& _color)
{
    setColorFor(m_ui->textColor, _color);
}

void SettingsView::setScenarioEditBackgroundColor(const QColor& _color)
{
    setColorFor(m_ui->backgroundColor, _color);
}

void SettingsView::setScenarioEditNonprintableTexColor(const QColor& _color)
{
    setColorFor(m_ui->nonprintableTextColor, _color);
}

void SettingsView::setScenarioEditFolderTextColor(const QColor& _color)
{
    setColorFor(m_ui->folderTextColor, _color);
}

void SettingsView::setScenarioEditFolderBackgroundColor(const QColor& _color)
{
    setColorFor(m_ui->folderBackgroundColor, _color);
}

void SettingsView::setScenarioEditTextColorDark(const QColor& _color)
{
    setColorFor(m_ui->textColorDark, _color);
}

void SettingsView::setScenarioEditBackgroundColorDark(const QColor& _color)
{
    setColorFor(m_ui->backgroundColorDark, _color);
}

void SettingsView::setScenarioEditNonprintableTexColorDark(const QColor& _color)
{
    setColorFor(m_ui->nonprintableTextColorDark, _color);
}

void SettingsView::setScenarioEditFolderTextColorDark(const QColor& _color)
{
    setColorFor(m_ui->folderTextColorDark, _color);
}

void SettingsView::setScenarioEditFolderBackgroundColorDark(const QColor& _color)
{
    setColorFor(m_ui->folderBackgroundColorDark, _color);
}

void SettingsView::setScenarioEditCurrentTemplate(const QString& _templateName)
{
    m_ui->currentScenarioTemplate->setCurrentText(_templateName);
}

void SettingsView::setScenarioEditAutoJumpToNextBlock(bool _value)
{
    m_ui->autoJumpToNextBlock->setChecked(_value);
}

void SettingsView::setScenarioEditShowSuggestionsInEmptyBlocks(bool _value)
{
    m_ui->showSuggestionsInEmptyBlocks->setChecked(_value);
}

void SettingsView::setScenarioEditReviewUseWordHighlight(bool _value)
{
    m_ui->reviewUseWordHighlight->setChecked(_value);
}

void SettingsView::setNavigatorShowScenesNumbers(bool _value)
{
    m_ui->showScenesNumbersInNavigator->setChecked(_value);
}

void SettingsView::setNavigatorShowSceneTitle(bool _value)
{
    m_ui->showSceneTitle->setChecked(_value);
}

void SettingsView::setNavigatorShowSceneDescription(bool _value)
{
    m_ui->showSceneDescription->setChecked(_value);
}

void SettingsView::setNavigatorSceneDescriptionIsSceneText(bool _value)
{
    if (_value) {
        m_ui->sceneDescriptionIsSceneText->setChecked(true);
    } else {
        m_ui->sceneDescriptionIsSceneSynopsis->setChecked(true);
    }
}

void SettingsView::setNavigatorSceneDescriptionHeight(int _value)
{
    m_ui->sceneDescriptionHeight->setValue(_value);
}

void SettingsView::setChronometryUsed(bool _value)
{
    m_ui->chronometryGroup->setChecked(_value);
}

void SettingsView::setChronometryCurrentType(int _value)
{
    switch (_value) {
        case 0: {
            m_ui->pagesChronometry->setChecked(true);;
            break;
        }

        case 1: {
            m_ui->charactersChronometry->setChecked(true);
            break;
        }

        case 2: {
            m_ui->configurableChronometry->setChecked(true);
            break;
        }
    }
}

void SettingsView::setChronometryPagesSeconds(int _value)
{
    m_ui->pagesChronometrySeconds->setValue(_value);
}

void SettingsView::setChronometryCharactersCharacters(int _value)
{
    m_ui->charactersChronometryCharacters->setValue(_value);
}

void SettingsView::setChronometryCharactersSeconds(int _value)
{
    m_ui->charactersChronometrySeconds->setValue(_value);
}

void SettingsView::setChronometryCharactersConsiderSpaces(bool _value)
{
    m_ui->charactersChronometryConsiderSpaces->setChecked(_value);
}

void SettingsView::setChronometryConfigurableSecondsForParagraphSceneHeading(double _value)
{
    m_ui->configurableChronometrySecondsForParagraphSceneHeading->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsFor50SceneHeading(double _value)
{
    m_ui->configurableChronometrySecondsPer50CharactersSceneHeading->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsForParagraphAction(double _value)
{
    m_ui->configurableChronometrySecondsForParagraphAction->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsFor50Action(double _value)
{
    m_ui->configurableChronometrySecondsPer50CharactersAction->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsForParagraphDialog(double _value)
{
    m_ui->configurableChronometrySecondsForParagraphDialog->setValue(_value);
}

void SettingsView::setChronometryConfigurableSecondsFor50Dialog(double _value)
{
    m_ui->configurableChronometrySecondsPer50CharactersDialog->setValue(_value);
}

void SettingsView::setPagesCounterUsed(bool _value)
{
    m_ui->pagesCounter->setChecked(_value);
}

void SettingsView::setWordsCounterUsed(bool _value)
{
    m_ui->wordsCounter->setChecked(_value);
}

void SettingsView::setSimbolsCounterUsed(bool _value)
{
    m_ui->simbolsCounter->setChecked(_value);
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
    emit scenarioEditSpellCheckLanguageChanged(m_ui->spellCheckingLanguage->currentData().toInt());
}

void SettingsView::aboutBrowseSaveBackupsFolder()
{
    const QString folder =
            QFileDialog::getExistingDirectory(this, tr("Choose backups saving folder"),
                m_ui->saveBackupsFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folder.isEmpty()) {
        m_ui->saveBackupsFolder->setText(folder);
    }
}

void SettingsView::aboutCardsChooseBackgroundColor()
{
    setColorFor(m_ui->cardsBackgroundColor);
    emit cardsBackgroundColorChanged(m_ui->cardsBackgroundColor->palette().button().color());
}

void SettingsView::aboutCardsChooseBackgroundColorDark()
{
    setColorFor(m_ui->cardsBackgroundColorDark);
    emit cardsBackgroundColorDarkChanged(m_ui->cardsBackgroundColorDark->palette().button().color());
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
    setColorFor(m_ui->textColor);
    emit scenarioEditTextColorChanged(m_ui->textColor->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseBackgroundColor()
{
    setColorFor(m_ui->backgroundColor);
    emit scenarioEditBackgroundColorChanged(m_ui->backgroundColor->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseNonprintableTextColor()
{
    setColorFor(m_ui->nonprintableTextColor);
    emit scenarioEditNonprintableTextColorChanged(m_ui->nonprintableTextColor->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseFolderTextColor()
{
    setColorFor(m_ui->folderTextColor);
    emit scenarioEditFolderTextColorChanged(m_ui->folderTextColor->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseFolderBackgroundColor()
{
    setColorFor(m_ui->folderBackgroundColor);
    emit scenarioEditFolderBackgroundColorChanged(m_ui->folderBackgroundColor->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseTextColorDark()
{
    setColorFor(m_ui->textColorDark);
    emit scenarioEditTextColorDarkChanged(m_ui->textColorDark->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseBackgroundColorDark()
{
    setColorFor(m_ui->backgroundColorDark);
    emit scenarioEditBackgroundColorDarkChanged(m_ui->backgroundColorDark->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseNonprintableTextColorDark()
{
    setColorFor(m_ui->nonprintableTextColorDark);
    emit scenarioEditNonprintableTextColorDarkChanged(m_ui->nonprintableTextColorDark->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseFolderTextColorDark()
{
    setColorFor(m_ui->folderTextColorDark);
    emit scenarioEditFolderTextColorDarkChanged(m_ui->folderTextColorDark->palette().button().color());
}

void SettingsView::aboutScenarioEditChooseFolderBackgroundColorDark()
{
    setColorFor(m_ui->folderBackgroundColorDark);
    emit scenarioEditFolderBackgroundColorDarkChanged(m_ui->folderBackgroundColorDark->palette().button().color());
}

void SettingsView::aboutEditTemplatePressed()
{
    if (!m_ui->templates->selectionModel()->selectedIndexes().isEmpty()) {
        QModelIndex selected = m_ui->templates->selectionModel()->selectedIndexes().first();
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
    if (!m_ui->templates->selectionModel()->selectedIndexes().isEmpty()) {
        QModelIndex selected = m_ui->templates->selectionModel()->selectedIndexes().first();
        if (selected.isValid()) {
            emit templateLibraryRemovePressed(selected);
        }
    }
}

void SettingsView::aboutSaveTemplatePressed()
{
    if (!m_ui->templates->selectionModel()->selectedIndexes().isEmpty()) {
        QModelIndex selected = m_ui->templates->selectionModel()->selectedIndexes().first();
        if (selected.isValid()) {
            emit templateLibrarySavePressed(selected);
        }
    }
}

void SettingsView::aboutApplyTemplatePressed()
{
    if (!m_ui->templates->selectionModel()->selectedIndexes().isEmpty()) {
        QModelIndex selected = m_ui->templates->selectionModel()->selectedIndexes().first();
        if (selected.isValid()) {
            const QString templateName = selected.data().toString();
            m_ui->currentScenarioTemplate->setCurrentText(templateName);
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
    m_ui->spellCheckingLanguage->addItem(tr("Russian with Yo"), SpellChecker::RussianWithYo);
    m_ui->spellCheckingLanguage->addItem(tr("Russian"), SpellChecker::Russian);
    m_ui->spellCheckingLanguage->addItem(tr("Armenian (Eastern)"), SpellChecker::ArmenianEastern);
    m_ui->spellCheckingLanguage->addItem(tr("Armenian (Western)"), SpellChecker::ArmenianWestern);
    m_ui->spellCheckingLanguage->addItem(tr("Belorussian"), SpellChecker::Belorussian);
    m_ui->spellCheckingLanguage->addItem(tr("English (GB)"), SpellChecker::EnglishGB);
    m_ui->spellCheckingLanguage->addItem(tr("English (US)"), SpellChecker::EnglishUS);
    m_ui->spellCheckingLanguage->addItem(tr("Farsi"), SpellChecker::Farsi);
    m_ui->spellCheckingLanguage->addItem(tr("French"), SpellChecker::French);
    m_ui->spellCheckingLanguage->addItem(tr("Kazakh"), SpellChecker::Kazakh);
    m_ui->spellCheckingLanguage->addItem(tr("Portuguese (Brazil)"), SpellChecker::PortugueseBrazilian);
    m_ui->spellCheckingLanguage->addItem(tr("Portuguese (Portugal)"), SpellChecker::Portuguese);
    m_ui->spellCheckingLanguage->addItem(tr("Spanish"), SpellChecker::Spanish);
    m_ui->spellCheckingLanguage->addItem(tr("Swedish"), SpellChecker::Swedish);
    m_ui->spellCheckingLanguage->addItem(tr("Ukrainian"), SpellChecker::Ukrainian);
}

void SettingsView::initView()
{
    m_ui->categories->setCurrentRow(0);
    m_ui->categoriesWidgets->setCurrentIndex(0);

    m_ui->settingsSplitter->setHandleWidth(1);
    m_ui->settingsSplitter->setOpaqueResize(false);
    m_ui->settingsSplitter->setStretchFactor(1, 1);

    m_ui->spellCheckingLanguage->setCurrentIndex(0);
    m_ui->spellCheckingLanguage->setEnabled(false);
    m_ui->browseBackupFolder->updateIcons();

    m_applicationTabs->addTab(tr("Common"));
    m_applicationTabs->addTab(tr("Modules"));
    m_applicationTabs->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    m_ui->applicationPageLayout->addWidget(m_applicationTabs, 0, 0);
    m_ui->applicationPageLayout->addWidget(m_ui->topRightEmptyLabel_1, 0, 1);
    m_ui->applicationPageStack->setCurrentIndex(0);

    m_scenarioEditorTabs->addTab(tr("Common"));
    m_scenarioEditorTabs->addTab(tr("Editing"));
    m_scenarioEditorTabs->addTab(tr("Shortcuts"));
    m_scenarioEditorTabs->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    m_ui->scenarioEditPageLayout->addWidget(m_scenarioEditorTabs, 0, 0);
    m_ui->scenarioEditPageLayout->addWidget(m_ui->topRightEmptyLabel_2, 0, 1);
    m_ui->scenarioEditPageStack->setCurrentIndex(0);

    m_jumpsTableHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_ui->scenarioEditBlockSettingsTable->setHorizontalHeader(m_jumpsTableHeader);
    m_ui->scenarioEditBlockSettingsTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_ui->newTemplate->updateIcons();
    m_ui->editTemplate->updateIcons();
    m_ui->saveTemplate->updateIcons();
    m_ui->loadTemplate->updateIcons();
    m_ui->removeTemplate->updateIcons();
    m_ui->applyTemplate->updateIcons();
    m_ui->templates->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void SettingsView::initConnections()
{
    //
    // Настроим соединения формы
    //
    connect(m_ui->categories, SIGNAL(currentRowChanged(int)), m_ui->categoriesWidgets, SLOT(setCurrentIndex(int)));
    // ... смена вкладок страницы настроек приложения
    connect(m_applicationTabs, &TabBar::currentChanged, m_ui->applicationPageStack, &QStackedWidget::setCurrentIndex);
    // ... активация автосохранения
    connect(m_ui->autosave, SIGNAL(toggled(bool)), m_ui->autosaveInterval, SLOT(setEnabled(bool)));
    // ... корректировка описания автосохранения
    connect(m_ui->autosaveInterval, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] (int _interval) {
        m_ui->autosaveDescription->setToolTip(
            tr("Autosave works very accuracy. It saves project every 3 seconds if you don't use mouse or keyboard, \n"
               "but if you work with no terminations it saves project every %1 minutes.").arg(_interval));
    });
    // ... активация проверки орфографии
    connect(m_ui->spellChecking, SIGNAL(toggled(bool)), m_ui->spellCheckingLanguage, SLOT(setEnabled(bool)));
    // ... выбор папки сохранения резервных копий
    connect(m_ui->browseBackupFolder, SIGNAL(clicked()), this, SLOT(aboutBrowseSaveBackupsFolder()));
    // ... активность выбора цветов заливки фона карточек
    connect(m_ui->cardsUseColorsBackground, &QRadioButton::toggled, m_ui->cardsColorsGroup, &QGroupBox::setEnabled);
    // ... выбор цвета элементов редактора сценария
    connect(m_ui->cardsBackgroundColor, SIGNAL(clicked()), this, SLOT(aboutCardsChooseBackgroundColor()));
    connect(m_ui->cardsBackgroundColorDark, SIGNAL(clicked()), this, SLOT(aboutCardsChooseBackgroundColorDark()));
    // ... смена вкладок страницы настройки редактора сценария
    connect(m_scenarioEditorTabs, SIGNAL(currentChanged(int)), m_ui->scenarioEditPageStack, SLOT(setCurrentIndex(int)));
    // ... выбор цвета элементов редактора сценария
    connect(m_ui->textColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseTextColor()));
    connect(m_ui->backgroundColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseBackgroundColor()));
    connect(m_ui->nonprintableTextColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseNonprintableTextColor()));
    connect(m_ui->folderTextColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderTextColor()));
    connect(m_ui->folderBackgroundColor, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderBackgroundColor()));
    connect(m_ui->textColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseTextColorDark()));
    connect(m_ui->backgroundColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseBackgroundColorDark()));
    connect(m_ui->nonprintableTextColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseNonprintableTextColorDark()));
    connect(m_ui->folderTextColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderTextColorDark()));
    connect(m_ui->folderBackgroundColorDark, SIGNAL(clicked()), this, SLOT(aboutScenarioEditChooseFolderBackgroundColorDark()));
    // ... смена текущей системы хронометража
    connect(m_ui->pagesChronometry, SIGNAL(toggled(bool)), m_ui->pagesChronometryGroup, SLOT(setEnabled(bool)));
    connect(m_ui->charactersChronometry, SIGNAL(toggled(bool)), m_ui->charactersChronometryGroup, SLOT(setEnabled(bool)));
    connect(m_ui->configurableChronometry, SIGNAL(toggled(bool)), m_ui->configurableChronometryGroup, SLOT(setEnabled(bool)));

    //
    // Сбросить настройки
    //
    connect(m_ui->resetSettings, SIGNAL(clicked(bool)), this, SIGNAL(resetSettings()));

    //
    // Сигналы об изменении параметров
    //
    // ... приложение
    connect(m_ui->changeLanguage, SIGNAL(clicked(bool)), this, SLOT(aboutChooseApplicationLanguage()));
    connect(m_ui->useDarkTheme, SIGNAL(toggled(bool)), this, SIGNAL(applicationUseDarkThemeChanged(bool)));
    connect(m_ui->autosave, SIGNAL(toggled(bool)), this, SIGNAL(applicationAutosaveChanged(bool)));
    connect(m_ui->autosaveInterval, SIGNAL(valueChanged(int)), this, SIGNAL(applicationAutosaveIntervalChanged(int)));
    connect(m_ui->saveBackups, SIGNAL(toggled(bool)), this, SIGNAL(applicationSaveBackupsChanged(bool)));
    connect(m_ui->saveBackupsFolder, SIGNAL(textChanged(QString)), this, SIGNAL(applicationSaveBackupsFolderChanged(QString)));
    connect(m_ui->applicationCompactMode, &QCheckBox::toggled, this, &SettingsView::applicationCompactModeChanged);
    connect(m_ui->applicationTwoPanelMode, &QCheckBox::toggled, this, &SettingsView::applicationTwoPanelModeChanged);
    connect(m_ui->applicationModuleResearch, &QCheckBox::toggled, this, &SettingsView::applicationModuleResearchChanged);
    connect(m_ui->applicationModuleCards, &QCheckBox::toggled, this, &SettingsView::applicationModuleCardsChanged);
    connect(m_ui->applicationModuleScenario, &QCheckBox::toggled, this, &SettingsView::applicationModuleScenarioChanged);
    connect(m_ui->applicationModuleStatistics, &QCheckBox::toggled, this, &SettingsView::applicationModuleStatisticsChanged);
    // ... карточки
    connect(m_ui->cardsUseCorkboardBackground, &QRadioButton::toggled, this, &SettingsView::cardsUseCorkboardBackgroundChanged);
    // ... текстовый редактор
    connect(m_ui->pageView, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditPageViewChanged(bool)));
    connect(m_ui->showScenesNumbersInEditor, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditShowScenesNumbersChanged(bool)));
    connect(m_ui->highlightCurrentLine, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditHighlightCurrentLineChanged(bool)));
    connect(m_ui->capitalizeFirstWord, &QCheckBox::toggled, this, &SettingsView::scenarioEditCapitalizeFirstWordChanged);
    connect(m_ui->correctDoubleCapitals, &QCheckBox::toggled, this, &SettingsView::scenarioEditCorrectDoubleCapitalsChanged);
    connect(m_ui->replaceThreeDots, &QCheckBox::toggled, this, &SettingsView::scenarioEditReplaceThreeDotsChanged);
    connect(m_ui->smartQuotes, &QCheckBox::toggled, this, &SettingsView::scenarioEditSmartQuotesChanged);
    connect(m_ui->spellChecking, &QCheckBox::toggled, [=] (bool _checked) {
        emit scenarioEditSpellCheckChanged(_checked);
        if (_checked) {
            aboutScenarioEditSpellCheckLanguageChanged();
        }
    });
    connect(m_ui->spellCheckingLanguage, SIGNAL(currentIndexChanged(int)), this, SLOT(aboutScenarioEditSpellCheckLanguageChanged()));
    connect(m_ui->currentScenarioTemplate, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(scenarioEditCurrentTemplateChanged(QString)));
    connect(m_ui->autoJumpToNextBlock, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditAutoJumpToNextBlockChanged(bool)));
    connect(m_ui->showSuggestionsInEmptyBlocks, &QCheckBox::toggled, this, &SettingsView::scenarioEditShowSuggestionsInEmptyBlocksChanged);
    connect(m_ui->reviewUseWordHighlight, SIGNAL(toggled(bool)), this, SIGNAL(scenarioEditReviewUseWordHighlightChanged(bool)));
    // ... навигатор
    connect(m_ui->showScenesNumbersInNavigator, SIGNAL(toggled(bool)), this, SIGNAL(navigatorShowScenesNumbersChanged(bool)));
    connect(m_ui->showSceneTitle, &QCheckBox::toggled, this, &SettingsView::navigatorShowSceneTitleChanged);
    connect(m_ui->showSceneDescription, SIGNAL(toggled(bool)), this, SIGNAL(navigatorShowSceneDescriptionChanged(bool)));
    connect(m_ui->sceneDescriptionIsSceneText, SIGNAL(toggled(bool)), this, SIGNAL(navigatorSceneDescriptionIsSceneTextChanged(bool)));
    connect(m_ui->sceneDescriptionHeight, SIGNAL(valueChanged(int)), this, SIGNAL(navigatorSceneDescriptionHeightChanged(int)));
    // ... хронометраж
    connect(m_ui->chronometryGroup, SIGNAL(toggled(bool)), this, SIGNAL(chronometryUsedChanged(bool)));
    connect(m_ui->pagesChronometry, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCurrentTypeChanged()));
    connect(m_ui->charactersChronometry, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCurrentTypeChanged()));
    connect(m_ui->configurableChronometry, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCurrentTypeChanged()));
    connect(m_ui->pagesChronometrySeconds, SIGNAL(valueChanged(int)), this, SIGNAL(chronometryPagesSecondsChanged(int)));
    connect(m_ui->charactersChronometryCharacters, SIGNAL(valueChanged(int)), this, SIGNAL(chronometryCharactersCharactersChanged(int)));
    connect(m_ui->charactersChronometrySeconds, SIGNAL(valueChanged(int)), this, SIGNAL(chronometryCharactersSecondsChanged(int)));
    connect(m_ui->charactersChronometryConsiderSpaces, SIGNAL(toggled(bool)), this, SIGNAL(chronometryCharactersConsiderSpaces(bool)));
    connect(m_ui->configurableChronometrySecondsForParagraphSceneHeading, SIGNAL(valueChanged(double)),
            this, SIGNAL(chronometryConfigurableSecondsForParagraphSceneHeadingChanged(double)));
    connect(m_ui->configurableChronometrySecondsPer50CharactersSceneHeading, SIGNAL(valueChanged(double)),
            this, SIGNAL(chronometryConfigurableSecondsFor50SceneHeadingChanged(double)));
    connect(m_ui->configurableChronometrySecondsForParagraphAction, SIGNAL(valueChanged(double)),
            this, SIGNAL(chronometryConfigurableSecondsForParagraphActionChanged(double)));
    connect(m_ui->configurableChronometrySecondsPer50CharactersAction, SIGNAL(valueChanged(double)),
            this, SIGNAL(chronometryConfigurableSecondsFor50ActionChanged(double)));
    connect(m_ui->configurableChronometrySecondsForParagraphDialog, SIGNAL(valueChanged(double)),
            this, SIGNAL(chronometryConfigurableSecondsForParagraphDialogChanged(double)));
    connect(m_ui->configurableChronometrySecondsPer50CharactersDialog, SIGNAL(valueChanged(double)),
            this, SIGNAL(chronometryConfigurableSecondsFor50DialogChanged(double)));
    // ... счётчики
    connect(m_ui->pagesCounter, SIGNAL(toggled(bool)), this, SIGNAL(pagesCounterUsedChanged(bool)));
    connect(m_ui->wordsCounter, SIGNAL(toggled(bool)), this, SIGNAL(wordsCounterUsedChanged(bool)));
    connect(m_ui->simbolsCounter, SIGNAL(toggled(bool)), this, SIGNAL(simbolsCounterUsedChanged(bool)));

    //
    // Библиотека стилей
    //
    connect(m_ui->newTemplate, SIGNAL(clicked()), this, SIGNAL(templateLibraryNewPressed()));
    connect(m_ui->editTemplate, SIGNAL(clicked()), this, SLOT(aboutEditTemplatePressed()));
    connect(m_ui->templates, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(aboutEditTemplatePressed()));
    connect(m_ui->removeTemplate, SIGNAL(clicked()), this, SLOT(aboutRemoveTemplatePressed()));
    connect(m_ui->loadTemplate, SIGNAL(clicked()), this, SIGNAL(templateLibraryLoadPressed()));
    connect(m_ui->saveTemplate, SIGNAL(clicked()), this, SLOT(aboutSaveTemplatePressed()));
    connect(m_ui->applyTemplate, SIGNAL(clicked()), this, SLOT(aboutApplyTemplatePressed()));
}

void SettingsView::initStyleSheet()
{
    //
    // Верхние метки для заполнения пространства
    //
    QList<QWidget*> topEmptyList;
    topEmptyList << m_ui->topLeftEmptyLabel
                 << m_ui->topRightEmptyLabel_1
                 << m_ui->topRightEmptyLabel_2
                 << m_ui->topRightEmptyLabel_3
                 << m_ui->topRightEmptyLabel_4
                 << m_ui->topRightEmptyLabel_5
                 << m_ui->topRightEmptyLabel_7
                 << m_ui->topRightEmptyLabel_8
                 << m_ui->topRightEmptyLabel_9;

    foreach (QWidget* topEmpty, topEmptyList) {
        topEmpty->setProperty("inTopPanel", true);
        topEmpty->setProperty("topPanelTopBordered", true);
    }

    //
    // Основные контейнеры с содержимым
    //
    QList<QWidget*> mainList;
    mainList << m_ui->categories
             << m_ui->applicationPageStack
             << m_ui->cardsPageWidget
             << m_ui->scenarioEditPageStack
             << m_ui->navigatorPageWidget
             << m_ui->chronometryPageWidget
             << m_ui->templatesLibraryPageWidget;

    foreach (QWidget* main, mainList) {
        main->setProperty("mainContainer", true);
    }

    //
    // Кнопки панели инструментов
    //
    QList<QWidget*> topButtonsList;
    topButtonsList << m_ui->newTemplate
                   << m_ui->editTemplate
                   << m_ui->removeTemplate
                   << m_ui->loadTemplate
                   << m_ui->saveTemplate
                   << m_ui->applyTemplate;
    foreach (QWidget* topButton, topButtonsList) {
        topButton->setProperty("inTopPanel", true);
    }

    //
    // Вкладки
    //
    m_applicationTabs->setProperty("inTopPanel", true);
    m_scenarioEditorTabs->setProperty("inTopPanel", true);

    //
    // Таблицы
    //
    m_ui->scenarioEditBlockSettingsTable->setProperty("nobordersContainer", true);
    m_ui->templates->setProperty("mainContainer", true);

    //
    // Кнопка сброса изменений
    //
    m_ui->resetSettings->setStyleSheet("border-left: none; border-right: none;");

    m_ui->browseBackupFolder->setProperty("isBrowseButton", true);
}

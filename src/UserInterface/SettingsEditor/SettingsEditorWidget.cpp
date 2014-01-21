#include "SettingsEditorWidget.h"
#include "ui_SettingsEditorWidget.h"

#include <Storage/StorageFacade.h>
#include <Storage/SettingsStorage.h>

#include <BusinessLogic/Chronometry/PagesChronometer.h>
#include <BusinessLogic/Chronometry/CharactersChronometer.h>
#include <BusinessLogic/Chronometry/ConfigurableChronometer.h>

#include <QListWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QStackedLayout>

using namespace StorageLayer;


SettingsEditorWidget::SettingsEditorWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::SettingsEditorWidget)
{
	m_ui->setupUi(this);

	initView();
	initConnections();

	loadSettings();

	initSaveSettingsConnections();
}

SettingsEditorWidget::~SettingsEditorWidget()
{
	delete m_ui;
}

void SettingsEditorWidget::aboutShowCategorySettings(int _categoryRow)
{
	categoriesWidgets()->setCurrentIndex(_categoryRow);
}

void SettingsEditorWidget::saveSettings()
{
	//
	// Настройки текстового редактора
	//
	StorageFacade::settingsStorage()->setValue(
				"text-editor/spell-checking",
				m_ui->spellChecking->isChecked() ? "1" : "0",
				SettingsStorage::ApplicationSettings);

	//
	// Настройки хронометража
	//

	// ... текущая система
	QString chronometryType;
	if (m_ui->pagesChronometry->isChecked()) {
		chronometryType = PagesChronometer().name();
	} else if (m_ui->charactersChronometry->isChecked()) {
		chronometryType = CharactersChronometer().name();
	} else {
		chronometryType = ConfigurableChronometer().name();
	}
	StorageFacade::settingsStorage()->setValue(
				"chronometry/current-chronometer-type",
				chronometryType,
				SettingsStorage::ApplicationSettings);

	// ... параметры систем
	StorageFacade::settingsStorage()->setValue(
				"chronometry/pages/seconds",
				m_ui->pagesChronometrySeconds->text(),
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"chronometry/characters/characters",
				m_ui->charactersChronometryCharacters->text(),
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"chronometry/characters/seconds",
				m_ui->charactersChronometrySeconds->text(),
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"chronometry/configurable/seconds-for-paragraph/time-and-place",
				QString::number(m_ui->configurableChronometrySecondsForParagraphTimeAndPlace->value()),
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"chronometry/configurable/seconds-for-every-50/time-and-place",
				QString::number(m_ui->configurableChronometrySecondsPer50CharactersTimeAndPlace->value()),
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"chronometry/configurable/seconds-for-paragraph/action",
				QString::number(m_ui->configurableChronometrySecondsForParagraphAction->value()),
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"chronometry/configurable/seconds-for-every-50/action",
				QString::number(m_ui->configurableChronometrySecondsPer50CharactersAction->value()),
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"chronometry/configurable/seconds-for-paragraph/dialog",
				QString::number(m_ui->configurableChronometrySecondsForParagraphDialog->value()),
				SettingsStorage::ApplicationSettings);
	StorageFacade::settingsStorage()->setValue(
				"chronometry/configurable/seconds-for-for-every-50/dialog",
				QString::number(m_ui->configurableChronometrySecondsPer50CharactersDialog->value()),
				SettingsStorage::ApplicationSettings);
}

void SettingsEditorWidget::initView()
{
	categories()->setCurrentRow(0);
	categoriesWidgets()->setCurrentIndex(0);
}

void SettingsEditorWidget::initConnections()
{
	connect(categories(), SIGNAL(currentRowChanged(int)), this, SLOT(aboutShowCategorySettings(int)));
	connect(m_ui->pagesChronometry, SIGNAL(toggled(bool)), m_ui->pagesChronometryGroup, SLOT(setEnabled(bool)));
	connect(m_ui->charactersChronometry, SIGNAL(toggled(bool)), m_ui->charactersChronometryGroup, SLOT(setEnabled(bool)));
	connect(m_ui->configurableChronometry, SIGNAL(toggled(bool)), m_ui->configurableChronometryGroup, SLOT(setEnabled(bool)));

	connect(m_ui->spellChecking, SIGNAL(toggled(bool)), this, SIGNAL(useSpellCheckingChanged(bool)));
}

void SettingsEditorWidget::initSaveSettingsConnections()
{
	connect(m_ui->spellChecking, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
	connect(m_ui->pagesChronometry, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
	connect(m_ui->pagesChronometrySeconds, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
	connect(m_ui->charactersChronometry, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
	connect(m_ui->charactersChronometryCharacters, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
	connect(m_ui->charactersChronometrySeconds, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
	connect(m_ui->configurableChronometry, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
	connect(m_ui->configurableChronometrySecondsForParagraphTimeAndPlace, SIGNAL(valueChanged(double)), this, SLOT(saveSettings()));
	connect(m_ui->configurableChronometrySecondsPer50CharactersTimeAndPlace, SIGNAL(valueChanged(double)), this, SLOT(saveSettings()));
	connect(m_ui->configurableChronometrySecondsForParagraphAction, SIGNAL(valueChanged(double)), this, SLOT(saveSettings()));
	connect(m_ui->configurableChronometrySecondsPer50CharactersAction, SIGNAL(valueChanged(double)), this, SLOT(saveSettings()));
	connect(m_ui->configurableChronometrySecondsForParagraphDialog, SIGNAL(valueChanged(double)), this, SLOT(saveSettings()));
	connect(m_ui->configurableChronometrySecondsPer50CharactersDialog, SIGNAL(valueChanged(double)), this, SLOT(saveSettings()));
}

void SettingsEditorWidget::loadSettings()
{
	//
	// Настройки текстового редактора
	//
	m_ui->spellChecking->setChecked(
				StorageFacade::settingsStorage()->value(
					"text-editor/spell-checking",
					SettingsStorage::ApplicationSettings)
				.toInt()
				);

	//
	// Настройки хронометража
	//

	// ... текущая система
	QString chronometryType =
			StorageFacade::settingsStorage()->value(
				"chronometry/current-chronometer-type",
				SettingsStorage::ApplicationSettings);
	if (chronometryType == PagesChronometer().name()) {
		m_ui->pagesChronometry->setChecked(true);
	} else if (chronometryType == CharactersChronometer().name()) {
		m_ui->charactersChronometry->setChecked(true);
	} else {
		m_ui->configurableChronometry->setChecked(true);
	}

	// ... параметры систем
	m_ui->pagesChronometrySeconds->setValue(
				StorageFacade::settingsStorage()->value(
					"chronometry/pages/seconds",
					SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_ui->charactersChronometryCharacters->setValue(
				StorageFacade::settingsStorage()->value(
					"chronometry/characters/characters",
					SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_ui->charactersChronometrySeconds->setValue(
				StorageFacade::settingsStorage()->value(
					"chronometry/characters/seconds",
					SettingsStorage::ApplicationSettings)
				.toInt()
				);
	m_ui->configurableChronometrySecondsForParagraphTimeAndPlace->setValue(
				StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-paragraph/time-and-place",
					SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_ui->configurableChronometrySecondsPer50CharactersTimeAndPlace->setValue(
				StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-every-50/time-and-place",
					SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_ui->configurableChronometrySecondsForParagraphAction->setValue(
				StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-paragraph/action",
					SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_ui->configurableChronometrySecondsPer50CharactersAction->setValue(
				StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-every-50/action",
					SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_ui->configurableChronometrySecondsForParagraphDialog->setValue(
				StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-paragraph/dialog",
					SettingsStorage::ApplicationSettings)
				.toDouble()
				);
	m_ui->configurableChronometrySecondsPer50CharactersDialog->setValue(
				StorageFacade::settingsStorage()->value(
					"chronometry/configurable/seconds-for-for-every-50/dialog",
					SettingsStorage::ApplicationSettings)
				.toDouble()
				);
}

QListWidget* SettingsEditorWidget::categories()
{
	return m_ui->categories;
}

QStackedWidget* SettingsEditorWidget::categoriesWidgets()
{
	return m_ui->categoriesWidgets;
}

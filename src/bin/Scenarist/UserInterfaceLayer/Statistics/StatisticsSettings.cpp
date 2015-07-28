#include "StatisticsSettings.h"
#include "ui_StatisticsSettings.h"

using UserInterface::StatisticsSettings;


StatisticsSettings::StatisticsSettings(QWidget *parent) :
	QStackedWidget(parent),
	ui(new Ui::StatisticsSettings)
{
	ui->setupUi(this);

	initView();
	initConnections();
}

StatisticsSettings::~StatisticsSettings()
{
	delete ui;
}

void StatisticsSettings::setCharacters(QAbstractItemModel* _characters)
{
	ui->characterName->setModel(_characters);
	if (_characters != 0) {
		ui->characterName->setCurrentIndex(_characters->index(0, 0));
		connect(ui->characterName->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SIGNAL(settingsChanged()));
	}
}

const BusinessLogic::ReportParameters& StatisticsSettings::settings() const
{
	m_settings.summaryText = ui->summaryText->isChecked();
	m_settings.summaryScenes = ui->summaryScenes->isChecked();
	m_settings.summaryLocations = ui->summaryLocations->isChecked();
	m_settings.summaryCharacters = ui->summaryCharacters->isChecked();

	m_settings.sceneShowCharacters = ui->sceneShowCharacters->isChecked();
	m_settings.sceneSortByColumn = ui->sceneSortBy->currentIndex();

	m_settings.locationExtendedView = ui->locationExtendedView->isChecked();
	m_settings.locationSortByColumn = ui->locationSortBy->currentIndex();

	m_settings.castShowSpeakingAndNonspeakingScenes = ui->castShowSpeakingAndNonspeakingScenes->isChecked();
	m_settings.castSortByColumn = ui->castSortBy->currentIndex();

	m_settings.characterName = ui->characterName->currentIndex().data().toString();

	return m_settings;
}

void StatisticsSettings::initView()
{
	setCurrentIndex(0);
}

void StatisticsSettings::initConnections()
{
	connect(ui->summaryText, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->summaryScenes, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->summaryLocations, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->summaryCharacters, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));

	connect(ui->sceneShowCharacters, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->sceneSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(ui->locationExtendedView, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->locationSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(ui->castShowSpeakingAndNonspeakingScenes, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->castSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));
}

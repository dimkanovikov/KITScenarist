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
	ui->characterName->setCurrentIndex(_characters->index(0, 0));
	connect(ui->characterName->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SIGNAL(settingsChanged()));
}

const BusinessLogic::ReportParameters& StatisticsSettings::settings() const
{
	m_settings.statisticsSummaryText = ui->statisticsSummaryText->isChecked();
	m_settings.statisticsScenes = ui->statisticsScenes->isChecked();
	m_settings.statisticsLocations = ui->statisticsLocations->isChecked();
	m_settings.statisticsCharacters = ui->statisticsCharacters->isChecked();

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
	connect(ui->statisticsSummaryText, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->statisticsScenes, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->statisticsLocations, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->statisticsCharacters, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));

	connect(ui->sceneShowCharacters, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->sceneSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(ui->locationExtendedView, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->locationSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(ui->castShowSpeakingAndNonspeakingScenes, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->castSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));
}

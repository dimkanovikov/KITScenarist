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
	connect(ui->characterName->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SIGNAL(settingsChanged()));
}

void StatisticsSettings::setScriptElements(QAbstractItemModel* _elements)
{
	ui->scriptElements->setModel(_elements);
	connect(ui->scriptElements->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SIGNAL(settingsChanged()));
}

const BusinessLogic::ReportParameters& StatisticsSettings::settings() const
{
	m_settings.statisticsSummaryText = ui->statisticsSummaryText->isChecked();
	m_settings.statisticsScenes = ui->statisticsScenes->isChecked();
	m_settings.statisticsLocations = ui->statisticsLocations->isChecked();
	m_settings.statisticsCharacters = ui->statisticsCharacters->isChecked();

	m_settings.sceneSortByColumn = ui->sceneSortBy->currentIndex();

	m_settings.locationSortByColumn = ui->locationSortBy->currentIndex();

	m_settings.castNumberOfScenes = ui->castNumberOfScenes->isChecked();
	m_settings.castNumberOdNonspekingScenes = ui->castNumberOfNonspeakingScenes->isChecked();
	m_settings.castTotalSceneAppearance = ui->castTotalSceneAppearances->isChecked();
	m_settings.castTotalDialogues = ui->castTotalDialogues->isChecked();
	m_settings.castSortByColumn = ui->castSortBy->currentIndex();

	m_settings.characterName = ui->characterName->currentIndex().data().toString();
	m_settings.characterIncludeSceneHeadings = ui->characterIncludeSceneHeadings->isChecked();
	m_settings.characterIncludeDialogs = ui->characterIncludeDialogues->isChecked();
	m_settings.characterIncludeArcBeats = ui->characterIncludeArcBeats->isChecked();

	QStringList scriptElements;
	if (ui->scriptElements->model() != 0) {
		foreach (const QModelIndex& index, ui->scriptElements->selectionModel()->selectedRows()) {
			scriptElements << index.data().toString();
		}
	}
	m_settings.scriptElements = scriptElements;
	m_settings.scriptUseCourierNew = ui->scriptUseCourierNew->isChecked();

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

	connect(ui->sceneSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(ui->locationSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(ui->castNumberOfScenes, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->castNumberOfNonspeakingScenes, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->castTotalSceneAppearances, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->castTotalDialogues, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->castSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(ui->characterIncludeSceneHeadings, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->characterIncludeDialogues, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(ui->characterIncludeArcBeats, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));

	connect(ui->scriptUseCourierNew, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
}

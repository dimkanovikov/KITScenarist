#include "StatisticsSettings.h"
#include "ui_StatisticsSettings.h"

#include <QSortFilterProxyModel>

using UserInterface::StatisticsSettings;


StatisticsSettings::StatisticsSettings(QWidget *parent) :
	QStackedWidget(parent),
	m_ui(new Ui::StatisticsSettings),
	m_charactersModel(new QSortFilterProxyModel(this))
{
	m_ui->setupUi(this);

	initView();
	initConnections();
}

StatisticsSettings::~StatisticsSettings()
{
	delete m_ui;
}

void StatisticsSettings::setCharacters(QAbstractItemModel* _characters)
{
	m_charactersModel->setSourceModel(_characters);
	if (_characters != 0) {
		m_charactersModel->sort(0);
		m_ui->characterName->setCurrentIndex(_characters->index(0, 0));
		m_ui->characterActivityNames->selectAll();
	}
}

const BusinessLogic::StatisticsParameters& StatisticsSettings::settings() const
{
	m_settings.summaryText = m_ui->summaryText->isChecked();
	m_settings.summaryScenes = m_ui->summaryScenes->isChecked();
	m_settings.summaryLocations = m_ui->summaryLocations->isChecked();
	m_settings.summaryCharacters = m_ui->summaryCharacters->isChecked();

	m_settings.sceneShowCharacters = m_ui->sceneShowCharacters->isChecked();
	m_settings.sceneSortByColumn = m_ui->sceneSortBy->currentIndex();

	m_settings.locationExtendedView = m_ui->locationExtendedView->isChecked();
	m_settings.locationSortByColumn = m_ui->locationSortBy->currentIndex();

	m_settings.castShowSpeakingAndNonspeakingScenes = m_ui->castShowSpeakingAndNonspeakingScenes->isChecked();
	m_settings.castSortByColumn = m_ui->castSortBy->currentIndex();

    m_settings.characterNames.clear();
    if (m_ui->characterName->selectionModel() != 0) {
        foreach (const QModelIndex& index, m_ui->characterName->selectionModel()->selectedIndexes()) {
            m_settings.characterNames.append(index.data().toString());
        }
    }

	m_settings.storyStructureAnalisysSceneChron = m_ui->storyStructureAnalisysSceneChron->isChecked();
	m_settings.storyStructureAnalisysActionChron = m_ui->storyStructureAnalisysActionChron->isChecked();
	m_settings.storyStructureAnalisysDialoguesChron = m_ui->storyStructureAnalisysDialoguesChron->isChecked();
	m_settings.storyStructureAnalisysCharactersCount = m_ui->storyStructureAnalisysCharactersCount->isChecked();
	m_settings.storyStructureAnalisysDialoguesCount = m_ui->storyStructureAnalisysDialoguesCount->isChecked();

	m_settings.charactersActivityNames.clear();
	if (m_ui->characterActivityNames->selectionModel() != 0) {
		foreach (const QModelIndex& index, m_ui->characterActivityNames->selectionModel()->selectedIndexes()) {
			m_settings.charactersActivityNames.append(index.data().toString());
		}
	}

	return m_settings;
}

void StatisticsSettings::initView()
{
	setCurrentIndex(0);

	m_ui->characterName->setModel(m_charactersModel);
	m_ui->characterActivityNames->setModel(m_charactersModel);
}

void StatisticsSettings::initConnections()
{
	connect(m_ui->summaryText, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(m_ui->summaryScenes, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(m_ui->summaryLocations, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(m_ui->summaryCharacters, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));

	connect(m_ui->sceneShowCharacters, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(m_ui->sceneSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(m_ui->locationExtendedView, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(m_ui->locationSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(m_ui->castShowSpeakingAndNonspeakingScenes, SIGNAL(toggled(bool)), this, SIGNAL(settingsChanged()));
	connect(m_ui->castSortBy, SIGNAL(currentIndexChanged(int)), this, SIGNAL(settingsChanged()));

	connect(m_ui->characterName->selectionModel(), &QItemSelectionModel::selectionChanged, this, &StatisticsSettings::settingsChanged);

	connect(m_ui->storyStructureAnalisysSceneChron, &QCheckBox::toggled, this, &StatisticsSettings::settingsChanged);
	connect(m_ui->storyStructureAnalisysActionChron, &QCheckBox::toggled, this, &StatisticsSettings::settingsChanged);
	connect(m_ui->storyStructureAnalisysDialoguesChron, &QCheckBox::toggled, this, &StatisticsSettings::settingsChanged);
	connect(m_ui->storyStructureAnalisysCharactersCount, &QCheckBox::toggled, this, &StatisticsSettings::settingsChanged);
	connect(m_ui->storyStructureAnalisysDialoguesCount, &QCheckBox::toggled, this, &StatisticsSettings::settingsChanged);

	connect(m_ui->characterActivityNames->selectionModel(), &QItemSelectionModel::selectionChanged, this, &StatisticsSettings::settingsChanged);
}

#include "ScenarioDataEdit.h"
#include "ui_ScenarioDataEdit.h"

using UserInterface::ScenarioDataEdit;


ScenarioDataEdit::ScenarioDataEdit(QWidget* _parent) :
	QWidget(_parent),
	ui(new Ui::ScenarioDataEdit)
{
	ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

ScenarioDataEdit::~ScenarioDataEdit()
{
	delete ui;
}

QWidget* ScenarioDataEdit::toolbar() const
{
	return ui->toolbar;
}

void ScenarioDataEdit::clear()
{
	disconnect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutNameChanged()));
	disconnect(ui->additionalInfo, SIGNAL(textChanged(QString)), this, SLOT(aboutAdditionalInfoChanged()));
	disconnect(ui->genre, SIGNAL(textChanged(QString)), this, SLOT(aboutGenreChanged()));
	disconnect(ui->author, SIGNAL(textChanged(QString)), this, SLOT(aboutAuthorChanged()));
	disconnect(ui->contacts, SIGNAL(textChanged(QString)), this, SLOT(aboutContactsChanged()));
	disconnect(ui->year, SIGNAL(textChanged(QString)), this, SLOT(aboutYearChanged()));
	disconnect(ui->sourceSynopsis, SIGNAL(textChanged()), this, SLOT(aboutSourceSynopsisChanged()));

	m_sourceName.clear();
	m_sourceSourceSynopsis = QTextDocument().toHtml();

	ui->name->clear();
	ui->additionalInfo->clear();
	ui->genre->clear();
	ui->author->clear();
	ui->contacts->clear();
	ui->year->clear();
	ui->sourceSynopsis->clear();
	ui->fromScenarioSynopsis->clear();
	ui->isSourceSynopsis->setChecked(true);
	ui->tabs->setCurrentIndex(0);
	ui->synopsisStack->setCurrentWidget(ui->sourceSynopsisPage);

	ui->name->setFocus();

	connect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutNameChanged()));
	connect(ui->additionalInfo, SIGNAL(textChanged(QString)), this, SLOT(aboutAdditionalInfoChanged()));
	connect(ui->genre, SIGNAL(textChanged(QString)), this, SLOT(aboutGenreChanged()));
	connect(ui->author, SIGNAL(textChanged(QString)), this, SLOT(aboutAuthorChanged()));
	connect(ui->contacts, SIGNAL(textChanged(QString)), this, SLOT(aboutContactsChanged()));
	connect(ui->year, SIGNAL(textChanged(QString)), this, SLOT(aboutYearChanged()));
	connect(ui->sourceSynopsis, SIGNAL(textChanged()), this, SLOT(aboutSourceSynopsisChanged()));
}

QString ScenarioDataEdit::scenarioName() const
{
	return ui->name->text();
}

void ScenarioDataEdit::setScenarioName(const QString& _name)
{
	disconnect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutNameChanged()));

	m_sourceName = _name;
	ui->name->setText(_name);

	connect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutNameChanged()));
}

QString ScenarioDataEdit::scenarioAdditionalInfo() const
{
	return ui->additionalInfo->text();
}

void ScenarioDataEdit::setScenarioAdditionalInfo(const QString& _additionalInfo)
{
	disconnect(ui->additionalInfo, SIGNAL(textChanged(QString)), this, SLOT(aboutAdditionalInfoChanged()));

	m_sourceAdditionalInfo = _additionalInfo;
	ui->additionalInfo->setText(_additionalInfo);

	connect(ui->additionalInfo, SIGNAL(textChanged(QString)), this, SLOT(aboutAdditionalInfoChanged()));
}

QString ScenarioDataEdit::scenarioGenre() const
{
	return ui->genre->text();
}

void ScenarioDataEdit::setScenarioGenre(const QString& _genre)
{
	disconnect(ui->genre, SIGNAL(textChanged(QString)), this, SLOT(aboutGenreChanged()));

	m_sourceGenre = _genre;
	ui->genre->setText(_genre);

	connect(ui->genre, SIGNAL(textChanged(QString)), this, SLOT(aboutGenreChanged()));
}

QString ScenarioDataEdit::scenarioAuthor() const
{
	return ui->author->text();
}

void ScenarioDataEdit::setScenarioAuthor(const QString _author)
{
	disconnect(ui->author, SIGNAL(textChanged(QString)), this, SLOT(aboutAuthorChanged()));

	m_sourceAuthor = _author;
	ui->author->setText(_author);

	connect(ui->author, SIGNAL(textChanged(QString)), this, SLOT(aboutAuthorChanged()));
}

QString ScenarioDataEdit::scenarioContacts() const
{
	return ui->contacts->text();
}

void ScenarioDataEdit::setScenarioContacts(const QString& _contacts)
{
	disconnect(ui->contacts, SIGNAL(textChanged(QString)), this, SLOT(aboutContactsChanged()));

	m_sourceContacts = _contacts;
	ui->contacts->setText(_contacts);

	connect(ui->contacts, SIGNAL(textChanged(QString)), this, SLOT(aboutContactsChanged()));
}

QString ScenarioDataEdit::scenarioYear() const
{
	return ui->year->text();
}

void ScenarioDataEdit::setScenarioYear(const QString& _year)
{
	disconnect(ui->year, SIGNAL(textChanged(QString)), this, SLOT(aboutYearChanged()));

	m_sourceYear = _year;
	ui->year->setText(_year);

	connect(ui->year, SIGNAL(textChanged(QString)), this, SLOT(aboutYearChanged()));
}

QString ScenarioDataEdit::scenarioSynopsis() const
{
	return ui->sourceSynopsis->toHtml();
}

void ScenarioDataEdit::setScenarioSynopsis(const QString& _synopsis)
{
	disconnect(ui->sourceSynopsis, SIGNAL(textChanged()), this, SLOT(aboutSourceSynopsisChanged()));

	//
	// Сформируем значение синопсиса, для корректности последующих сравнений
	//
	QTextDocument synopsisDoc;
	synopsisDoc.setHtml(_synopsis);
	m_sourceSourceSynopsis = synopsisDoc.toHtml();

	ui->sourceSynopsis->setHtml(_synopsis);

	connect(ui->sourceSynopsis, SIGNAL(textChanged()), this, SLOT(aboutSourceSynopsisChanged()));
}

void ScenarioDataEdit::setScenarioSynopsisFromScenes(const QString& _synopsis)
{
	ui->fromScenarioSynopsis->setHtml(_synopsis);
}

void ScenarioDataEdit::aboutNameChanged()
{
	if (ui->name->text() != m_sourceName) {
		emit scenarioNameChanged();
	}
}

void ScenarioDataEdit::aboutAdditionalInfoChanged()
{
	if (ui->additionalInfo->text() != m_sourceAdditionalInfo) {
		emit scenarioAdditionalInfoChanged();
	}
}

void ScenarioDataEdit::aboutGenreChanged()
{
	if (ui->genre->text() != m_sourceGenre) {
		emit scenarioGenreChanged();
	}
}

void ScenarioDataEdit::aboutAuthorChanged()
{
	if (ui->author->text() != m_sourceAuthor) {
		emit scenarioAuthorChanged();
	}
}

void ScenarioDataEdit::aboutContactsChanged()
{
	if (ui->contacts->text() != m_sourceContacts) {
		emit scenarioContactsChanged();
	}
}

void ScenarioDataEdit::aboutYearChanged()
{
	if (ui->year->text() != m_sourceYear) {
		emit scenarioYearChanged();
	}
}

void ScenarioDataEdit::aboutSourceSynopsisChanged()
{
	if (ui->sourceSynopsis->toHtml() != m_sourceSourceSynopsis) {
		emit scenarioSynopsisChanged();
	}
}

void ScenarioDataEdit::aboutCurrentSynopsisChanged()
{
	if (ui->isSourceSynopsis->isChecked()) {
		ui->synopsisStack->setCurrentWidget(ui->sourceSynopsisPage);
	} else {
		ui->synopsisStack->setCurrentWidget(ui->fromScenarioSynopsisPage);
	}
}

void ScenarioDataEdit::initView()
{
	ui->tabs->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	ui->tabs->addTab(tr("Common"));
	ui->tabs->addTab(tr("Synopsis"));

	ui->fromScenarioSynopsis->setReadOnly(true);

	clear();
}

void ScenarioDataEdit::initConnections()
{
	connect(ui->tabs, SIGNAL(currentChanged(int)), ui->mainContainer, SLOT(setCurrentIndex(int)));

	connect(ui->isSourceSynopsis, SIGNAL(toggled(bool)), this, SLOT(aboutCurrentSynopsisChanged()));

	connect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutNameChanged()));
	connect(ui->additionalInfo, SIGNAL(textChanged(QString)), this, SLOT(aboutAdditionalInfoChanged()));
	connect(ui->genre, SIGNAL(textChanged(QString)), this, SLOT(aboutGenreChanged()));
	connect(ui->author, SIGNAL(textChanged(QString)), this, SLOT(aboutAuthorChanged()));
	connect(ui->contacts, SIGNAL(textChanged(QString)), this, SLOT(aboutContactsChanged()));
	connect(ui->year, SIGNAL(textChanged(QString)), this, SLOT(aboutYearChanged()));
	connect(ui->sourceSynopsis, SIGNAL(textChanged()), this, SLOT(aboutSourceSynopsisChanged()));
	connect(ui->isFromScenarioSynopsis, SIGNAL(clicked()), this, SIGNAL(buildSynopsisFromScenes()));
}

void ScenarioDataEdit::initStyleSheet()
{
	ui->tabs->setProperty("inTopPanel", true);

	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
}

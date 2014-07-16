#include "ScenarioDataEditManager.h"

#include <UserInterfaceLayer/Scenario/ScenarioDataEdit/ScenarioDataEdit.h>

#include <3rd_party/Helpers/TextEditHelper.h>

using ManagementLayer::ScenarioDataEditManager;
using UserInterface::ScenarioDataEdit;


ScenarioDataEditManager::ScenarioDataEditManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioDataEdit(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* ScenarioDataEditManager::toolbar() const
{
	return m_view->toolbar();
}

QWidget* ScenarioDataEditManager::view() const
{
	return m_view;
}

void ScenarioDataEditManager::clear()
{
	m_view->clear();
}

QString ScenarioDataEditManager::scenarioName() const
{
	return m_view->scenarioName();
}

void ScenarioDataEditManager::setScenarioName(const QString& _name)
{
	m_view->setScenarioName(_name);
}

QString ScenarioDataEditManager::scenarioAdditionalInfo() const
{
	return m_view->scenarioAdditionalInfo();
}

void ScenarioDataEditManager::setScenarioAdditionalInfo(const QString& _additionalInfo)
{
	m_view->setScenarioAdditionalInfo(_additionalInfo);
}

QString ScenarioDataEditManager::scenarioGenre() const
{
	return m_view->scenarioGenre();
}

void ScenarioDataEditManager::setScenarioGenre(const QString& _genre)
{
	m_view->setScenarioGenre(_genre);
}

QString ScenarioDataEditManager::scenarioAuthor() const
{
	return m_view->scenarioAuthor();
}

void ScenarioDataEditManager::setScenarioAuthor(const QString _author)
{
	m_view->setScenarioAuthor(_author);
}

QString ScenarioDataEditManager::scenarioContacts() const
{
	return m_view->scenarioContacts();
}

void ScenarioDataEditManager::setScenarioContacts(const QString& _contacts)
{
	m_view->setScenarioContacts(_contacts);
}

QString ScenarioDataEditManager::scenarioYear() const
{
	return m_view->scenarioYear();
}

void ScenarioDataEditManager::setScenarioYear(const QString& _year)
{
	m_view->setScenarioYear(_year);
}

QString ScenarioDataEditManager::scenarioSynopsis() const
{
	return TextEditHelper::removeDocumentTags(m_view->scenarioSynopsis());;
}

void ScenarioDataEditManager::setScenarioSynopsis(const QString& _synopsis)
{
	m_view->setScenarioSynopsis(_synopsis);
}

void ScenarioDataEditManager::setScenarioSynopsisFromScenes(const QString& _synopsis)
{
	m_view->setScenarioSynopsisFromScenes(_synopsis);
}

void ScenarioDataEditManager::initView()
{

}

void ScenarioDataEditManager::initConnections()
{
	connect(m_view, SIGNAL(scenarioNameChanged()), this, SIGNAL(scenarioNameChanged()));
	connect(m_view, SIGNAL(scenarioAdditionalInfoChanged()), this, SIGNAL(scenarioAdditionalInfoChanged()));
	connect(m_view, SIGNAL(scenarioGenreChanged()), this, SIGNAL(scenarioGenreChanged()));
	connect(m_view, SIGNAL(scenarioAuthorChanged()), this, SIGNAL(scenarioAuthorChanged()));
	connect(m_view, SIGNAL(scenarioContactsChanged()), this, SIGNAL(scenarioContactsChanged()));
	connect(m_view, SIGNAL(scenarioYearChanged()), this, SIGNAL(scenarioYearChanged()));
	connect(m_view, SIGNAL(scenarioSynopsisChanged()), this, SIGNAL(scenarioSynopsisChanged()));
	connect(m_view, SIGNAL(buildSynopsisFromScenes()), this, SIGNAL(buildSynopsisFromScenes()));
}

#include "CharactersNavigator.h"

#include <QLabel>
#include <QToolButton>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>

using UserInterface::CharactersNavigator;


CharactersNavigator::CharactersNavigator(QWidget *parent) :
	QWidget(parent),
	m_addCharacter(new QToolButton(this)),
	m_removeCharacter(new QToolButton(this)),
	m_refreshCharacters(new QToolButton(this)),
	m_navigator(new QListView(this))
{
	initView();
	initConnections();
}

void CharactersNavigator::setModel(QAbstractItemModel* _model)
{
	m_navigator->setModel(_model);
}

void CharactersNavigator::aboutEditCharacter()
{
	emit editCharacter(selectedUserName());
}

void CharactersNavigator::aboutRemoveCharacter()
{
	emit removeCharacter(selectedUserName());
}

QString CharactersNavigator::selectedUserName() const
{
	QModelIndex currentSelected = m_navigator->selectionModel()->currentIndex();
	QString userName = m_navigator->model()->data(currentSelected).toString();
	return userName;
}

void CharactersNavigator::initView()
{
	QLabel* title = new QLabel(tr("Characters"), this);

	m_addCharacter->setIcon(QIcon(":/Graphics/Icons/Editing/add.png"));
	m_removeCharacter->setIcon(QIcon(":/Graphics/Icons/Editing/delete.png"));
	m_refreshCharacters->setIcon(QIcon(":/Graphics/Icons/Editing/refresh.png"));

	m_navigator->setAlternatingRowColors(true);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(title);
	topLayout->addStretch();
	topLayout->addWidget(m_addCharacter);
	topLayout->addWidget(m_removeCharacter);
	topLayout->addWidget(m_refreshCharacters);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(topLayout);
	layout->addWidget(m_navigator);

	setLayout(layout);
}

void CharactersNavigator::initConnections()
{
	connect(m_addCharacter, SIGNAL(clicked()), this, SIGNAL(addCharacter()));
	connect(m_navigator, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(aboutEditCharacter()));
	connect(m_removeCharacter, SIGNAL(clicked()), this, SLOT(aboutRemoveCharacter()));
	connect(m_refreshCharacters, SIGNAL(clicked()), this, SIGNAL(refreshCharacters()));
}

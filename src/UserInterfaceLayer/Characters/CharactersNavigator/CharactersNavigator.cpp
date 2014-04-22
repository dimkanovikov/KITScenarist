#include "CharactersNavigator.h"

#include "CharactersNavigatorItemDelegate.h"

#include <QLabel>
#include <QToolButton>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>

using UserInterface::CharactersNavigator;
using UserInterface::CharactersNavigatorItemDelegate;


CharactersNavigator::CharactersNavigator(QWidget *parent) :
	QWidget(parent),
	m_addCharacter(new QToolButton(this)),
	m_editCharacter(new QToolButton(this)),
	m_removeCharacter(new QToolButton(this)),
	m_refreshCharacters(new QToolButton(this)),
	m_navigator(new QListView(this)),
	m_navigatorProxyModel(new QSortFilterProxyModel(m_navigator))
{
	initView();
	initConnections();
	initStyleSheet();
}

void CharactersNavigator::setModel(QAbstractItemModel* _model)
{
	m_navigatorProxyModel->setSourceModel(_model);
	m_navigator->setModel(m_navigatorProxyModel);

	//
	// Настраиваем запрос на изменение при изменении текущего элемента
	//
	connect(m_navigator->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(aboutEditCharacter()));
}

void CharactersNavigator::selectFirstCharacter()
{
	selectCharacter(m_navigatorProxyModel->index(0, 0));
}

void CharactersNavigator::selectCharacter(const QString& _name)
{
	const QModelIndex matchStartFrom = m_navigatorProxyModel->index(0, 0);
	QModelIndexList matches = m_navigatorProxyModel->match(matchStartFrom, Qt::DisplayRole, _name);
	if (matches.size() > 0) {
		selectCharacter(matches.first());
	}
}

void CharactersNavigator::selectCharacter(const QModelIndex& _index)
{
	if (m_navigator->model() != 0
		&& m_navigator->model()->rowCount() > 0) {
		//
		// Выделим первого персонажа
		//
		m_navigator->setCurrentIndex(_index);

		//
		// Готовим к изменению
		//
		aboutEditCharacter();
	}
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
	m_title = new QLabel(tr("Characters"), this);
	m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_addCharacter->setIcon(QIcon(":/Graphics/Icons/Editing/add.png"));
	m_editCharacter->setIcon(QIcon(":/Graphics/Icons/Editing/edit.png"));
	m_removeCharacter->setIcon(QIcon(":/Graphics/Icons/Editing/delete.png"));
	m_refreshCharacters->setIcon(QIcon(":/Graphics/Icons/Editing/refresh.png"));

	m_navigatorProxyModel->sort(0);

	m_navigator->setAlternatingRowColors(true);
	m_navigator->setItemDelegate(new CharactersNavigatorItemDelegate(m_navigator));
	m_navigator->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_title);
	topLayout->addWidget(m_addCharacter);
	topLayout->addWidget(m_editCharacter);
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
	connect(m_editCharacter, SIGNAL(clicked()), this, SLOT(aboutEditCharacter()));
	connect(m_removeCharacter, SIGNAL(clicked()), this, SLOT(aboutRemoveCharacter()));
	connect(m_refreshCharacters, SIGNAL(clicked()), this, SIGNAL(refreshCharacters()));
}

void CharactersNavigator::initStyleSheet()
{
	m_title->setProperty("inTopPanel", true);
	m_title->setProperty("topPanelTopBordered", true);

	m_addCharacter->setProperty("inTopPanel", true);
	m_editCharacter->setProperty("inTopPanel", true);
	m_removeCharacter->setProperty("inTopPanel", true);
	m_refreshCharacters->setProperty("inTopPanel", true);

	m_navigator->setProperty("mainContainer", true);
}

#include "CharactersNavigator.h"

#include "CharactersNavigatorItemDelegate.h"

#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QLabel>
#include <QToolButton>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>
#include <QSortFilterProxyModel>

using UserInterface::CharactersNavigator;
using UserInterface::CharactersNavigatorItemDelegate;


CharactersNavigator::CharactersNavigator(QWidget *parent) :
	QWidget(parent),
	m_title(new QLabel(this)),
	m_addCharacter(new FlatButton(this)),
	m_removeCharacter(new FlatButton(this)),
	m_mergeCharacters(new FlatButton(this)),
	m_refreshCharacters(new FlatButton(this)),
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
	connect(m_navigator->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
		this, SLOT(aboutEditCharacter()));
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

void CharactersNavigator::setCommentOnly(bool _isCommentOnly)
{
	m_addCharacter->setVisible(!_isCommentOnly);
	m_removeCharacter->setVisible(!_isCommentOnly);
	m_refreshCharacters->setVisible(!_isCommentOnly);
}

void CharactersNavigator::aboutEditCharacter()
{
	const QStringList selectedCharacters = selectedCharactersNames();
	if (!selectedCharacters.isEmpty()) {
		emit editCharacter(selectedCharacters.first());
	}
}

void CharactersNavigator::aboutRemoveCharacters()
{
	emit removeCharacters(selectedCharactersNames());
}

QStringList CharactersNavigator::selectedCharactersNames() const
{
	QStringList charactersNames;
	foreach (QModelIndex characterIndex, m_navigator->selectionModel()->selectedIndexes()) {
		charactersNames.append(m_navigatorProxyModel->data(characterIndex).toString());
	}
	return charactersNames;
}

void CharactersNavigator::initView()
{
	setFocusProxy(m_navigator);

	m_title = new QLabel(tr("Characters"), this);
	m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_addCharacter->setIcons(QIcon(":/Graphics/Icons/Editing/add.png"));
	m_addCharacter->setShortcut(QKeySequence::New);
	m_addCharacter->setToolTip(
			QString("%1 (%2)")
				.arg(tr("Create New Character"))
				.arg(m_addCharacter->shortcut().toString(QKeySequence::NativeText)));

	m_removeCharacter->setIcons(QIcon(":/Graphics/Icons/Editing/delete.png"));
	m_removeCharacter->setShortcut(QKeySequence("Delete"));
	m_removeCharacter->setToolTip(tr("Remove Selected Character") + " (Del)");

	m_mergeCharacters->setIcons(QIcon(":/Graphics/Icons/Editing/merge.png"));
	m_mergeCharacters->setToolTip(tr("Merge Selected Characters"));
	m_mergeCharacters->hide();

	m_refreshCharacters->setIcons(QIcon(":/Graphics/Icons/Editing/refresh.png"));
	m_refreshCharacters->setToolTip(tr("Find All Characters from Scenario"));

	m_navigatorProxyModel->sort(0);

	m_navigator->setAlternatingRowColors(true);
	m_navigator->setItemDelegate(new CharactersNavigatorItemDelegate(m_navigator));
	m_navigator->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_navigator->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_title);
	topLayout->addWidget(m_addCharacter);
	topLayout->addWidget(m_removeCharacter);
	topLayout->addWidget(m_mergeCharacters);
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
	connect(m_removeCharacter, SIGNAL(clicked()), this, SLOT(aboutRemoveCharacters()));
	QShortcut* removeCharacterShortcut = new QShortcut(QKeySequence("Backspace"), m_navigator);
	connect(removeCharacterShortcut, &QShortcut::activated, m_removeCharacter, &FlatButton::click);
	connect(m_refreshCharacters, SIGNAL(clicked()), this, SIGNAL(refreshCharacters()));
}

void CharactersNavigator::initStyleSheet()
{
	m_title->setProperty("inTopPanel", true);
	m_title->setProperty("topPanelTopBordered", true);

	m_addCharacter->setProperty("inTopPanel", true);
	m_removeCharacter->setProperty("inTopPanel", true);
	m_refreshCharacters->setProperty("inTopPanel", true);
	m_mergeCharacters->setProperty("inTopPanel", true);

	m_navigator->setProperty("mainContainer", true);
}

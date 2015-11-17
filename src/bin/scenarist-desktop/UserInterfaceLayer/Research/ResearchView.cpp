#include "ResearchView.h"
#include "ui_ResearchView.h"

#include <3rd_party/Helpers/TextEditHelper.h>

#include "ResearchNavigatorItemDelegate.h"
#include "ResearchNavigatorProxyStyle.h"

using UserInterface::ResearchView;
using UserInterface::ResearchNavigatorItemDelegate;
using UserInterface::ResearchNavigatorProxyStyle;


ResearchView::ResearchView(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::ResearchView)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

ResearchView::~ResearchView()
{
	delete m_ui;
}

void ResearchView::setResearchModel(QAbstractItemModel* _model)
{
	//
	// Загружаем модель
	//
	m_ui->researchNavigator->setModel(_model);
	if (_model != 0) {
		m_ui->researchNavigator->expand(_model->index(0, 0));
		m_ui->researchNavigator->expand(_model->index(1, 0));
		//
		// Выбираем сценарий
		//
		selectItem(_model->index(0, 0));

		//
		// Настраиваем соединения
		//
		connect(m_ui->researchNavigator->selectionModel(), &QItemSelectionModel::selectionChanged,
				this, &ResearchView::currentResearchChanged);
	}
}

QModelIndex ResearchView::currentResearchIndex() const
{
	QModelIndex currentResearchIndex;
	foreach (QModelIndex researchIndex, m_ui->researchNavigator->selectionModel()->selectedIndexes()) {
		currentResearchIndex = researchIndex;
		break;
	}
	return currentResearchIndex;
}

void ResearchView::selectItem(const QModelIndex& _index)
{
	m_ui->researchNavigator->setCurrentIndex(_index);
}

void ResearchView::editScenario(const QString& _name, const QString& _logline)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->scenarioEdit);
	m_ui->scenarioName->setText(_name);
	m_ui->scenarioLogline->setText(_logline);

	setResearchManageButtonsVisible(false);
}

void ResearchView::editTitlePage(const QString& _name, const QString& _additionalInfo,
	const QString& _genre, const QString& _author, const QString& _contacts, const QString& _year)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->titlePageEdit);
	m_ui->titlePageName->setText(_name);
	m_ui->titlePageAdditionalInfo->setEditText(_additionalInfo);
	m_ui->titlePageGenre->setText(_genre);
	m_ui->titlePageAuthor->setText(_author);
	m_ui->titlePageContacts->setPlainText(_contacts);
	m_ui->titlePageYear->setText(_year);

	setResearchManageButtonsVisible(false);
}

void ResearchView::editSynopsis(const QString& _synopsis)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->synopsisEdit);
	m_ui->synopsisText->setHtml(_synopsis);

	setResearchManageButtonsVisible(false);
}

void ResearchView::editResearchRoot()
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->researchRootEdit);

	//
	// Но кнопку удаления всё-равно скрываем
	//
	setResearchManageButtonsVisible(true);
	m_ui->removeResearchItem->hide();
}

void ResearchView::editText(const QString& _name, const QString& _description)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->textDataEdit);
	m_ui->textName->setText(_name);
	m_ui->textDescription->setHtml(_description);

	setResearchManageButtonsVisible(true);
}

void ResearchView::setCommentOnly(bool _isCommentOnly)
{
	m_ui->addResearchItem->setEnabled(_isCommentOnly);
	m_ui->removeResearchItem->setEnabled(_isCommentOnly);
	m_ui->scenarioName->setReadOnly(_isCommentOnly);
	m_ui->scenarioLogline->setReadOnly(_isCommentOnly);
	m_ui->titlePageName->setReadOnly(_isCommentOnly);
	m_ui->titlePageAdditionalInfo->lineEdit()->setReadOnly(_isCommentOnly);
	m_ui->titlePageAuthor->setReadOnly(_isCommentOnly);
	m_ui->titlePageContacts->setReadOnly(_isCommentOnly);
	m_ui->titlePageGenre->setReadOnly(_isCommentOnly);
	m_ui->titlePageYear->setReadOnly(_isCommentOnly);
	m_ui->synopsisText->setReadOnly(_isCommentOnly);
	m_ui->textName->setReadOnly(_isCommentOnly);
	m_ui->textDescription->setReadOnly(_isCommentOnly);
}

bool ResearchView::eventFilter(QObject* _object, QEvent* _event)
{
	if (_object == m_ui->researchNavigator
		&& _event->type() == QEvent::ContextMenu) {
		QContextMenuEvent* contextMenuEvent = dynamic_cast<QContextMenuEvent*>(_event);
		emit navigatorContextMenuRequested(
			currentResearchIndex(), m_ui->researchNavigator->mapToGlobal(contextMenuEvent->pos()));
	}

	return QWidget::eventFilter(_object, _event);
}

void ResearchView::setResearchManageButtonsVisible(bool _isVisible)
{
	m_ui->addResearchItem->setVisible(_isVisible);
	m_ui->removeResearchItem->setVisible(_isVisible);
}

void ResearchView::currentResearchChanged()
{
	QModelIndex selectedResearchIndex = currentResearchIndex();

	//
	// Испускаем сигнал о намерении изменить элемент разработки, только если он выделен
	//
	if (selectedResearchIndex.isValid()) {
		emit editResearchRequested(selectedResearchIndex);
	}
}

void ResearchView::initView()
{
	m_ui->researchNavigator->setItemDelegate(new ResearchNavigatorItemDelegate(m_ui->researchNavigator));
	m_ui->researchNavigator->setDragDropMode(QAbstractItemView::DragDrop);
	m_ui->researchNavigator->setDragEnabled(true);
	m_ui->researchNavigator->setDropIndicatorShown(true);
	m_ui->researchNavigator->setAlternatingRowColors(true);
	m_ui->researchNavigator->setHeaderHidden(true);
	m_ui->researchNavigator->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_ui->researchNavigator->setSelectionMode(QAbstractItemView::SingleSelection);
	m_ui->researchNavigator->setStyle(new ResearchNavigatorProxyStyle(m_ui->researchNavigator->style()));
	m_ui->researchNavigator->installEventFilter(this);

	m_ui->researchSplitter->setObjectName("researchSplitter");
	m_ui->researchSplitter->setHandleWidth(1);
	m_ui->researchSplitter->setOpaqueResize(false);
	m_ui->researchSplitter->setStretchFactor(1, 1);
}

void ResearchView::initConnections()
{
	//
	// Реакции на нажатие кнопок
	//
	connect(m_ui->addResearchItem, &FlatButton::clicked, [=](){
		emit addResearchRequested(currentResearchIndex());
	});
	connect(m_ui->removeResearchItem, &FlatButton::clicked, [=](){
		emit removeResearchRequested(currentResearchIndex());
	});

	//
	// Внутренние соединения формы
	//
	connect(m_ui->scenarioName, &QLineEdit::textChanged, m_ui->titlePageName, &QLineEdit::setText);
	connect(m_ui->scenarioLogline, &SimpleTextEditor::textChanged, [=](){
		const QString textToSplit = m_ui->scenarioLogline->toPlainText().simplified();
		const int wordsCount = textToSplit.split(QRegExp("([^\\w,^\\\\]|(?=\\\\))+"), QString::SkipEmptyParts).size();
		m_ui->scenarioLoglineWords->setText(QString::number(wordsCount));
	});
	connect(m_ui->titlePageName, &QLineEdit::textChanged, m_ui->scenarioName, &QLineEdit::setText);

	//
	// Сигналы об изменении данных
	//
	// ... сценарий
	//
	connect(m_ui->scenarioName, &QLineEdit::textChanged, this, &ResearchView::scenarioNameChanged);
	connect(m_ui->scenarioLogline, &SimpleTextEditor::textChanged, [=](){
		emit scenarioLoglineChanged(TextEditHelper::removeDocumentTags(m_ui->scenarioLogline->toHtml()));
	});
	//
	// ... титульная страница
	//
	connect(m_ui->titlePageName, &QLineEdit::textChanged, this, &ResearchView::scenarioNameChanged);
	connect(m_ui->titlePageAdditionalInfo, &QComboBox::editTextChanged, this, &ResearchView::titlePageAdditionalInfoChanged);
	connect(m_ui->titlePageGenre, &QLineEdit::textChanged, this, &ResearchView::titlePageGenreChanged);
	connect(m_ui->titlePageAuthor, &QLineEdit::textChanged, this, &ResearchView::titlePageAuthorChanged);
	connect(m_ui->titlePageContacts, &QPlainTextEdit::textChanged, [=](){
		emit titlePageContactsChanged(m_ui->titlePageContacts->toPlainText());
	});
	connect(m_ui->titlePageYear, &QLineEdit::textChanged, this, &ResearchView::titlePageYearChanged);
	//
	// ... синопсис
	//
	connect(m_ui->synopsisText, &SimpleTextEditor::textChanged, [=](){
		emit synopsisTextChanged(TextEditHelper::removeDocumentTags(m_ui->synopsisText->toHtml()));
	});
	//
	// ... текстовый элемент
	//
	connect(m_ui->textName, &QLineEdit::textChanged, this, &ResearchView::textNameChanged);
	connect(m_ui->textDescription, &SimpleTextEditor::textChanged, [=](){
		emit textDescriptionChanged(TextEditHelper::removeDocumentTags(m_ui->textDescription->toHtml()));
	});
}

void ResearchView::initStyleSheet()
{
	m_ui->topNavigatorLabel->setProperty("inTopPanel", true);
	m_ui->topNavigatorLabel->setProperty("topPanelTopBordered", true);
	m_ui->topNavigatorEndLabel->setProperty("inTopPanel", true);
	m_ui->topNavigatorEndLabel->setProperty("topPanelTopBordered", true);
	m_ui->topDataLabel->setProperty("inTopPanel", true);
	m_ui->topDataLabel->setProperty("topPanelTopBordered", true);

	m_ui->addResearchItem->setProperty("inTopPanel", true);
	m_ui->removeResearchItem->setProperty("inTopPanel", true);

	m_ui->researchNavigator->setProperty("mainContainer", true);
	m_ui->researchDataEditsContainer->setProperty("mainContainer", true);
}

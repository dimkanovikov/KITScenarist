#include "ScenarioCardsManager.h"

#include <Domain/Scenario.h>

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>

#include <UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCardsView.h>
#include <UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioSchemeItemDialog.h>

using ManagementLayer::ScenarioCardsManager;
using UserInterface::ScenarioCardsView;
using UserInterface::ScenarioSchemeItemDialog;

ScenarioCardsManager::ScenarioCardsManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioCardsView(_parentWidget)),
	m_addItemDialog(new ScenarioSchemeItemDialog(_parentWidget)),
	m_scenario(nullptr),
	m_model(nullptr)
{
	initConnections();
}

QWidget* ScenarioCardsManager::view() const
{
	return m_view;
}

QString ScenarioCardsManager::save() const
{
	return m_view->save();
}

void ScenarioCardsManager::load(BusinessLogic::ScenarioModel* _model, const QString& _xml)
{
	//
	// Сохраним модель
	//
	if (m_model != _model) {
		m_model = _model;
		connect(m_model, &BusinessLogic::ScenarioModel::rowsInserted, [=] (const QModelIndex& _parent, int _first, int _last) {
			for (int row = _first; row <= _last; ++row) {
				const QModelIndex index = m_model->index(row, 0, _parent);
				BusinessLogic::ScenarioModelItem* item = m_model->itemForIndex(index);
				QModelIndex currentCardIndex = _parent;
				const bool isCardFirstInParent = row == 0;
				if (row > 0) {
					//
					// -1 т.к. нужен предыдущий элемент
					//
					const int itemRow = row - 1;
					if (_parent.isValid()) {
						currentCardIndex = _parent.child(itemRow, 0);
					} else {
						currentCardIndex = m_model->index(itemRow, 0);
					}
				}

				BusinessLogic::ScenarioModelItem* currentCard = m_model->itemForIndex(currentCardIndex);
				m_view->selectCard(currentCard->uuid());
				m_view->addCard(
					item->uuid(),
					item->type(),
					item->title().isEmpty() ? item->header() : item->title(),
					item->description(),
					item->colors(),
					isCardFirstInParent);
			}
		});
		connect(m_model, &BusinessLogic::ScenarioModel::rowsAboutToBeRemoved, [=] (const QModelIndex& _parent, int _first, int _last) {
			for (int row = _last; row >= _first; --row) {
				QModelIndex currentCardIndex = _parent;
				if (_parent.isValid()) {
					currentCardIndex = _parent.child(row, 0);
				} else {
					currentCardIndex = m_model->index(row, 0);
				}
				BusinessLogic::ScenarioModelItem* currentCard = m_model->itemForIndex(currentCardIndex);
				m_view->removeCard(currentCard->uuid());
			}
		});
		connect(m_model, &BusinessLogic::ScenarioModel::dataChanged, [=] (const QModelIndex& _topLeft, const QModelIndex& _bottomRight) {
			for (int row = _topLeft.row(); row <= _bottomRight.row(); ++row) {
				const QModelIndex index = m_model->index(row, 0, _topLeft.parent());
				const BusinessLogic::ScenarioModelItem* item = m_model->itemForIndex(index);
				m_view->updateCard(
					item->uuid(),
					item->type(),
					item->title().isEmpty() ? item->header() : item->title(),
					item->description(),
					item->colors());
			}
		});
	}

	//
	// Загрузим сценарий
	//
	// ... если схема есть, то просто загружаем её
	//
	if (!_xml.isEmpty()) {
		m_view->load(_xml);
	}
	//
	// ... а если схема пуста, сформируем её на основе модели
	//
	else {
		m_view->load(m_model->simpleScheme());
	}
}

void ScenarioCardsManager::clear()
{
	if (m_model != nullptr) {
		m_model->disconnect();
		m_model = nullptr;
	}
	m_view->clear();
}

void ScenarioCardsManager::setCommentOnly(bool _isCommentOnly)
{
	m_view->setCommentOnly(_isCommentOnly);
}

void ScenarioCardsManager::addCard()
{
	m_addItemDialog->showCardPage();
	m_addItemDialog->clear();

	//
	// Если пользователь действительно хочет добавить элемент
	//
	if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
		const int type = m_addItemDialog->cardType();
		const QString title = m_addItemDialog->cardTitle();
		const QString description = m_addItemDialog->cardDescription();

		//
		// Если задан заголовок
		//
		if (!title.isEmpty()) {
			//
			// Определим карточку, после которой нужно добавить элемент
			//
			QModelIndex index;
			const QString selectedItemUuid = m_view->selectedCardUuid();
			if (!selectedItemUuid.isEmpty()) {
				index = m_model->indexForUuid(selectedItemUuid);
			}

			emit addCardRequest(index, type, title, description);
		}
	}
}

void ScenarioCardsManager::editCard(const QString& _uuid, int _cardType, const QString& _title, const QString& _description)
{
	m_addItemDialog->showCardPage();
	m_addItemDialog->clear();
	m_addItemDialog->setCardType((BusinessLogic::ScenarioModelItem::Type)_cardType);
	m_addItemDialog->setCardTitle(_title);
	m_addItemDialog->setCardDescription(_description);

	//
	// Если пользователь действительно хочет добавить элемент
	//
	if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
		const int type = m_addItemDialog->cardType();
		const QString title = m_addItemDialog->cardTitle();
		const QString description = m_addItemDialog->cardDescription();

		//
		// Если задан заголовок
		//
		if (!title.isEmpty()) {
			emit editCardRequest(m_model->indexForUuid(_uuid), type, title, description);
		}
	}
}

void ScenarioCardsManager::removeCard(const QString& _uuid)
{
	emit removeCardRequest(m_model->indexForUuid(_uuid));
}

void ScenarioCardsManager::moveCard(const QString& _parentUuid, const QString& _previousUuid, const QString& _movedUuid)
{
	if (!_movedUuid.isEmpty()) {
		const QModelIndex parentIndex = m_model->indexForUuid(_parentUuid);
		const QModelIndex previousIndex = m_model->indexForUuid(_previousUuid);
		const QModelIndex movedIndex = m_model->indexForUuid(_movedUuid);

		//
		// Синхронизируем перемещение с моделью
		//
		int previousRow = 0;
		if (previousIndex.isValid()) {
			previousRow = previousIndex.row() + 1;
		}
		QMimeData* mime = m_model->mimeData({movedIndex});
		m_model->dropMimeData(mime, Qt::MoveAction, previousRow, 0, parentIndex);
	}
}

void ScenarioCardsManager::addNote()
{
	m_addItemDialog->showNotePage();
	m_addItemDialog->clear();

	//
	// Если пользователь действительно хочет добавить элемент
	//
	if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
		const QString text = m_addItemDialog->noteText();

		//
		// Если задана заметка
		//
		if (!text.isEmpty()) {
			m_view->addNote(text);
		}
	}
}

void ScenarioCardsManager::editNote(const QString& _text)
{
	m_addItemDialog->showNotePage();
	m_addItemDialog->clear();
	m_addItemDialog->setNoteText(_text);

	//
	// Если пользователь действительно хочет добавить элемент
	//
	if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
		const QString text = m_addItemDialog->noteText();

		//
		// Если задана заметка
		//
		if (!text.isEmpty()) {
			m_view->editNote(text);
		}
	}
}

void ScenarioCardsManager::addFlowText()
{

	m_addItemDialog->showFlowPage();
	m_addItemDialog->clear();

	//
	// Если пользователь действительно хочет добавить элемент
	//
	if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
		const QString text = m_addItemDialog->flowText();

		//
		// Если задан текст связи
		//
		if (!text.isEmpty()) {
			m_view->addFlowText(text);
		}
	}
}

void ScenarioCardsManager::editFlowText(const QString& _text)
{
	m_addItemDialog->showFlowPage();
	m_addItemDialog->clear();
	m_addItemDialog->setFlowText(_text);

	//
	// Если пользователь действительно хочет добавить элемент
	//
	if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
		const QString text = m_addItemDialog->noteText();
		m_view->editFlowText(text);
	}
}

void ScenarioCardsManager::initConnections()
{
	connect(m_view, &ScenarioCardsView::addCardClicked, this, &ScenarioCardsManager::addCard);
	connect(m_view, &ScenarioCardsView::editCardRequest, this, &ScenarioCardsManager::editCard);
	connect(m_view, &ScenarioCardsView::removeCardRequest, this, &ScenarioCardsManager::removeCard);
	connect(m_view, &ScenarioCardsView::cardMoved, this, &ScenarioCardsManager::moveCard);

	connect(m_view, &ScenarioCardsView::addNoteClicked, this, &ScenarioCardsManager::addNote);
	connect(m_view, &ScenarioCardsView::editNoteRequest, this, &ScenarioCardsManager::editNote);

	connect(m_view, &ScenarioCardsView::addFlowTextRequest, this, &ScenarioCardsManager::addFlowText);
	connect(m_view, &ScenarioCardsView::editFlowTextRequest, this, &ScenarioCardsManager::editFlowText);

	connect(m_view, &ScenarioCardsView::schemeChanged, this, &ScenarioCardsManager::schemeChanged);
}

#include "ResearchManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/DataStorageLayer/ResearchStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <Domain/Research.h>

#include <BusinessLayer/Research/ResearchModel.h>
#include <BusinessLayer/Research/ResearchModelItem.h>

#include <UserInterfaceLayer/Research/ResearchView.h>
#include <UserInterfaceLayer/Research/ResearchItemDialog.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QWidget>
#include <QSplitter>
#include <QHBoxLayout>

using ManagementLayer::ResearchManager;
using BusinessLogic::ResearchModel;
using BusinessLogic::ResearchModelItem;
using DataStorageLayer::StorageFacade;
using UserInterface::ResearchView;
using UserInterface::ResearchItemDialog;


ResearchManager::ResearchManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ResearchView(_parentWidget)),
	m_dialog(new ResearchItemDialog(m_view)),
	m_model(new ResearchModel(this)),
	m_currentResearch(0)
{
	initView();
	initConnections();
}

QWidget* ResearchManager::view() const
{
	return m_view;
}

void ResearchManager::loadCurrentProject()
{
	//
	// Загрузим модель разработки
	//
	m_model->load(DataStorageLayer::StorageFacade::researchStorage()->all());
	m_view->setResearchModel(m_model);
	editResearch(m_model->index(0, 0));
}

void ResearchManager::closeCurrentProject()
{
	m_model->clear();
	m_view->setResearchModel(0);
}

void ResearchManager::saveResearch()
{
//	foreach (Domain::DomainObject* researchObject,
//			 DataStorageLayer::StorageFacade::researchStorage()->all()->toList()) {
//		Domain::Research* research = dynamic_cast<Domain::Research*>(researchObject);
//		DataStorageLayer::StorageFacade::researchStorage()->updateResearch(research);
//	}
}

void ResearchManager::setCommentOnly(bool _isCommentOnly)
{
//	m_navigatorManager->setCommentOnly(_isCommentOnly);
//	m_dataEditManager->setCommentOnly(_isCommentOnly);
}

void ResearchManager::addResearch(const QModelIndex& _selectedItemIndex)
{
	QString selectedResearchName;
	ResearchModelItem* selectedResearchItem = m_model->itemForIndex(_selectedItemIndex);
	Research* selectedResearch = selectedResearchItem->research();
	if (selectedResearch != 0
		&& (selectedResearch->type() == Research::ResearchRoot
			|| selectedResearch->type() == Research::Folder)) {
		selectedResearchName = selectedResearch->name();
	}

	m_dialog->clear();
	m_dialog->setInsertParent(selectedResearchName);
	if (m_dialog->exec() == QLightBoxDialog::Accepted) {
		//
		// Определим родительский элемент
		//
		ResearchModelItem* parentResearchItem = 0;
		int insertPosition = 0;
		if (selectedResearchItem->research()->type() == Research::ResearchRoot
			|| m_dialog->insertResearchInParent()) {
			parentResearchItem = selectedResearchItem;
			insertPosition = parentResearchItem->childCount();
		} else {
			parentResearchItem = selectedResearchItem->parent();
			insertPosition = _selectedItemIndex.row() + 1;
		}
		Research* parentResearch = parentResearchItem->research();

		//
		// Создаём новый элемент
		//
		Research* newResearch =
			StorageFacade::researchStorage()->storeResearch(
				parentResearch,
				(Research::Type)m_dialog->researchType(),
				m_dialog->researchName(),
				insertPosition);

		//
		// Добавляем его в дерево
		//
		ResearchModelItem* newResearchItem = new ResearchModelItem(newResearch);
		if (selectedResearchItem->research()->type() == Research::ResearchRoot
			|| m_dialog->insertResearchInParent()) {
			m_model->addItem(newResearchItem, parentResearchItem);
		} else {
			m_model->insertItem(newResearchItem, selectedResearchItem);
		}

		//
		// Выбираем его в представлении
		//
		m_view->selectItem(m_model->indexForItem(newResearchItem));
	}
}

void ResearchManager::editResearch(const QModelIndex& _index)
{
	//
	// Определим какой элемент разработки нужно редактировать
	//
	if (ResearchModelItem* researchItem = m_model->itemForIndex(_index)) {
		if (Research* research = researchItem->research()) {
			m_currentResearch = research;

			//
			// В зависимости от типа элемента загрузим необходимые данные в редактор
			//
			switch (research->type()) {
				case Research::Scenario: {
					m_view->editScenario(
						StorageFacade::scenarioDataStorage()->name(),
						StorageFacade::scenarioDataStorage()->logline());
					break;
				}

				case Research::TitlePage: {
					m_view->editTitlePage(
						StorageFacade::scenarioDataStorage()->name(),
						StorageFacade::scenarioDataStorage()->additionalInfo(),
						StorageFacade::scenarioDataStorage()->genre(),
						StorageFacade::scenarioDataStorage()->author(),
						StorageFacade::scenarioDataStorage()->contacts(),
						StorageFacade::scenarioDataStorage()->year());
					break;
				}

				case Research::Synopsis: {
					m_view->editSynopsis(StorageFacade::scenarioDataStorage()->synopsis());
					break;
				}

				case Research::ResearchRoot: {
					m_view->editResearchRoot();
					break;
				}

				case Research::Folder:
				case Research::Text: {
					m_view->editText(research->name(), research->description());
					break;
				}
			}
		}
	}
}

void ResearchManager::removeResearch(const QModelIndex& _index)
{
	//
	// Если пользователь серьёзно намерен удалить разработку
	//
	ResearchModelItem* researchItem = m_model->itemForIndex(_index);
	Research* research = researchItem->research();
	if (QLightBoxMessage::question(m_view, QString::null,
			tr("Are you shure to remove research: <b>%1</b>?").arg(research->name()),
			QDialogButtonBox::Yes | QDialogButtonBox::No)
		== QDialogButtonBox::Yes) {
		//
		// ... удалим
		//
		m_model->removeItem(researchItem);
		DataStorageLayer::StorageFacade::researchStorage()->removeResearch(research);

		//
		// ... реактивируем редактор на родительский, или соседний элемент
		//
		if (_index.row() == 0) {
			m_view->selectItem(_index.parent());
		} else {
			m_view->selectItem(_index.sibling(_index.row() - 1, _index.column()));
		}
	}
}

void ResearchManager::initView()
{


//	m_viewSplitter->setObjectName("researchSplitter");
//	m_viewSplitter->setHandleWidth(1);
//	m_viewSplitter->setStretchFactor(1, 1);
//	m_viewSplitter->setOpaqueResize(false);
//	m_viewSplitter->addWidget(m_navigatorManager->view());
//	m_viewSplitter->addWidget(m_dataEditManager->view());

//	QHBoxLayout* layout = new QHBoxLayout;
//	layout->setContentsMargins(QMargins());
//	layout->setSpacing(0);
//	layout->addWidget(m_viewSplitter);

//	m_view->setLayout(layout);
}

void ResearchManager::initConnections()
{
	connect(m_view, &ResearchView::addResearchRequested, this, &ResearchManager::addResearch);
	connect(m_view, &ResearchView::editResearchRequested, this, &ResearchManager::editResearch);
	connect(m_view, &ResearchView::removeResearchRequested, this, &ResearchManager::removeResearch);

	connect(m_view, &ResearchView::textNameChanged, [=](const QString& _name){
		if (m_currentResearch != 0) {
			m_currentResearch->setName(_name);
			m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
		}
	});
	connect(m_view, &ResearchView::textDescriptionChanged, [=](const QString& _description){
		if (m_currentResearch != 0) {
			m_currentResearch->setDescription(_description);
		}
	});
}

#include "ResearchManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/DataStorageLayer/ResearchStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <Domain/Research.h>
#include <Domain/ScenarioData.h>

#include <BusinessLayer/Research/ResearchModel.h>
#include <BusinessLayer/Research/ResearchModelItem.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <UserInterfaceLayer/Research/ResearchView.h>
#include <UserInterfaceLayer/Research/ResearchItemDialog.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditorWidget.h>

#include <QHBoxLayout>
#include <QMenu>
#include <QSplitter>
#include <QWidget>

using ManagementLayer::ResearchManager;
using BusinessLogic::ResearchModel;
using BusinessLogic::ResearchModelItem;
using DataStorageLayer::StorageFacade;
using UserInterface::ResearchView;
using UserInterface::ResearchItemDialog;

namespace {
	/**
	 * @brief Флаг загрузки проекта
	 */
	static bool g_isProjectLoading = false;
}


ResearchManager::ResearchManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ResearchView(_parentWidget)),
	m_dialog(new ResearchItemDialog(m_view)),
	m_model(new ResearchModel(this)),
	m_currentResearchItem(0),
	m_currentResearch(0)
{
	initView();
	initConnections();

	updateSettings();
}

QWidget* ResearchManager::view() const
{
	return m_view;
}

void ResearchManager::loadCurrentProject()
{
	g_isProjectLoading = true;

	//
	// Загрузим данные сценария
	//
	m_scenarioData.insert(ScenarioData::NAME_KEY, StorageFacade::scenarioDataStorage()->name());
	m_scenarioData.insert(ScenarioData::LOGLINE_KEY, StorageFacade::scenarioDataStorage()->logline());
	m_scenarioData.insert(ScenarioData::ADDITIONAL_INFO_KEY, StorageFacade::scenarioDataStorage()->additionalInfo());
	m_scenarioData.insert(ScenarioData::GENRE_KEY, StorageFacade::scenarioDataStorage()->genre());
	m_scenarioData.insert(ScenarioData::AUTHOR_KEY, StorageFacade::scenarioDataStorage()->author());
	m_scenarioData.insert(ScenarioData::CONTACTS_KEY, StorageFacade::scenarioDataStorage()->contacts());
	m_scenarioData.insert(ScenarioData::YEAR_KEY, StorageFacade::scenarioDataStorage()->year());
	m_scenarioData.insert(ScenarioData::SYNOPSIS_KEY, StorageFacade::scenarioDataStorage()->synopsis());

	//
	// Загрузим модель разработки
	//
    m_model->load(StorageFacade::researchStorage()->all());
	editResearch(m_model->index(0, 0));

	g_isProjectLoading = false;
}

void ResearchManager::closeCurrentProject()
{
    m_scenarioData.clear();
    m_model->clear();
}

void ResearchManager::updateSettings()
{
	//
	// Обновим настройки проверки орфографии
	//
	SimpleTextEditorWidget::enableSpellCheck(
		DataStorageLayer::StorageFacade::settingsStorage()->value(
			"scenario-editor/spell-checking",
			DataStorageLayer::SettingsStorage::ApplicationSettings)
		.toInt(),
		(SpellChecker::Language)DataStorageLayer::StorageFacade::settingsStorage()->value(
			"scenario-editor/spell-checking-language",
			DataStorageLayer::SettingsStorage::ApplicationSettings)
		.toInt());

	//
	// Обновим настройку используемого шаблона для синопсиса
	//
	BusinessLogic::ScenarioTemplate scenarioTemplate = BusinessLogic::ScenarioTemplateFacade::getTemplate();
	m_view->setSynopsisSettings(scenarioTemplate.pageSizeId(), scenarioTemplate.pageMargins(), scenarioTemplate.numberingAlignment());
}

void ResearchManager::saveResearch()
{
	//
	// Сохраняем данные сценария
	//
	StorageFacade::scenarioDataStorage()->setName(m_scenarioData.value(ScenarioData::NAME_KEY));
	StorageFacade::scenarioDataStorage()->setLogline(m_scenarioData.value(ScenarioData::LOGLINE_KEY));
	StorageFacade::scenarioDataStorage()->setAdditionalInfo(m_scenarioData.value(ScenarioData::ADDITIONAL_INFO_KEY));
	StorageFacade::scenarioDataStorage()->setGenre(m_scenarioData.value(ScenarioData::GENRE_KEY));
	StorageFacade::scenarioDataStorage()->setAuthor(m_scenarioData.value(ScenarioData::AUTHOR_KEY));
	StorageFacade::scenarioDataStorage()->setContacts(m_scenarioData.value(ScenarioData::CONTACTS_KEY));
	StorageFacade::scenarioDataStorage()->setYear(m_scenarioData.value(ScenarioData::YEAR_KEY));
	StorageFacade::scenarioDataStorage()->setSynopsis(m_scenarioData.value(ScenarioData::SYNOPSIS_KEY));

	//
	// Сохраняем элементы разработки
	//
	foreach (Domain::DomainObject* researchObject,
			 DataStorageLayer::StorageFacade::researchStorage()->all()->toList()) {
		Domain::Research* research = dynamic_cast<Domain::Research*>(researchObject);
		DataStorageLayer::StorageFacade::researchStorage()->updateResearch(research);
	}
}

void ResearchManager::setCommentOnly(bool _isCommentOnly)
{
	m_view->setCommentOnly(_isCommentOnly);
}

QString ResearchManager::scenarioName() const
{
	return m_scenarioData.value(ScenarioData::NAME_KEY);
}

QMap<QString, QString> ResearchManager::scenarioData() const
{
	return m_scenarioData;
}

void ResearchManager::addResearch(const QModelIndex& _selectedItemIndex)
{
	QString selectedResearchName;
	ResearchModelItem* selectedResearchItem = m_model->itemForIndex(_selectedItemIndex);
	Research* selectedResearch = selectedResearchItem->research();
	if (selectedResearch != 0
		&& selectedResearch->type() == Research::Folder) {
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
		// Сохраняем новый элемент
		//
		StorageFacade::researchStorage()->storeResearch(
			parentResearch,
			(Research::Type)m_dialog->researchType(),
			insertPosition,
			m_dialog->researchName());

		emit researchChanged();
	}
}

void ResearchManager::editResearch(const QModelIndex& _index)
{
	//
	// Определим какой элемент разработки нужно редактировать
	//
	if (ResearchModelItem* researchItem = m_model->itemForIndex(_index)) {
		m_currentResearchItem = researchItem;
		if (Research* research = researchItem->research()) {
			m_currentResearch = research;

			//
			// В зависимости от типа элемента загрузим необходимые данные в редактор
			//
			switch (research->type()) {
				case Research::Scenario: {
					m_view->editScenario(
						m_scenarioData.value(ScenarioData::NAME_KEY),
						m_scenarioData.value(ScenarioData::LOGLINE_KEY));
					break;
				}

				case Research::TitlePage: {
					m_view->editTitlePage(
						m_scenarioData.value(ScenarioData::NAME_KEY),
						m_scenarioData.value(ScenarioData::ADDITIONAL_INFO_KEY),
						m_scenarioData.value(ScenarioData::GENRE_KEY),
						m_scenarioData.value(ScenarioData::AUTHOR_KEY),
						m_scenarioData.value(ScenarioData::CONTACTS_KEY),
						m_scenarioData.value(ScenarioData::YEAR_KEY));
					break;
				}

				case Research::Synopsis: {
					m_view->editSynopsis(m_scenarioData.value(ScenarioData::SYNOPSIS_KEY));
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

				case Research::Url: {
					m_view->editUrl(research->name(), research->url(), research->description());
					break;
				}

				case Research::ImagesGallery: {
					//
					// Формируем список изображений от вложенных элементов
					//
					QList<QPixmap> images;
					if (researchItem->hasChildren()) {
						for (int childIndex = 0; childIndex < researchItem->childCount(); ++childIndex) {
							images.append(researchItem->childAt(childIndex)->research()->image());
						}
					}
					m_view->editImagesGallery(research->name(), images);
					break;
				}

				case Research::Image: {
					m_view->editImage(research->name(), research->image());
					break;
				}

				case Research::MindMap: {
					m_view->editMindMap(research->name(), research->description());
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
			QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::Yes)
		== QDialogButtonBox::Yes) {
		//
		// ... удалим
		//
		DataStorageLayer::StorageFacade::researchStorage()->removeResearch(research);

		//
		// ... реактивируем редактор на родительский, или соседний элемент
		//
		if (_index.row() == 0) {
			m_view->selectItem(_index.parent());
		} else {
			m_view->selectItem(_index.sibling(_index.row() - 1, _index.column()));
		}

		emit researchChanged();
	}
}

void ResearchManager::showNavigatorContextMenu(const QModelIndex& _index, const QPoint& _pos)
{
	ResearchModelItem* researchItem = m_model->itemForIndex(_index);
	bool showAdd = false;
	bool showRemove = false;
	switch (researchItem->research()->type()) {
		case Research::ResearchRoot: {
			showAdd = true;
			break;
		}

		case Research::Folder:
		case Research::Text:
		case Research::Url:
		case Research::ImagesGallery:
		case Research::Image:
		case Research::MindMap: {
			showAdd = true;
			showRemove = true;
			break;
		}

		default: {
			break;
		}
	}

	if (showAdd || showRemove) {
		QMenu menu(m_view);
		QAction* addAction = menu.addAction(tr("Add New"));
		addAction->setVisible(showAdd);
		QAction* removeAction = menu.addAction(tr("Remove"));
		removeAction->setVisible(showRemove);

		QAction* toggledAction = menu.exec(_pos);
		if (toggledAction == addAction) {
			addResearch(_index);
		} else if (toggledAction == removeAction) {
			removeResearch(_index);
		}
	}
}

void ResearchManager::updateScenarioData(const QString& _key, const QString& _value)
{
	//
	// Обновляем данные, если это не загрузка проекта
	//
	if (g_isProjectLoading == false
		&& m_scenarioData.value(_key) != _value) {
		m_scenarioData.insert(_key, _value);
		emit researchChanged();
	}
}

void ResearchManager::initView()
{
    m_view->setResearchModel(m_model);
}

void ResearchManager::initConnections()
{
	connect(m_view, &ResearchView::addResearchRequested, this, &ResearchManager::addResearch);
	connect(m_view, &ResearchView::editResearchRequested, this, &ResearchManager::editResearch);
	connect(m_view, &ResearchView::removeResearchRequested, this, &ResearchManager::removeResearch);
	connect(m_view, &ResearchView::navigatorContextMenuRequested, this, &ResearchManager::showNavigatorContextMenu);
	connect(m_view, &ResearchView::researchItemAdded, this, &ResearchManager::researchChanged);

	connect(m_view, &ResearchView::scenarioNameChanged, [=](const QString& _name){
		updateScenarioData(ScenarioData::NAME_KEY, _name);
	});
	connect(m_view, &ResearchView::scenarioLoglineChanged, [=](const QString& _logline){
		updateScenarioData(ScenarioData::LOGLINE_KEY, _logline);
	});
	connect(m_view, &ResearchView::titlePageAdditionalInfoChanged, [=](const QString& _additionalInfo){
		updateScenarioData(ScenarioData::ADDITIONAL_INFO_KEY, _additionalInfo);
	});
	connect(m_view, &ResearchView::titlePageGenreChanged, [=](const QString& _genre){
		updateScenarioData(ScenarioData::GENRE_KEY, _genre);
	});
	connect(m_view, &ResearchView::titlePageAuthorChanged, [=](const QString& _author){
		updateScenarioData(ScenarioData::AUTHOR_KEY, _author);
	});
	connect(m_view, &ResearchView::titlePageContactsChanged, [=](const QString& _contacts){
		updateScenarioData(ScenarioData::CONTACTS_KEY, _contacts);
	});
	connect(m_view, &ResearchView::titlePageYearChanged, [=](const QString& _year){
		updateScenarioData(ScenarioData::YEAR_KEY, _year);
	});
	connect(m_view, &ResearchView::synopsisTextChanged, [=](const QString& _synopsis){
		updateScenarioData(ScenarioData::SYNOPSIS_KEY, _synopsis);
	});

	connect(m_view, &ResearchView::textNameChanged, [=](const QString& _name){
		if (m_currentResearch != 0
			&& (m_currentResearch->type() == Research::Folder
				|| m_currentResearch->type() == Research::Text)
			&& m_currentResearch->name() != _name) {
			m_currentResearch->setName(_name);
			m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
			emit researchChanged();
		}
	});
	connect(m_view, &ResearchView::textDescriptionChanged, [=](const QString& _description){
		if (m_currentResearch != 0
			&& (m_currentResearch->type() == Research::Folder
				|| m_currentResearch->type() == Research::Text)
			&& m_currentResearch->description() != _description) {
			m_currentResearch->setDescription(_description);
			emit researchChanged();
		}
	});
	//
	// ... ссылка
	//
	connect(m_view, &ResearchView::urlNameChanged, [=](const QString& _name){
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::Url
			&& m_currentResearch->name() != _name) {
			m_currentResearch->setName(_name);
			m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
			emit researchChanged();
		}
	});
	connect(m_view, &ResearchView::urlLinkChanged, [=](const QString& _urlLink){
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::Url
			&& m_currentResearch->url() != _urlLink) {
			m_currentResearch->setUrl(_urlLink);
			m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
			emit researchChanged();
		}
	});
	connect(m_view, &ResearchView::urlContentChanged, [=](const QString& _html){
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::Url
			&& m_currentResearch->description() != _html) {
			m_currentResearch->setDescription(_html);
			emit researchChanged();
		}
	});
	//
	// ... галерея изображений
	//
	connect(m_view, &ResearchView::imagesGalleryNameChanged, [=](const QString& _name){
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::ImagesGallery
			&& m_currentResearch->name() != _name) {
			m_currentResearch->setName(_name);
			m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
			emit researchChanged();
		}
	});
	connect(m_view, &ResearchView::imagesGalleryImageAdded, [=](const QPixmap& _image, int _sortOrder){
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::ImagesGallery) {
			//
			// Создаём новый элемент
			//
			Research* newResearch =
				StorageFacade::researchStorage()->storeResearch(
					m_currentResearch, Research::Image, _sortOrder, tr("Unnamed image"));
			newResearch->setImage(_image);

			emit researchChanged();
		}
	});
	connect(m_view, &ResearchView::imagesGalleryImageRemoved, [=](const QPixmap&, int _sortOrder){
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::ImagesGallery) {
			//
			// Получим ребёнка, которого удаляют
			//
			ResearchModelItem* researchItemToDelete = m_currentResearchItem->childAt(_sortOrder);
			Research* researchToDelete = researchItemToDelete->research();
			//
			// ... удалим
			//
			DataStorageLayer::StorageFacade::researchStorage()->removeResearch(researchToDelete);

			//
			// ... обновляем индексы сортировок для всех последующих изображений
			//
			for (int childIndex = _sortOrder; childIndex < m_currentResearchItem->childCount(); ++childIndex) {
				Research* research = m_currentResearchItem->childAt(childIndex)->research();
				research->setSortOrder(research->sortOrder() - 1);
			}
		}
	});
	//
	// ... изображение
	//
	connect(m_view, &ResearchView::imageNameChanged, [=](const QString& _name){
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::Image
			&& m_currentResearch->name() != _name) {
			m_currentResearch->setName(_name);
			m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
			emit researchChanged();
		}
	});
	connect(m_view, &ResearchView::imagePreviewChanged, [=](const QPixmap& _image){
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::Image) {
			m_currentResearch->setImage(_image);
			emit researchChanged();
		}
	});
	//
	// ... ментальная карта
	//
	connect(m_view, &ResearchView::mindMapNameChanged, [=] (const QString& _name) {
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::MindMap
			&& m_currentResearch->name() != _name) {
			m_currentResearch->setName(_name);
			m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
			emit researchChanged();
		}
	});
	connect(m_view, &ResearchView::mindMapChanged, [=] (const QString& _xml) {
		if (m_currentResearch != 0
			&& m_currentResearch->type() == Research::MindMap) {
			m_currentResearch->setDescription(_xml);
			emit researchChanged();
		}
	});
}

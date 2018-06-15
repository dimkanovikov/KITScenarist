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

#include <3rd_party/Helpers/TextEditHelper.h>
#include <3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditorWidget.h>

#include <QHBoxLayout>
#include <QMenu>
#include <QSplitter>
#include <QWidget>
#include <QWidgetAction>

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

QAbstractItemModel* ResearchManager::model() const
{
    return m_model;
}

void ResearchManager::loadCurrentProject()
{
    g_isProjectLoading = true;

    //
    // Загрузим данные сценария
    //
    loadScenarioData();

    //
    // Загрузим модель разработки
    //
    m_model->load(StorageFacade::researchStorage()->all());

    //
    // Откроем первый элемент на редактирование
    //
    m_view->clear();
    m_view->selectItem(m_model->index(0, 0));
    editResearch(m_model->index(0, 0));

    g_isProjectLoading = false;
}

void ResearchManager::loadScenarioData()
{
    m_scenarioData.insert(ScenarioData::NAME_KEY, StorageFacade::scenarioDataStorage()->name());
    m_scenarioData.insert(ScenarioData::SCENE_NUMBERS_PREFIX_KEY, StorageFacade::scenarioDataStorage()->sceneNumbersPrefix());
    m_scenarioData.insert(ScenarioData::SCENE_START_NUMBER, StorageFacade::scenarioDataStorage()->sceneStartNumber());
    m_scenarioData.insert(ScenarioData::ADDITIONAL_INFO_KEY, StorageFacade::scenarioDataStorage()->additionalInfo());
    m_scenarioData.insert(ScenarioData::GENRE_KEY, StorageFacade::scenarioDataStorage()->genre());
    m_scenarioData.insert(ScenarioData::AUTHOR_KEY, StorageFacade::scenarioDataStorage()->author());
    m_scenarioData.insert(ScenarioData::CONTACTS_KEY, StorageFacade::scenarioDataStorage()->contacts());
    m_scenarioData.insert(ScenarioData::YEAR_KEY, StorageFacade::scenarioDataStorage()->year());
    m_scenarioData.insert(ScenarioData::LOGLINE_KEY, StorageFacade::scenarioDataStorage()->logline());
    m_scenarioData.insert(ScenarioData::SYNOPSIS_KEY, StorageFacade::scenarioDataStorage()->synopsis());

    if (m_view->currentResearchIndex().isValid()) {
        editResearch(m_view->currentResearchIndex());
    }
}

void ResearchManager::loadCurrentProjectSettings(const QString& _projectPath)
{
    //
    // Загрузим состояние дерева
    //
    m_view->setExpandedIndexes(
        DataStorageLayer::StorageFacade::settingsStorage()->variantValue(
            QString("projects/%1/research/expanded-items").arg(_projectPath),
            DataStorageLayer::SettingsStorage::ApplicationSettings)
        .toStringList());
}

void ResearchManager::closeCurrentProject()
{
    m_scenarioData.clear();
    m_model->clear();
}

void ResearchManager::saveCurrentProjectSettings(const QString& _projectPath)
{
    //
    // Сохраним состояние дерева
    //
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
        QString("projects/%1/research/expanded-items").arg(_projectPath),
        m_view->expandedIndexes(),
        DataStorageLayer::SettingsStorage::ApplicationSettings);
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
    const QFont defaultFont(DataStorageLayer::StorageFacade::settingsStorage()->value(
                                "research/default-font/family",
                                DataStorageLayer::SettingsStorage::ApplicationSettings),
                            DataStorageLayer::StorageFacade::settingsStorage()->value(
                                "research/default-font/size",
                                DataStorageLayer::SettingsStorage::ApplicationSettings)
                            .toInt());
    m_view->setTextSettings(scenarioTemplate.pageSizeId(), scenarioTemplate.pageMargins(), scenarioTemplate.numberingAlignment(), defaultFont);
}

void ResearchManager::saveResearch()
{
    //
    // Сохраняем данные сценария
    //
    if (!m_scenarioData.isEmpty()) {
        StorageFacade::scenarioDataStorage()->setName(m_scenarioData.value(ScenarioData::NAME_KEY));
        StorageFacade::scenarioDataStorage()->setSceneNumbersPrefix(m_scenarioData.value(ScenarioData::SCENE_NUMBERS_PREFIX_KEY));
        StorageFacade::scenarioDataStorage()->setSceneStartNumber(m_scenarioData.value(ScenarioData::SCENE_START_NUMBER));
        StorageFacade::scenarioDataStorage()->setAdditionalInfo(m_scenarioData.value(ScenarioData::ADDITIONAL_INFO_KEY));
        StorageFacade::scenarioDataStorage()->setGenre(m_scenarioData.value(ScenarioData::GENRE_KEY));
        StorageFacade::scenarioDataStorage()->setAuthor(m_scenarioData.value(ScenarioData::AUTHOR_KEY));
        StorageFacade::scenarioDataStorage()->setContacts(m_scenarioData.value(ScenarioData::CONTACTS_KEY));
        StorageFacade::scenarioDataStorage()->setYear(m_scenarioData.value(ScenarioData::YEAR_KEY));
        StorageFacade::scenarioDataStorage()->setLogline(m_scenarioData.value(ScenarioData::LOGLINE_KEY));
        StorageFacade::scenarioDataStorage()->setSynopsis(m_scenarioData.value(ScenarioData::SYNOPSIS_KEY));
    }

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

QString ResearchManager::sceneNumbersPrefix() const
{
    return m_scenarioData.value(ScenarioData::SCENE_NUMBERS_PREFIX_KEY);
}

int ResearchManager::sceneStartNumber() const
{
    return m_scenarioData.value(ScenarioData::SCENE_START_NUMBER).toInt();
}

void ResearchManager::setSceneStartNumberEnabled(bool _disabled)
{
    m_view->changeSceneStartNumberEnabled(_disabled);
}

QMap<QString, QString> ResearchManager::scenarioData() const
{
    return m_scenarioData;
}

void ResearchManager::addResearch(const QModelIndex& _selectedItemIndex, int _type)
{
    //
    // Определим выделенный элемент дерева
    //
    QString selectedResearchName;
    ResearchModelItem* selectedResearchItem = m_model->itemForIndex(_selectedItemIndex);
    Research* selectedResearch = selectedResearchItem->research();
    if (selectedResearch != 0
        && (selectedResearch->type() == Research::Character
            || selectedResearch->type() == Research::Location
            || selectedResearch->type() == Research::Folder)) {
        selectedResearchName = selectedResearch->name();
    }

    //
    // Настроим параметры диалога добавления элементов разарботки
    //
    m_dialog->clear();
    m_dialog->setInsertParent(selectedResearchName);
    //
    // ... и скорректируем возможность добавления персонажей и локаций
    //
    switch (selectedResearch->type()) {
        case Research::CharactersRoot:
        case Research::Character: {
            m_dialog->setInsertAllow(true, false);
            break;
        }

        case Research::LocationsRoot:
        case Research::Location: {
            m_dialog->setInsertAllow(false, true);
            break;
        }

        default: {
            m_dialog->setInsertAllow(false, false);
            break;
        }
    }
    if (_type != -1) {
        m_dialog->setResearchType(_type);
    }

    if (m_dialog->exec() == QLightBoxDialog::Accepted) {
        //
        // Определим родительский элемент
        //
        ResearchModelItem* parentResearchItem = 0;
        int insertPosition = 0;
        if (selectedResearchItem->research()->type() == Research::CharactersRoot
            || selectedResearchItem->research()->type() == Research::LocationsRoot
            || selectedResearchItem->research()->type() == Research::ResearchRoot
            || m_dialog->insertResearchInParent()) {
            parentResearchItem = selectedResearchItem;
            insertPosition = parentResearchItem->childCount();
        } else {
            parentResearchItem = selectedResearchItem->parent();
            insertPosition = _selectedItemIndex.row() + 1;
        }
        Research* parentResearch = parentResearchItem->research();

        //
        // Сохраняем новый элемент, если надо
        //
        bool newWasAdded = true;
        switch (m_dialog->researchType()) {
            case Research::Character: {
                if (!StorageFacade::researchStorage()->hasCharacter(m_dialog->researchName())) {
                    StorageFacade::researchStorage()->storeCharacter(m_dialog->researchName(), insertPosition);
                } else {
                    newWasAdded = false;
                }
                break;
            }

            case Research::Location: {
                if (!StorageFacade::researchStorage()->hasLocation(m_dialog->researchName())) {
                    StorageFacade::researchStorage()->storeLocation(m_dialog->researchName(), insertPosition);
                } else {
                    newWasAdded = false;
                }
                break;
            }

            default: {
                StorageFacade::researchStorage()->storeResearch(
                            parentResearch,
                            (Research::Type)m_dialog->researchType(),
                            insertPosition,
                            m_dialog->researchName());
                break;
            }
        }

        //
        // И выделяем добавленный элемент в дереве
        //
        if (newWasAdded) {
            //
            // Обновляем порядок сортировки
            //
            for (int row = 0; row < parentResearchItem->childCount(); ++row) {
                parentResearchItem->childAt(row)->research()->setSortOrder(row);
            }

            QModelIndex indexForSelect;
            if (m_dialog->insertResearchInParent()) {
                indexForSelect = m_model->index(insertPosition, 0, _selectedItemIndex);
            } else {
                if (_selectedItemIndex.parent().isValid()) {
                    indexForSelect = m_model->index(insertPosition, 0, _selectedItemIndex.parent());
                } else {
                    indexForSelect = m_model->index(insertPosition, 0, _selectedItemIndex);
                }
            }
            m_view->selectItem(indexForSelect);

            //
            // Уведомляем подписчиков
            //
            emit researchChanged();
        }
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
            // На время установки данных для изменения, отключаем посыл сигналов, чтобы данные
            // не были восприняты как новые для предыдущего выделенного элемента
            //
            m_view->blockSignals(true);

            //
            // В зависимости от типа элемента загрузим необходимые данные в редактор
            //
            switch (research->type()) {
                case Research::Script: {
                    m_view->editScript(
                        m_scenarioData.value(ScenarioData::NAME_KEY),
                        m_scenarioData.value(ScenarioData::SCENE_NUMBERS_PREFIX_KEY),
                        m_scenarioData.value(ScenarioData::SCENE_START_NUMBER));
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

                case Research::Logline: {
                    m_view->editLogline(m_scenarioData.value(ScenarioData::LOGLINE_KEY));
                    break;
                }

                case Research::Synopsis: {
                    m_view->editSynopsis(m_scenarioData.value(ScenarioData::SYNOPSIS_KEY));
                    break;
                }

                case Research::CharactersRoot: {
                    m_view->editCharactersRoot();
                    break;
                }

                case Research::Character: {
                    auto* researchCharacter = dynamic_cast<Domain::ResearchCharacter*>(research);
                    m_view->editCharacter(researchCharacter->name(), researchCharacter->realName(),
                                          researchCharacter->descriptionText());
                    break;
                }

                case Research::LocationsRoot: {
                    m_view->editLocationsRoot();
                    break;
                }

                case Research::Location: {
                    m_view->editLocation(research->name(), research->description());
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

            m_view->blockSignals(false);
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
            tr("Are you sure to remove research: <b>%1</b>?").arg(research->name()),
            QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::Yes)
        == QDialogButtonBox::Yes) {
        //
        // Удалим и оповестим о смене данных в разработке
        //
        removeResearchItem(researchItem);
        emit researchChanged();
    }
}

void ResearchManager::removeResearchItem(BusinessLogic::ResearchModelItem* _item)
{
    const QModelIndex index = m_model->indexForItem(_item);
    const QModelIndex parentIndex = index.parent();
    //
    // Пробуем выделить следующий за удалённым
    //
    QModelIndex itemForSelect = m_model->index(index.row(), 0, parentIndex);
    //
    // Если нет, то перед удалённым
    //
    if (!itemForSelect.isValid()) {
        itemForSelect = m_model->index(index.row() - 1, 0, parentIndex);
    }
    //
    // Или же родителя
    //
    if (!itemForSelect.isValid()) {
        itemForSelect = parentIndex;
    }
    m_view->selectItem(itemForSelect);

    //
    // Удалим
    //
    DataStorageLayer::StorageFacade::researchStorage()->removeResearch(_item->research());
}

void ResearchManager::refreshResearchSubtree(const QModelIndex& _index)
{
    ResearchModelItem* researchItem = m_model->itemForIndex(_index);
    Research* research = researchItem->research();
    if (research->type() == Research::CharactersRoot) {
        emit refreshCharacters();
    } else if (research->type() == Research::LocationsRoot) {
        emit refreshLocations();
    }
}

void ResearchManager::showNavigatorContextMenu(const QModelIndex& _index, const QPoint& _pos)
{
    if (!_index.isValid()) {
        return;
    }

    ResearchModelItem* researchItem = m_model->itemForIndex(_index);
    bool showColor = false;
    bool showAdd = false;
    bool showRemove = false;
    bool showUpdate = false;
    switch (researchItem->research()->type()) {
        case Research::CharactersRoot:
        case Research::LocationsRoot: {
            showAdd = true;
            showUpdate = true;
            break;
        }

        case Research::ResearchRoot: {
            showAdd = true;
            break;
        }

        case Research::Folder:
        case Research::Text:
        case Research::Url:
        case Research::ImagesGallery:
        case Research::Image:
        case Research::MindMap:
        case Research::Character:
        case Research::Location: {
            showColor = true;
            showAdd = true;
            showRemove = true;
            break;
        }

        default: {
            break;
        }
    }

    if (showColor || showAdd || showRemove) {
        QMenu menu(m_view);
        //
        // Цвет
        //
        QAction* colorAction = menu.addAction(tr("Color"));
        QAction* removeColorAction = nullptr;
        GoogleColorsPane* colorsPane = nullptr;
        {
            colorAction->setVisible(showColor);
            QMenu* colorMenu = new QMenu(m_view);
            removeColorAction = colorMenu->addAction(tr("Clear"));
            colorsPane = new GoogleColorsPane(colorMenu);
            QWidgetAction* wa = new QWidgetAction(colorMenu);
            colorsPane->setCurrentColor(researchItem->color());
            wa->setDefaultWidget(colorsPane);
            colorMenu->addAction(wa);
            colorAction->setMenu(colorMenu);

            connect(colorsPane, &GoogleColorsPane::selected, &menu, &QMenu::close);
        }
        menu.addSeparator();
        //
        // Добавить
        //
        QAction* addAction = menu.addAction(tr("Add New"));
        addAction->setVisible(showAdd);
        //
        // Удалить
        //
        QAction* removeAction = menu.addAction(tr("Remove"));
        removeAction->setVisible(showRemove);
        //
        // Обновить список (зависит от контекста)
        //
        QAction* updateAction =
                menu.addAction(researchItem->research()->type() == Research::CharactersRoot
                               ? tr("Find All Characters from Script")
                               : tr("Find All Locations from Script"));
        updateAction->setVisible(showUpdate);

        QAction* toggledAction = menu.exec(_pos);
        if (toggledAction == addAction) {
            addResearch(_index);
        } else if (toggledAction == removeAction) {
            removeResearch(_index);
        } else if (toggledAction == updateAction) {
            refreshResearchSubtree(_index);
        } else if (toggledAction == removeColorAction) {
            researchItem->research()->setColor(QColor());
        } else {
            if (colorsPane->currentColor().isValid()) {
                researchItem->research()->setColor(colorsPane->currentColor());
            }
        }
    }
}

void ResearchManager::updateScenarioData(const QString& _key, const QString& _value)
{
    //
    // Обновляем данные, если это не загрузка проекта
    //
    if (g_isProjectLoading == false
        && m_scenarioData.contains(_key)
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
    connect(m_model, &ResearchModel::itemMoved, this, [this] (const QModelIndex& _index) {
        m_view->selectItem(_index);
        emit researchChanged();
    });

    connect(m_view, &ResearchView::addResearchRequested, this, &ResearchManager::addResearch);
    connect(m_view, &ResearchView::editResearchRequested, this, &ResearchManager::editResearch);
    connect(m_view, &ResearchView::removeResearchRequested, this, &ResearchManager::removeResearch);
    connect(m_view, &ResearchView::refeshResearchSubtreeRequested, this, &ResearchManager::refreshResearchSubtree);
    connect(m_view, &ResearchView::navigatorContextMenuRequested, this, &ResearchManager::showNavigatorContextMenu);

    connect(m_view, &ResearchView::scriptNameChanged, this, [this] (const QString& _name){
        emit scriptNameChanged(_name);
        updateScenarioData(ScenarioData::NAME_KEY, _name);
    });
    connect(m_view, &ResearchView::scriptSceneNumbersPrefixChanged, this, [this] (const QString& _sceneNumbersPrefix) {
        emit sceneNumbersPrefixChanged(_sceneNumbersPrefix);
        updateScenarioData(ScenarioData::SCENE_NUMBERS_PREFIX_KEY, _sceneNumbersPrefix);
    });
    connect(m_view, &ResearchView::scriptSceneStartNumber, this, [this] (const QString& _startSceneNumber) {
        int startNumber = _startSceneNumber.toInt();
        emit sceneStartNumberChanged(startNumber);
        updateScenarioData(ScenarioData::SCENE_START_NUMBER, _startSceneNumber);
    });
    connect(m_view, &ResearchView::titlePageAdditionalInfoChanged, this, [this] (const QString& _additionalInfo){
        updateScenarioData(ScenarioData::ADDITIONAL_INFO_KEY, _additionalInfo);
    });
    connect(m_view, &ResearchView::titlePageGenreChanged, this, [this] (const QString& _genre){
        updateScenarioData(ScenarioData::GENRE_KEY, _genre);
    });
    connect(m_view, &ResearchView::titlePageAuthorChanged, this, [this] (const QString& _author){
        updateScenarioData(ScenarioData::AUTHOR_KEY, _author);
    });
    connect(m_view, &ResearchView::titlePageContactsChanged, this, [this] (const QString& _contacts){
        updateScenarioData(ScenarioData::CONTACTS_KEY, _contacts);
    });
    connect(m_view, &ResearchView::titlePageYearChanged, this, [this] (const QString& _year){
        updateScenarioData(ScenarioData::YEAR_KEY, _year);
    });
    connect(m_view, &ResearchView::loglineTextChanged, this, [this] (const QString& _logline){
        updateScenarioData(ScenarioData::LOGLINE_KEY, _logline);
    });
    connect(m_view, &ResearchView::synopsisTextChanged, this, [this] (const QString& _synopsis){
        updateScenarioData(ScenarioData::SYNOPSIS_KEY, _synopsis);
    });

    //
    // ... персонаж
    //
    connect(m_view, &ResearchView::characterNameChanged, this, [this] (const QString& _name) {
        const QString newName = TextEditHelper::smartToUpper(_name);
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Character
            && m_currentResearch->name() != newName) {
            const QString oldName = m_currentResearch->name();
            //
            // Если такой персонаж уже существует, переместим описание к нему, а текущего удалим
            //
            if (StorageFacade::researchStorage()->hasCharacter(_name)) {
                Research* mainCharacter = StorageFacade::researchStorage()->character(_name);
                mainCharacter->addDescription(m_currentResearch->description());
                removeResearchItem(m_currentResearchItem);
            }
            //
            // В противном случае просто меняем имя персонажа
            //
            else {
                m_currentResearch->setName(newName);
                m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
            }
            emit researchChanged();
            emit characterNameChanged(oldName, newName);
        }
    });
    connect(m_view, &ResearchView::characterRealNameChanged, this, [this] (const QString& _name) {
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Character) {
            auto* researchCharacter = dynamic_cast<ResearchCharacter*>(m_currentResearch);
            researchCharacter->setRealName(_name);
            emit researchChanged();
        }
    });
    connect(m_view, &ResearchView::characterDescriptionChanged, this, [this] (const QString& _description) {
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Character) {
            auto* researchCharacter = dynamic_cast<ResearchCharacter*>(m_currentResearch);
            researchCharacter->setDescriptionText(_description);
            emit researchChanged();
        }
    });
    //
    // ... локация
    //
    connect(m_view, &ResearchView::locationNameChanged, this, [this] (const QString& _name) {
        const QString newName = TextEditHelper::smartToUpper(_name);
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Location
            && m_currentResearch->name() != newName) {
            const QString oldName = m_currentResearch->name();
            //
            // Если такая локация уже существует, переместим описание к ней, а текущую удалим
            //
            if (StorageFacade::researchStorage()->hasLocation(_name)) {
                Research* mainLocation = StorageFacade::researchStorage()->location(_name);
                mainLocation->addDescription(m_currentResearch->description());
                removeResearchItem(m_currentResearchItem);
            }
            //
            // В противном случае просто меняем название
            //
            else {
                m_currentResearch->setName(newName);
                m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
            }
            emit researchChanged();
            emit locationNameChanged(oldName, newName);
        }
    });
    connect(m_view, &ResearchView::locationDescriptionChanged, this, [this] (const QString& _description) {
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Location
            && m_currentResearch->description() != _description) {
            m_currentResearch->setDescription(_description);
            emit researchChanged();
        }
    });
    //
    // ... текстовый элемент
    //
    connect(m_view, &ResearchView::textNameChanged, this, [this] (const QString& _name){
        if (m_currentResearch != nullptr
            && (m_currentResearch->type() == Research::Folder
                || m_currentResearch->type() == Research::Text)
            && m_currentResearch->name() != _name) {
            m_currentResearch->setName(_name);
            m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
            emit researchChanged();
        }
    });
    connect(m_view, &ResearchView::textDescriptionChanged, this, [this] (const QString& _description){
        if (m_currentResearch != nullptr
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
    connect(m_view, &ResearchView::urlNameChanged, this, [this] (const QString& _name){
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Url
            && m_currentResearch->name() != _name) {
            m_currentResearch->setName(_name);
            m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
            emit researchChanged();
        }
    });
    connect(m_view, &ResearchView::urlLinkChanged, this, [this] (const QString& _urlLink){
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Url
            && m_currentResearch->url() != _urlLink) {
            m_currentResearch->setUrl(_urlLink);
            m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
            emit researchChanged();
        }
    });
    connect(m_view, &ResearchView::urlContentChanged, this, [this] (const QString& _html){
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Url
            && m_currentResearch->description() != _html) {
            m_currentResearch->setDescription(_html);
            emit researchChanged();
        }
    });
    //
    // ... галерея изображений
    //
    connect(m_view, &ResearchView::imagesGalleryNameChanged, this, [this] (const QString& _name){
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::ImagesGallery
            && m_currentResearch->name() != _name) {
            m_currentResearch->setName(_name);
            m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
            emit researchChanged();
        }
    });
    connect(m_view, &ResearchView::imagesGalleryImageAdded, this, [this] (const QPixmap& _image, int _sortOrder){
        if (m_currentResearch != nullptr
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
    connect(m_view, &ResearchView::imagesGalleryImageRemoved, this, [this] (const QPixmap&, int _sortOrder){
        if (m_currentResearch != nullptr
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
    connect(m_view, &ResearchView::imageNameChanged, this, [this] (const QString& _name){
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Image
            && m_currentResearch->name() != _name) {
            m_currentResearch->setName(_name);
            m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
            emit researchChanged();
        }
    });
    connect(m_view, &ResearchView::imagePreviewChanged, this, [this] (const QPixmap& _image){
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::Image) {
            m_currentResearch->setImage(_image);
            emit researchChanged();
        }
    });
    //
    // ... ментальная карта
    //
    connect(m_view, &ResearchView::mindMapNameChanged, this, [this] (const QString& _name) {
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::MindMap
            && m_currentResearch->name() != _name) {
            m_currentResearch->setName(_name);
            m_model->updateItem(m_model->itemForIndex(m_view->currentResearchIndex()));
            emit researchChanged();
        }
    });
    connect(m_view, &ResearchView::mindMapChanged, this, [this] (const QString& _xml) {
        if (m_currentResearch != nullptr
            && m_currentResearch->type() == Research::MindMap) {
            m_currentResearch->setDescription(_xml);
            emit researchChanged();
        }
    });
}

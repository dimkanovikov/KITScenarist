#include "ScenarioNavigator.h"


#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>
#include <BusinessLayer/ScenarioDocument/ScenarioModelItem.h>

#include <3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorItemDelegate.h>
#include <UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorProxyStyle.h>

#include <QAction>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidgetAction>

using UserInterface::ScenarioNavigator;
using UserInterface::ScenarioNavigatorItemDelegate;
using UserInterface::ScenarioNavigatorProxyStyle;


ScenarioNavigator::ScenarioNavigator(QWidget *_parent) :
    QWidget(_parent),
    m_draftTitle(new QLabel(this)),
    m_scenesCountTitle(new QLabel(this)),
    m_scenesCount(new QLabel(this)),
    m_addItem(new FlatButton(this)),
    m_removeItem(new FlatButton(this)),
    m_middleTitle(new QLabel(this)),
    m_showAdditionalPanels(new FlatButton(this)),
    m_showDraft(new QAction(m_showAdditionalPanels)),
    m_showSceneDescription(new QAction(m_showAdditionalPanels)),
    m_showScriptBookmarks(new QAction(m_showAdditionalPanels)),
    m_showScriptDictionaries(new QAction(m_showAdditionalPanels)),
    m_navigationTree(new QTreeView(this)),
    m_navigationTreeDelegate(new ScenarioNavigatorItemDelegate(this))
{
    initView();
    initConnections();
    initStyleSheet();
}

void ScenarioNavigator::setScenesCount(int _scenesCount)
{
    m_scenesCount->setText(QString::number(_scenesCount));
}

void ScenarioNavigator::setModel(QAbstractItemModel* _model)
{
    m_navigationTree->setModel(_model);

    //
    // Обновляем размер колонки со списком сцен, чтобы не появлялось горизонтальной прокрутки
    //
    m_navigationTree->resizeColumnToContents(0);
}

void ScenarioNavigator::setCurrentIndex(const QModelIndex& _index)
{
    if (!m_navigationTree->selectionModel()->selectedIndexes().contains(_index)) {
        m_navigationTree->clearSelection();
        m_navigationTree->setCurrentIndex(_index);
    }
}

void ScenarioNavigator::setShowSceneNumber(bool _show)
{
    m_navigationTreeDelegate->setShowSceneNumber(_show);
}

void ScenarioNavigator::setShowSceneTitle(bool _show)
{
    m_navigationTreeDelegate->setShowSceneTitle(_show);
}

void ScenarioNavigator::setShowSceneDescription(bool _show)
{
    m_navigationTreeDelegate->setShowSceneDescription(_show);
}

void ScenarioNavigator::setSceneDescriptionIsSceneText(bool _isSceneText)
{
    m_navigationTreeDelegate->setSceneDescriptionIsSceneText(_isSceneText);
}

void ScenarioNavigator::setSceneDescriptionHeight(int _height)
{
    m_navigationTreeDelegate->setSceneDescriptionHeight(_height);
}

void ScenarioNavigator::setSceneNumbersPrefix(const QString& _prefix)
{
    m_navigationTreeDelegate->setSceneNumbersPrefix(_prefix);
}

void ScenarioNavigator::resetView()
{
    m_navigationTree->setItemDelegate(0);
    delete m_navigationTreeDelegate;
    m_navigationTreeDelegate = 0;

    m_navigationTreeDelegate = new ScenarioNavigatorItemDelegate(this);
    m_navigationTree->setItemDelegate(m_navigationTreeDelegate);
}

void ScenarioNavigator::setIsDraft(bool _isDraft)
{
    m_draftTitle->setVisible(_isDraft);
    m_scenesCountTitle->setVisible(!_isDraft);
    m_scenesCount->setVisible(!_isDraft);
    m_middleTitle->setVisible(!_isDraft);
    m_showAdditionalPanels->setVisible(!_isDraft);

    m_draftTitle->setProperty("topPanelTopBordered", !_isDraft);
    m_scenesCountTitle->setProperty("topPanelTopBordered", !_isDraft);
    m_scenesCount->setProperty("topPanelTopBordered", !_isDraft);
    m_addItem->setProperty("topPanelTopBordered", !_isDraft);
    m_removeItem->setProperty("topPanelTopBordered", !_isDraft);
    m_middleTitle->setProperty("topPanelTopBordered", !_isDraft);
}

void ScenarioNavigator::clearSelection()
{
    m_navigationTree->selectionModel()->clearSelection();
}

void ScenarioNavigator::setDraftVisible(bool _visible)
{
    m_showDraft->setChecked(_visible);
}

void ScenarioNavigator::setSceneDescriptionVisible(bool _visible)
{
    m_showSceneDescription->setChecked(_visible);
}

void ScenarioNavigator::setScriptBookmarksVisible(bool _visible)
{
    m_showScriptBookmarks->setChecked(_visible);
}

void ScenarioNavigator::setScriptDictionariesVisible(bool _visible)
{
    m_showScriptDictionaries->setChecked(_visible);
}

void ScenarioNavigator::setCommentOnly(bool _isCommentOnly)
{
    m_addItem->setVisible(!_isCommentOnly);
    m_removeItem->setVisible(!_isCommentOnly);
    m_navigationTree->setDragDropMode(_isCommentOnly ? QAbstractItemView::NoDragDrop : QAbstractItemView::DragDrop);
    m_navigationTree->setSelectionMode(_isCommentOnly ? QAbstractItemView::SingleSelection : QAbstractItemView::ContiguousSelection);
    setContextMenuPolicy(_isCommentOnly ? Qt::PreventContextMenu : Qt::CustomContextMenu);
}

bool ScenarioNavigator::eventFilter(QObject* _watched, QEvent* _event)
{
    bool isEventFiltered = false;

    if (_watched == m_navigationTree
        && _event->type() == QEvent::KeyPress) {
        //
        // Отлавливаем необходимую комбинацию клавиш
        //
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(_event);
        QString keyCharacter = keyEvent->text();
        if (keyEvent->modifiers().testFlag(Qt::ControlModifier)
            && ((Qt::Key)keyEvent->key() == Qt::Key_Z
                || keyCharacter == "z"
                || keyCharacter == QString::fromUtf8("я"))) {
            if (keyEvent->modifiers().testFlag(Qt::ShiftModifier)) {
                emit redoRequest();
            } else {
                emit undoRequest();
            }
            isEventFiltered = true;
        }
    }
    //
    // В противном случае выполняется стандартная обработка
    //
    else {
        isEventFiltered = QWidget::eventFilter(_watched, _event);
    }

    return isEventFiltered;
}

void ScenarioNavigator::aboutAddItem()
{
    QModelIndex currentItemIndex = m_navigationTree->currentIndex();

    //
    // Тут не проверяется состояние текущего элемента, если индекс не указывает ни на
    // какой элемент, то новый элемент создаётся в самом конце сценария, а если указывает,
    // то новый элемент создаётся после него
    //
    emit addItem(currentItemIndex);
}

void ScenarioNavigator::aboutRemoveItem()
{
    QModelIndexList indexes = m_navigationTree->selectionModel()->selectedIndexes();
    if (!indexes.isEmpty()) {
        emit removeItems(indexes);
    }
}

void ScenarioNavigator::aboutContextMenuRequested(const QPoint& _pos)
{
    QMenu* menu = new QMenu(this);
    const bool isSelectedJustOneItem = m_navigationTree->selectionModel()->selectedIndexes().size() == 1;

    //
    // Преобразование элемента
    //
    QAction* convertToScene = nullptr;
    QAction* convertToFolder = nullptr;
    //
    // ... создаём только если выделен один элемент
    //
    if (isSelectedJustOneItem) {
        convertToScene = menu->addAction(tr("Convert to scene"));
        convertToScene->setData(BusinessLogic::ScenarioModelItem::Scene);
        convertToFolder = menu->addAction(tr("Convert to folder"));
        convertToFolder->setData(BusinessLogic::ScenarioModelItem::Folder);
        switch (m_navigationTree->currentIndex().data(BusinessLogic::ScenarioModel::TypeIndex).toInt()) {
            case BusinessLogic::ScenarioModelItem::Scene: {
                convertToScene->setVisible(false);
                break;
            }

            case BusinessLogic::ScenarioModelItem::Folder: {
                convertToFolder->setVisible(false);
                break;
            }
        }
    }

    //
    // Цвета
    //
    if (isSelectedJustOneItem) {
        menu->addSeparator();
    }
    //
    // ... очистить цвета
    //
    QAction* clearColors = menu->addAction(tr("Clear all colors"));
    //
    // ... добавить
    //
    QList<GoogleColorsPane*> colorsPanesList;
    QString colorsNames;
    QStringList colorsTags;
    //
    // ... только если выделен один элемент
    //
    if (isSelectedJustOneItem) {
        colorsNames = m_navigationTree->currentIndex().data(BusinessLogic::ScenarioModel::ColorIndex).toString();
        int colorIndex = 1;
        //
        // ... добавляем каждый цвет
        //
        for (const QString& colorName : colorsNames.split(";")) {
            QAction* color = menu->addAction(colorIndex == 1 ? tr("Main color")
                                                             : tr("Additional color %1").arg(colorIndex));
            QMenu* colorMenu = new QMenu(this);
            QAction* removeColor = colorMenu->addAction(tr("Clear"));
            removeColor->setData(QString("removeColor:%1").arg(colorIndex));
            QWidgetAction* wa = new QWidgetAction(colorMenu);
            GoogleColorsPane* colorsPane = new GoogleColorsPane(colorMenu);
            colorsPane->setCurrentColor(QColor(colorName.left(7)));
            colorsTags.append(colorName.mid(7));
            wa->setDefaultWidget(colorsPane);
            colorMenu->addAction(wa);
            color->setMenu(colorMenu);

            connect(colorsPane, &GoogleColorsPane::selected, menu, &QMenu::close);

            colorsPanesList.append(colorsPane);

            ++colorIndex;
        }
    }
    //
    // ... пункт для нового цвета
    //
    {
        QAction* color = menu->addAction(tr("Add color"));
        QMenu* colorMenu = new QMenu(this);
        QWidgetAction* wa = new QWidgetAction(colorMenu);
        GoogleColorsPane* colorsPane = new GoogleColorsPane(colorMenu);
        wa->setDefaultWidget(colorsPane);
        colorMenu->addAction(wa);
        color->setMenu(colorMenu);

        connect(colorsPane, &GoogleColorsPane::selected, menu, &QMenu::close);

        colorsPanesList.append(colorsPane);
    }


    //
    // Остальное
    //
    menu->addSeparator();
    QAction* addNew = menu->addAction(tr("Create After"));
    QAction* remove = menu->addAction(tr("Remove"));


    //
    // Выводим меню
    //
    QAction* toggled = menu->exec(mapToGlobal(_pos));
    if (toggled != nullptr) {
        if (toggled->data().toString().startsWith("removeColor")) {
            //
            // Удаляем выбранный цвет из списка и обновляемся
            //
            const int removeColorIndex = toggled->data().toString().split(":").last().toInt();
            QString newColorsNames;
            int colorIndex = 1;
            for (const QString& colorName : colorsNames.split(";")) {
                if (colorIndex != removeColorIndex) {
                    if (!newColorsNames.isEmpty() || colorIndex == 2) {
                        newColorsNames.append(";");
                    }
                    newColorsNames.append(colorName);
                }

                ++colorIndex;
            }

            emit setItemsColors(m_navigationTree->selectionModel()->selectedIndexes(), newColorsNames);
        } else if (toggled == clearColors) {
            emit setItemsColors(m_navigationTree->selectionModel()->selectedIndexes(), QString());
        } else if (toggled == addNew) {
            aboutAddItem();
        } else if (toggled == remove) {
            aboutRemoveItem();
        } else if (toggled == convertToScene
                   || toggled == convertToFolder) {
            const QModelIndex currentItemIndex = m_navigationTree->currentIndex();
            emit changeItemTypeRequested(currentItemIndex, toggled->data().toInt());
        }
    } else {
        //
        // Добавляем новый цвет и обновляемся
        //
        QString newColorsNames;
        int colorIndex = 0;
        for (GoogleColorsPane* colorsPane : colorsPanesList) {
            if (colorsPane->currentColor().isValid()) {
                if (!newColorsNames.isEmpty() || colorIndex == 1) {
                    newColorsNames.append(";");
                }
                newColorsNames.append(colorsPane->currentColor().name() + colorsTags.value(colorIndex));
            }
            ++colorIndex;
        }
        emit setItemsColors(m_navigationTree->selectionModel()->selectedIndexes(), newColorsNames);
    }

    menu->deleteLater();
}

void ScenarioNavigator::initView()
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    m_draftTitle->setText(tr("Draft:"));

    m_scenesCountTitle->setText(tr("Scenes:"));
    m_scenesCount->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_scenesCount->setToolTip(tr("Scenes Count"));
    m_scenesCount->setAlignment((QLocale().textDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignVCenter);

    m_addItem->setIcons(QIcon(":/Graphics/Iconset/plus.svg"));
    m_addItem->setToolTip(tr("Add Scenario Item After Selected"));

    m_removeItem->setIcons(QIcon(":/Graphics/Iconset/delete.svg"));
    m_removeItem->setToolTip(tr("Remove Selected Scenario Item"));

    m_middleTitle->setFixedWidth(1);

    m_showAdditionalPanels->setIcons(QIcon(":/Graphics/Iconset/view-agenda.svg"));
    m_showAdditionalPanels->setToolTip(tr("Show/hide additional panels"));
    m_showAdditionalPanels->setPopupMode(QToolButton::MenuButtonPopup);
    m_showAdditionalPanels->addAction(m_showDraft);
    m_showAdditionalPanels->addAction(m_showSceneDescription);
    m_showAdditionalPanels->addAction(m_showScriptBookmarks);
    m_showAdditionalPanels->addAction(m_showScriptDictionaries);

    m_showDraft->setObjectName("navigatorShowDraft");
    m_showDraft->setText(tr("Draft"));
    m_showDraft->setCheckable(true);
    m_showDraft->setToolTip(tr("Show/hide draft"));

    m_showSceneDescription->setObjectName("navigatorShowNote");
    m_showSceneDescription->setText(tr("Scene description"));
    m_showSceneDescription->setToolTip(tr("Show/hide scene note"));
    m_showSceneDescription->setCheckable(true);

    m_showScriptBookmarks->setObjectName("navigatorShowScriptBookmars");
    m_showScriptBookmarks->setText(tr("Script bookmarks"));
    m_showScriptBookmarks->setToolTip(tr("Show/hide script bookmarks"));
    m_showScriptBookmarks->setCheckable(true);

    m_showScriptDictionaries->setObjectName("navigatorShowScriptDictionaries");
    m_showScriptDictionaries->setText(tr("Script dictionaries"));
    m_showScriptDictionaries->setToolTip(tr("Show/hide script dictionaries"));
    m_showScriptDictionaries->setCheckable(true);

    m_navigationTree->setItemDelegate(m_navigationTreeDelegate);
    m_navigationTree->setDragDropMode(QAbstractItemView::DragDrop);
    m_navigationTree->setDragEnabled(true);
    m_navigationTree->setDropIndicatorShown(true);
    m_navigationTree->setHeaderHidden(true);
    m_navigationTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_navigationTree->setSelectionMode(QAbstractItemView::ContiguousSelection);
    m_navigationTree->setStyle(new ScenarioNavigatorProxyStyle(m_navigationTree->style()));
    m_navigationTree->installEventFilter(this);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(QMargins());
    topLayout->setSpacing(0);
    topLayout->addWidget(m_draftTitle);
    topLayout->addWidget(m_scenesCountTitle);
    topLayout->addWidget(m_scenesCount);
    topLayout->addWidget(m_addItem);
    topLayout->addWidget(m_removeItem);
    topLayout->addWidget(m_middleTitle);
    topLayout->addWidget(m_showAdditionalPanels);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addLayout(topLayout);
    layout->addWidget(m_navigationTree);

    setLayout(layout);
}

void ScenarioNavigator::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(aboutContextMenuRequested(QPoint)));

    connect(m_addItem, SIGNAL(clicked()), this, SLOT(aboutAddItem()));
    connect(m_removeItem, SIGNAL(clicked()), this, SLOT(aboutRemoveItem()));
    connect(m_showAdditionalPanels, &FlatButton::clicked, m_showAdditionalPanels, &FlatButton::showMenu);
    connect(m_showDraft, &QAction::toggled, this, &ScenarioNavigator::draftVisibleChanged);
    connect(m_showSceneDescription, &QAction::toggled, this, &ScenarioNavigator::sceneDescriptionVisibleChanged);
    connect(m_showScriptBookmarks, &QAction::toggled, this, &ScenarioNavigator::scriptBookmarksVisibleChanged);
    connect(m_showScriptDictionaries, &QAction::toggled, this, &ScenarioNavigator::scriptDictionariesVisibleChanged);
    connect(m_navigationTree, SIGNAL(clicked(QModelIndex)), this, SIGNAL(sceneChoosed(QModelIndex)));
}

void ScenarioNavigator::initStyleSheet()
{
    m_draftTitle->setProperty("inTopPanel", true);
    m_draftTitle->setProperty("topPanelTopBordered", true);

    m_scenesCountTitle->setProperty("inTopPanel", true);
    m_scenesCountTitle->setProperty("topPanelTopBordered", true);

    m_scenesCount->setProperty("inTopPanel", true);
    m_scenesCount->setProperty("topPanelTopBordered", true);

    m_addItem->setProperty("inTopPanel", true);
    m_removeItem->setProperty("inTopPanel", true);

    m_middleTitle->setProperty("inTopPanel", true);
    m_middleTitle->setProperty("topPanelTopBordered", true);
    m_middleTitle->setProperty("topPanelRightBordered", true);

    m_showAdditionalPanels->setProperty("inTopPanel", true);
    m_showAdditionalPanels->setProperty("hasMenu", true);

    m_navigationTree->setProperty("mainContainer", true);
}

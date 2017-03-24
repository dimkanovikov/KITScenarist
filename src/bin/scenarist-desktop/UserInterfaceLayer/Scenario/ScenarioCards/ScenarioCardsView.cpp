#include "ScenarioCardsView.h"
#include "CardsResizer.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/ShortcutHelper.h>

#include <3rd_party/Widgets/CardsEdit/CardsView.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QFileInfo>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QShortcut>
#include <QStandardPaths>
#include <QVariant>
#include <QWidgetAction>

using UserInterface::ScenarioCardsView;
using UserInterface::CardsResizer;

namespace {
    /**
     * @brief Получить путь к последней используемой папке
     */
    static QString cardsFolderPath() {
        QString cardsFolderPath =
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "cards/save-folder",
                    DataStorageLayer::SettingsStorage::ApplicationSettings);
        if (cardsFolderPath.isEmpty()) {
            cardsFolderPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        }
        return cardsFolderPath;
    }

    /**
     * @brief Получить путь к сохраняемому файлу
     */
    static QString cardsFilePath(const QString& _fileName) {
        QString filePath = cardsFolderPath() + QDir::separator() + _fileName;
        return QDir::toNativeSeparators(filePath);
    }

    /**
     * @brief Сохранить путь к последней используемой папке
     */
    static void saveCardsFolderPath(const QString& _path) {
        DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                    "cards/save-folder",
                    QFileInfo(_path).absoluteDir().absolutePath(),
                    DataStorageLayer::SettingsStorage::ApplicationSettings);
    }
}


ScenarioCardsView::ScenarioCardsView(bool _isDraft, QWidget* _parent) :
    QWidget(_parent),
    m_cards(new CardsView(_parent)),
    m_active(new FlatButton(_parent)),
    m_addCard(new FlatButton(_parent)),
    m_removeCard(new FlatButton(_parent)),
    m_sort(new FlatButton(_parent)),
    m_resizer(new CardsResizer(m_sort)),
    m_save(new FlatButton(_parent)),
    m_print(new FlatButton(_parent)),
    m_fullscreen(new FlatButton(_parent)),
    m_toolbarSpacer(new QLabel(_parent))
{
    initView(_isDraft);
    initConnections();
    initShortcuts();
    initStyleSheet();
}

void ScenarioCardsView::clear()
{
    m_cards->clear();
}

void ScenarioCardsView::undo()
{
    m_cards->undo();
}

void ScenarioCardsView::redo()
{
    m_cards->redo();
}

void ScenarioCardsView::setUseCorkboardBackground(bool _use)
{
    m_cards->setUseCorkboardBackground(_use);
}

void ScenarioCardsView::setBackgroundColor(const QColor& _color)
{
    m_cards->setBackgroundColor(_color);
}

void ScenarioCardsView::load(const QString& _xml)
{
    if (m_cards->load(_xml)) {
        m_cards->saveChanges(true);
    } else {
        emit schemeNotLoaded();
    }
}

QString ScenarioCardsView::save() const
{
    return m_cards->save();
}

void ScenarioCardsView::saveToImage()
{
    const QString saveFileName = ::cardsFilePath(tr("Cards.png"));
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save cards"), saveFileName, tr("PNG files (*.png)"));
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".png")) {
            fileName.append(".png");
        }
        m_cards->saveToImage(fileName);

        ::saveCardsFolderPath(fileName);
    }
}

void ScenarioCardsView::saveChanges(bool _hasChangesInText)
{
    m_cards->saveChanges(_hasChangesInText);
}

void ScenarioCardsView::insertCard(const QString& _uuid, bool _isFolder, int _number,
    const QString& _title, const QString& _description, const QString& _stamp,
    const QString& _colors, bool _isEmbedded, const QString& _previousCardUuid)
{
    m_cards->insertCard(_uuid, _isFolder, _number, _title, _description, _stamp, _colors, _isEmbedded, m_newCardPosition, _previousCardUuid);
}

void ScenarioCardsView::updateCard(const QString& _uuid, bool _isFolder, int _number,
    const QString& _title, const QString& _description, const QString& _stamp,
    const QString& _colors, bool _isEmbedded, bool _isAct)
{
    m_cards->updateItem(_uuid, _isFolder, _number, _title, _description, _stamp, _colors, _isEmbedded, _isAct);
}

void ScenarioCardsView::removeCard(const QString& _uuid)
{
    m_cards->removeItem(_uuid);
}

QString ScenarioCardsView::lastItemUuid() const
{
    return m_cards->lastItemUuid();
}

QString ScenarioCardsView::beforeNewItemUuid() const
{
    return m_cards->beforeNewItemUuid(m_newCardPosition);
}

void ScenarioCardsView::setCommentOnly(bool _isCommentOnly)
{
    m_addCard->setEnabled(!_isCommentOnly);
    m_sort->setEnabled(!_isCommentOnly);
}

void ScenarioCardsView::resortCards()
{
    //
    // Вычисляем размер карточки
    //
    qreal widthDivider = 1;
    qreal heightDivider = 1;
    switch (m_resizer->cardRatio()) {
        case 0: heightDivider = 0.2; break; // 5x1
        case 1: heightDivider = 0.4; break; // 5x2
        case 2: heightDivider = 0.6; break; // 5x3
        case 3: heightDivider = 0.8; break; // 5x4
        case 4: break; // 5x5
        case 5: widthDivider = 0.8; break; // 4x5
        case 6: widthDivider = 0.6; break; // 3x5
        case 7: widthDivider = 0.4; break; // 2x5
        case 8: widthDivider = 0.2; break; // 1x5
    }
    const qreal cardWidth = (qreal)m_resizer->cardSize() * widthDivider;
    const qreal cardHeight = (qreal)m_resizer->cardSize() * heightDivider;
    const QSizeF cardSize(cardWidth, cardHeight);
    m_cards->setCardsSize(cardSize);

    //
    // Расстояние между карточками
    //
    m_cards->setCardsDistance(m_resizer->distance());

    //
    // Количество карточек в строке
    //
    m_cards->setCardsInRow(m_resizer->cardsInRow());
}

void ScenarioCardsView::initView(bool _isDraft)
{
    m_active->hide();
    if (_isDraft) {
        m_active->setText(tr("Draft"));
    } else {
        m_cards->setCanAddActs(true);
        m_cards->setFixedMode(true);

        m_active->setText(tr("Script"));
    }
    m_active->setCheckable(true);
    m_active->setToolButtonStyle(Qt::ToolButtonTextOnly);

    m_addCard->setIcons(QIcon(":/Graphics/Icons/Editing/add.png"));
    m_addCard->setToolTip(tr("Add new card"));

    m_removeCard->setIcons(QIcon(":/Graphics/Icons/Editing/delete.png"));
    m_removeCard->setToolTip(tr("Remove selected card"));

    m_sort->setIcons(QIcon(":/Graphics/Icons/Cards/table.png"));
    m_sort->setToolTip(tr("Sort cards"));
    //
    // Настроим меню кнопки упорядочивания карточек по сетке
    //
    m_sort->setPopupMode(QToolButton::MenuButtonPopup);
    {
        QMenu* menu = new QMenu(m_sort);
        QWidgetAction* wa = new QWidgetAction(menu);
        wa->setDefaultWidget(m_resizer);
        menu->addAction(wa);
        m_sort->setMenu(menu);
    }

    m_save->setIcons(QIcon(":/Graphics/Icons/Editing/download.png"));
    m_save->setToolTip(tr("Save cards to file"));

    m_print->setIcons(QIcon(":/Graphics/Icons/printer.png"));
    m_print->setToolTip(tr("Print cards"));

    m_fullscreen->setIcons(QIcon(":/Graphics/Icons/Editing/fullscreen.png"),
        QIcon(":/Graphics/Icons/Editing/fullscreen_active.png"));
    m_fullscreen->setToolTip(tr("On/off fullscreen mode (F5)"));
    m_fullscreen->setCheckable(true);

    m_toolbarSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QWidget* toolbar = new QWidget(this);
    QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(QMargins());
    toolbarLayout->setSpacing(0);
    toolbarLayout->addWidget(m_active);
    toolbarLayout->addWidget(m_addCard);
    toolbarLayout->addWidget(m_removeCard);
    toolbarLayout->addWidget(m_sort);
    toolbarLayout->addWidget(m_save);
    toolbarLayout->addWidget(m_print);
    toolbarLayout->addWidget(m_toolbarSpacer);
    toolbarLayout->addWidget(m_fullscreen);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(toolbar);
    layout->addWidget(m_cards);

    setLayout(layout);
}

void ScenarioCardsView::initConnections()
{
    connect(m_cards, &CardsView::cardsChanged, this, &ScenarioCardsView::cardsChanged);

    connect(m_addCard, &FlatButton::clicked, [=] {
        m_newCardPosition = QPointF();
        emit addCardClicked();
    });
    connect(m_cards, &CardsView::cardAddRequest, [=] (const QPointF& _position) {
        m_newCardPosition = _position;
        emit addCardClicked();
    });
    connect(m_cards, &CardsView::cardAddCopyRequest,
        [=] (bool _isFolder, const QString& _title, const QString& _description, const QString& _stamp,
             const QString& _colors, const QPointF& _position) {
        m_newCardPosition = _position;
        emit addCopyCardRequest(_isFolder, _title, _description, _stamp, _colors);
    });

    connect(m_removeCard, &FlatButton::clicked, m_cards, &CardsView::removeSelectedItem);

    connect(m_cards, &CardsView::actChangeRequest, this, &ScenarioCardsView::editCardRequest);
    connect(m_cards, &CardsView::cardChangeRequest, this, &ScenarioCardsView::editCardRequest);

    connect(m_cards, &CardsView::actRemoveRequest, this, &ScenarioCardsView::removeCardRequest);
    connect(m_cards, &CardsView::cardRemoveRequest, this, &ScenarioCardsView::removeCardRequest);

    connect(m_cards, &CardsView::cardMoved, this, &ScenarioCardsView::cardMoved);
    connect(m_cards, &CardsView::cardMovedToGroup, this, &ScenarioCardsView::cardMovedToGroup);

    connect(m_cards, &CardsView::cardColorsChanged, this, &ScenarioCardsView::cardColorsChanged);
    connect(m_cards, &CardsView::cardTypeChanged, this, &ScenarioCardsView::cardTypeChanged);

    connect(m_sort, &FlatButton::clicked, m_sort, &FlatButton::showMenu);
    connect(m_resizer, &CardsResizer::parametersChanged, this, &ScenarioCardsView::resortCards);

    connect(m_save, &FlatButton::clicked, this, &ScenarioCardsView::saveToImage);

    connect(m_print, &FlatButton::clicked, this, &ScenarioCardsView::printRequest);

    connect(m_fullscreen, &FlatButton::clicked, this, &ScenarioCardsView::fullscreenRequest);
}

void ScenarioCardsView::initShortcuts()
{
    QShortcut* undo = new QShortcut(QKeySequence::Undo, this);
    undo->setContext(Qt::WidgetWithChildrenShortcut);
    connect(undo, &QShortcut::activated, [=] {
        emit undoRequest();
        return;

        //
        // Если отмену необходимо синхронизировать с текстом, уведомляем об этом
        //
        if (m_cards->needSyncUndo()) {
            emit undoRequest();
        }
        //
        // А если синхронизировать не нужно, просто отменяем последнее изменение
        //
        else {
            m_cards->saveChanges(false);
            m_cards->undo();
        }
    });

    QShortcut* redo = new QShortcut(QKeySequence::Redo, this);
    redo->setContext(Qt::WidgetWithChildrenShortcut);
    connect(redo, &QShortcut::activated, [=] {
        emit redoRequest();
        return;

        //
        // Если повтор необходимо синхронизировать с текстом, уведомляем об этом
        //
        if (m_cards->needSyncRedo()) {
            emit redoRequest();
        }
        //
        // А если синхронизировать не нужно, просто повторяем последнее изменение
        //
        else {
            m_cards->redo();
        }
    });

    QShortcut* add = new QShortcut(Qt::Key_New, this);
    add->setContext(Qt::WidgetWithChildrenShortcut);
    connect(add, &QShortcut::activated, this, &ScenarioCardsView::addCardClicked);

    QShortcut* remove = new QShortcut(Qt::Key_Delete, this);
    remove->setContext(Qt::WidgetWithChildrenShortcut);
    connect(remove, &QShortcut::activated, m_cards, &CardsView::removeSelectedItem);
    QShortcut* backspace = new QShortcut(Qt::Key_Backspace, this);
    backspace->setContext(Qt::WidgetWithChildrenShortcut);
    connect(backspace, &QShortcut::activated, m_cards, &CardsView::removeSelectedItem);

    QShortcut* fullscreen = new QShortcut(Qt::Key_F5, this);
    fullscreen->setContext(Qt::WidgetWithChildrenShortcut);
    connect(fullscreen, &QShortcut::activated, m_fullscreen, &FlatButton::click);
}

void ScenarioCardsView::initStyleSheet()
{
    m_active->setProperty("inTopPanel", true);
    m_addCard->setProperty("inTopPanel", true);
    m_removeCard->setProperty("inTopPanel", true);
    m_sort->setProperty("inTopPanel", true);
    m_sort->setProperty("hasMenu", true);
    m_save->setProperty("inTopPanel", true);
    m_print->setProperty("inTopPanel", true);
    m_fullscreen->setProperty("inTopPanel", true);

    m_toolbarSpacer->setProperty("inTopPanel", true);
    m_toolbarSpacer->setProperty("topPanelTopBordered", true);
    m_toolbarSpacer->setProperty("topPanelRightBordered", true);

    m_cards->setProperty("mainContainer", true);
}

#include "ScenarioCardsManager.h"

#include <Domain/Scenario.h>

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>
#include <BusinessLayer/ScenarioDocument/ScenarioModelItem.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <UserInterfaceLayer/Scenario/ScenarioCards/PrintCardsDialog.h>
#include <UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCardsView.h>
#include <UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioSchemeItemDialog.h>

#include <3rd_party/Helpers/TextUtils.h>

#include <QApplication>
#include <QPainter>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QScopedPointer>

using ManagementLayer::ScenarioCardsManager;
using UserInterface::PrintCardsDialog;
using UserInterface::ScenarioCardsView;
using UserInterface::ScenarioSchemeItemDialog;

namespace {
    const bool IS_DRAFT = true;
    const bool IS_SCRIPT = false;
}


ScenarioCardsManager::ScenarioCardsManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new ScenarioCardsView(IS_SCRIPT, _parentWidget)),
    m_addItemDialog(new ScenarioSchemeItemDialog(_parentWidget)),
    m_printDialog(new PrintCardsDialog(_parentWidget))
{
    initConnections();
    reloadSettings();
}

QWidget* ScenarioCardsManager::view() const
{
    return m_view;
}

void ScenarioCardsManager::reloadSettings()
{
    m_view->setUseCorkboardBackground(
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "cards/use-corkboard",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt()
                );

    const bool useDarkTheme =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/use-dark-theme",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    const QString colorSuffix = useDarkTheme ? "-dark" : "";
    m_view->setBackgroundColor(
                QColor(
                    DataStorageLayer::StorageFacade::settingsStorage()->value(
                        "cards/background-color" + colorSuffix,
                        DataStorageLayer::SettingsStorage::ApplicationSettings)
                    )
                );
}

QString ScenarioCardsManager::save() const
{
    return m_view->save();
}

void ScenarioCardsManager::saveChanges(bool _hasChangesInText)
{
    m_view->saveChanges(_hasChangesInText);
}

void ScenarioCardsManager::load(BusinessLogic::ScenarioModel* _model, const QString& _xml)
{
    //
    // Сохраним модель
    //
    if (m_model != _model) {
        m_model = _model;
        connect(m_model, &BusinessLogic::ScenarioModel::rowsInserted, [=] (const QModelIndex& _parent, int _first, int _last) {
            //
            // Пробегаем каждый добавленный элемент
            //
            for (int row = _first; row <= _last; ++row) {
                const QModelIndex index = m_model->index(row, 0, _parent);
                BusinessLogic::ScenarioModelItem* item = m_model->itemForIndex(index);

                //
                // ... пропускаем сцены если их уровень вложенности больше второго
                //
                if (item->hasParent()
                    && item->parent()->hasParent()
                    && item->parent()->parent()->hasParent()) {
                    continue;
                }

                //
                // ... определим предыдущий элемент
                //
                QModelIndex currentCardIndex = _parent;
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

                //
                // ... вставляем
                //
                const bool isEmbedded =
                        item->hasParent()
                        && item->parent()->type() != BusinessLogic::ScenarioModelItem::Scenario;
                m_view->insertCard(
                    item->uuid(),
                    item->type() == BusinessLogic::ScenarioModelItem::Folder,
                    item->sceneNumber(),
                    item->title().isEmpty() ? item->header().toUpper() : item->title().toUpper(),
                    item->description().isEmpty() ? item->fullText() : item->description(),
                    QString::null,
                    item->colors(),
                    isEmbedded,
                    currentCard->uuid());
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
                //
                // Если тип карточки определить не удалось, удаляем её
                //
                if (item->type() == BusinessLogic::ScenarioModelItem::Undefined) {
                    m_view->removeCard(item->uuid());
                }
                //
                // А если тип нормальный, то обновляем данные о карточке
                //
                else {
                    const bool isAct =
                            item->type() == BusinessLogic::ScenarioModelItem::Folder
                            && item->hasParent()
                            && item->parent()->type() == BusinessLogic::ScenarioModelItem::Scenario;
                    const bool isEmbedded =
                            item->hasParent()
                            && item->parent()->type() != BusinessLogic::ScenarioModelItem::Scenario;
                    m_view->updateCard(
                        item->uuid(),
                        item->type() == BusinessLogic::ScenarioModelItem::Folder,
                        item->sceneNumber(),
                        item->title().isEmpty() ? item->header().toUpper() : item->title().toUpper(),
                        item->description().isEmpty() ? item->fullText() : item->description(),
                        QString::null,
                        item->colors(),
                        isEmbedded,
                        isAct);
                }
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

void ScenarioCardsManager::undo()
{
    m_view->undo();
}

void ScenarioCardsManager::redo()
{
    m_view->redo();
}

void ScenarioCardsManager::setCommentOnly(bool _isCommentOnly)
{
    m_view->setCommentOnly(_isCommentOnly);
}

void ScenarioCardsManager::addCard()
{
    m_addItemDialog->setWindowTitle(tr("Add card"));
    m_addItemDialog->clear();

    //
    // Если пользователь действительно хочет добавить элемент
    //
    if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
        const int type = m_addItemDialog->cardType();
        const QString title = m_addItemDialog->cardTitle();
        const QString color = m_addItemDialog->cardColor();
        const QString description = m_addItemDialog->cardDescription();

        //
        // Если задан заголовок
        //
        if (!title.isEmpty()) {
            //
            // Определим карточку, после которой нужно добавить элемент
            //
            QModelIndex index;
            const QString lastItemUuid = m_view->lastItemUuid();
            if (!lastItemUuid.isEmpty()) {
                index = m_model->indexForUuid(lastItemUuid);
            }

            //
            // Если добавляется акт, то нужно взять корневой индекс
            //
            if (type == BusinessLogic::ScenarioModelItem::Folder
                && index.parent().isValid()) {
                index = index.parent();
            }

            emit addCardRequest(index, type, title, QColor(color), description);
        }
    }
}

void ScenarioCardsManager::editCard(const QString& _uuid)
{
    m_addItemDialog->setWindowTitle(tr("Edit card"));
    m_addItemDialog->clear();

    const QModelIndex indexForUpdate = m_model->indexForUuid(_uuid);
    const auto* itemForUpdate = m_model->itemForIndex(indexForUpdate);
    m_addItemDialog->setCardType(itemForUpdate->type());
    m_addItemDialog->setCardTitle(itemForUpdate->title());
    const QString firstColor = itemForUpdate->colors().split(";").first();
    m_addItemDialog->setCardColor(firstColor);
    m_addItemDialog->setCardDescription(itemForUpdate->description());

    //
    // Если пользователь действительно хочет изменить элемент
    //
    if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
        const int type = m_addItemDialog->cardType();
        const QString title = m_addItemDialog->cardTitle();
        QString colors = itemForUpdate->colors();
        if (firstColor != m_addItemDialog->cardColor()) {
            colors.replace(firstColor, m_addItemDialog->cardColor());
        }
        const QString description = m_addItemDialog->cardDescription();

        //
        // Испускаем запрос на изменение
        //
        emit updateCardRequest(indexForUpdate, type, title, colors, description);
    }
}

void ScenarioCardsManager::removeCard(const QString& _uuid)
{
    emit removeCardRequest(m_model->indexForUuid(_uuid));
}

void ScenarioCardsManager::moveCard(const QString& _cardId, const QString& _actId, const QString& _previousCardId)
{
    if (!_cardId.isEmpty()) {
        const QModelIndex parentIndex = m_model->indexForUuid(_actId);
        const QModelIndex previousIndex = m_model->indexForUuid(_previousCardId);
        const QModelIndex movedIndex = m_model->indexForUuid(_cardId);

        //
        // Синхронизируем перемещение с моделью
        //
        int previousRow = 0;
        if (previousIndex.isValid()) {
            previousRow = previousIndex.row() + 1;
        }
        QMimeData* mime = m_model->mimeData({ movedIndex });
        m_model->dropMimeData(mime, Qt::MoveAction, previousRow, 0, parentIndex);
    }
}

void ScenarioCardsManager::moveCardToGroup(const QString& _cardId, const QString& _groupId)
{
    if (!_cardId.isEmpty()) {
        const QModelIndex parentIndex = m_model->indexForUuid(_groupId);
        const QModelIndex movedIndex = m_model->indexForUuid(_cardId);

        //
        // Синхронизируем перемещение с моделью
        //
        int previousRow = -1;
        QMimeData* mime = m_model->mimeData({ movedIndex });
        m_model->dropMimeData(mime, Qt::MoveAction, previousRow, 0, parentIndex);
    }
}

void ScenarioCardsManager::changeCardColors(const QString& _uuid, const QString& _colors)
{
    if (!_uuid.isEmpty()) {
        const QModelIndex index = m_model->indexForUuid(_uuid);
        emit cardColorsChanged(index, _colors);
    }
}

void ScenarioCardsManager::changeCardType(const QString& _uuid, bool _isFolder)
{
    if (!_uuid.isEmpty()) {
        const QModelIndex index = m_model->indexForUuid(_uuid);
        int mappedType = BusinessLogic::ScenarioModelItem::Scene;
        if (_isFolder) {
            mappedType = BusinessLogic::ScenarioModelItem::Folder;
        }
        emit cardTypeChanged(index, mappedType);
    }
}

void ScenarioCardsManager::print()
{
    //
    // Настроим принтер
    //
    QPrinter* printer = new QPrinter;
    printer->setPageOrientation(m_printDialog->isPortrait() ? QPageLayout::Portrait : QPageLayout::Landscape);

    //
    // Настроим диалог предпросмотра
    //
    QPrintPreviewDialog printDialog(printer, m_view);
    printDialog.setWindowState(Qt::WindowMaximized);
    connect(&printDialog, &QPrintPreviewDialog::paintRequested, this, &ScenarioCardsManager::printCards);

    //
    // Запускаем предпросмотр
    //
    printDialog.exec();

    //
    // Очищаем память
    //
    delete printer;
}

void ScenarioCardsManager::printCards(QPrinter* _printer)
{
    //
    // Покажем прогресс
    //
    m_printDialog->setEnabled(false);
    m_printDialog->setProgressValue(0);
    m_printDialog->showProgress(0, 0);

    //
    // Подготовим список карточек для печати
    //
    QMap<int, BusinessLogic::ScenarioModelItem*> items;
    {
        QVector<QModelIndex> parents { QModelIndex() };
        do {
            const int parentsSize = parents.size();
            for (int parentIndexRow = parentsSize - 1; parentIndexRow >= 0; --parentIndexRow) {
                //
                // Обновляем значение прогресса
                //
                QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

                //
                // Собираем элементы
                //
                const QModelIndex& parentIndex = parents.at(parentIndexRow);
                for (int row = 0; row < m_model->rowCount(parentIndex); ++row) {
                    const QModelIndex index = m_model->index(row, 0, parentIndex);
                    parents.append(index);

                    auto item = m_model->itemForIndex(index);
                    items.insert(item->position(), item);
                }
                parents.remove(parentIndexRow);
            }
        } while (!parents.isEmpty());
    }

    //
    // Покажем прогресс
    //
    int progress = 0;
    m_printDialog->setProgressValue(progress);
    m_printDialog->showProgress(0, items.size());

    //
    // Печатаем карточки
    //
    QPainter painter(_printer);

    //
    // Проходим все карточки по-очереди
    //
    bool isFirst = true;
    const int firstCardIndex = 0;
    const int cardsCount = m_printDialog->cardsCount();
    const qreal sideMargin = _printer->pageRect().x();
    int currentCardIndex = firstCardIndex;
    qreal lastY = 0;
    for (const int& key : items.keys()) {
        //
        // Обновляем значение прогресса
        //
        m_printDialog->setProgressValue(++progress);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        const BusinessLogic::ScenarioModelItem* item = items[key];
        const QRectF pageRect = _printer->paperRect().adjusted(0, 0, -2 * sideMargin, -2 * sideMargin);

        //
        // Если надо, переходим на новую страницу и рисуем линии разреза
        //
        if (currentCardIndex == firstCardIndex) {
            if (isFirst) {
                isFirst = false;
            } else {
                _printer->newPage();
            }

            painter.setClipRect(pageRect);
            painter.save();
            painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
            switch (cardsCount) {
                default:
                case 1: {
                    //
                    // Нет линий разреза
                    //
                    break;
                }

                case 2: {
                    //
                    // Горизонтальная линия
                    //
                    const qreal height = pageRect.height() / 2.;
                    QPointF p1 = pageRect.topLeft() + QPointF(0, height);
                    QPointF p2 = pageRect.topRight() + QPointF(0, height);
                    painter.drawLine(p1, p2);
                    break;
                }

                case 4:
                case 6:
                case 8: {
                    //
                    // Горизонтальные линии
                    //
                    {
                        const qreal height = pageRect.height() / (cardsCount / 2.);
                        qreal summaryHeight = 0;
                        while (summaryHeight + height < pageRect.height()) {
                            summaryHeight += height;
                            const QPointF p1 = pageRect.topLeft() + QPointF(0, summaryHeight);
                            const QPointF p2 = pageRect.topRight() + QPointF(0, summaryHeight);
                            painter.drawLine(p1, p2);
                        }
                    }
                    //
                    // Вертикальная линия
                    //
                    {
                        const qreal width = pageRect.width() / 2.;
                        const QPointF p1 = pageRect.topLeft() + QPointF(width, 0);
                        const QPointF p2 = pageRect.bottomLeft() + QPointF(width, 0);
                        painter.drawLine(p1, p2);
                    }
                    break;
                }
            }
            painter.restore();
        }

        //
        // Определяем область на странице
        //
        QRectF cardRect = pageRect;
        cardRect.moveTop(cardRect.top() + lastY);
        switch (cardsCount) {
            default:
            case 1: {
                //
                // Вся страница
                //
                break;
            }

            case 2: {
                const qreal height = cardRect.height() / 2.;
                cardRect.setHeight(height);
                lastY += height;
                break;
            }

            case 4:
            case 6:
            case 8: {
                const qreal width = cardRect.width() / 2.;
                cardRect.setWidth(width);
                const qreal height = cardRect.height() / (cardsCount / 2.);
                cardRect.setHeight(height);
                //
                // Если крайняя в ряду карточка
                //
                if (currentCardIndex % 2 != 0) {
                    //
                    // ... смещаем область отрисовки
                    //
                    cardRect.moveLeft(cardRect.left() + width);
                    //
                    // ... и переходим к следующему ряду
                    //
                    lastY += height;
                }
                break;
            }
        }

        //
        // Дополнительные отступы
        //
        // ... снизу
        //
        if (cardRect.bottom() != pageRect.bottom()) {
            cardRect.setBottom(cardRect.bottom() - sideMargin);
        }
        //
        // ... сверху
        //
        if (cardRect.top() != pageRect.top()) {
            cardRect.setTop(cardRect.top() + sideMargin);
        }
        //
        // ... слева
        //
        if (cardRect.left() != pageRect.left()) {
            cardRect.setLeft(cardRect.left() + sideMargin);
        }
        //
        // ... и справа
        //
        if (cardRect.right() != pageRect.right()) {
            cardRect.setRight(cardRect.right() - sideMargin);
        }

        //
        // Рисуем карточку
        //
        {
//            painter.setClipRect(cardRect);

            //
            // Рисуем заголовок
            //
            QTextOption textoption;
            textoption.setAlignment(Qt::AlignTop | Qt::AlignLeft);
            textoption.setWrapMode(QTextOption::NoWrap);
            QFont font = painter.font();
            font.setBold(true);
            painter.setFont(font);
            const int titleHeight = painter.fontMetrics().height();
            const QRectF titleRect(cardRect.left(), cardRect.top(), cardRect.width(), titleHeight);
            QString titleText = item->title().isEmpty() ? item->header() : item->title();
            if (item->type() == BusinessLogic::ScenarioModelItem::Scene) {
                titleText.prepend(QString("%1. ").arg(item->sceneNumber()));
            }
            titleText = TextUtils::elidedText(titleText, painter.font(), titleRect.size(), textoption);
            painter.drawText(titleRect, titleText, textoption);

            //
            // Рисуем описание
            //
            textoption.setAlignment(Qt::AlignTop | Qt::AlignLeft);
            textoption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            font.setBold(false);
            painter.setFont(font);
            const qreal spacing = titleRect.height() / 2;
            const QRectF descriptionRect(titleRect.left(), titleRect.bottom() + spacing, titleRect.width(), cardRect.height() - titleRect.height() - spacing);
            QString descriptionText = item->description().isEmpty() ? item->fullText() : item->description();
            descriptionText.replace("\n", "\n\n");
            descriptionText = TextUtils::elidedText(descriptionText, painter.font(), descriptionRect.size(), textoption);
            painter.drawText(descriptionRect, descriptionText, textoption);
        }

        //
        // Переходим к следующей карточке
        //
        ++currentCardIndex;
        if (currentCardIndex == cardsCount) {
            currentCardIndex = 0;
            lastY = 0;
        }
    }

    //
    // Скрываем прогресс
    //
    m_printDialog->hideProgress();
    m_printDialog->setEnabled(true);
}

void ScenarioCardsManager::initConnections()
{
    //
    // Если не удалось загрузить сохранённую схему, построим её заново
    //
    connect(m_view, &ScenarioCardsView::schemeNotLoaded, [=] {
        m_view->load(m_model->simpleScheme());
    });

    connect(m_view, &ScenarioCardsView::undoRequest, this, &ScenarioCardsManager::undoRequest);
    connect(m_view, &ScenarioCardsView::redoRequest, this, &ScenarioCardsManager::redoRequest);

    connect(m_view, &ScenarioCardsView::addCardClicked, this, &ScenarioCardsManager::addCard);
    connect(m_view, &ScenarioCardsView::editCardRequest, this, &ScenarioCardsManager::editCard);
    connect(m_view, &ScenarioCardsView::removeCardRequest, this, &ScenarioCardsManager::removeCard);
    connect(m_view, &ScenarioCardsView::cardMoved, this, &ScenarioCardsManager::moveCard);
    connect(m_view, &ScenarioCardsView::cardMovedToGroup, this, &ScenarioCardsManager::moveCardToGroup);
    connect(m_view, &ScenarioCardsView::cardColorsChanged, this, &ScenarioCardsManager::changeCardColors);
    connect(m_view, &ScenarioCardsView::cardTypeChanged, this, &ScenarioCardsManager::changeCardType);

    connect(m_view, &ScenarioCardsView::printRequest, m_printDialog, &PrintCardsDialog::exec);
    connect(m_printDialog, &PrintCardsDialog::printPreview, this, &ScenarioCardsManager::print);

    connect(m_view, &ScenarioCardsView::fullscreenRequest, this, &ScenarioCardsManager::fullscreenRequest);

    connect(m_view, &ScenarioCardsView::cardsChanged, this, &ScenarioCardsManager::cardsChanged);
}

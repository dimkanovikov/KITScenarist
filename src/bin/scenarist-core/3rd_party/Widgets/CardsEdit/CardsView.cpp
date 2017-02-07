#include "CardsView.h"

#include "CardsScene.h"
#include "CardsUndoStack.h"
#include "ScalableGraphicsView.h"

#include <QVBoxLayout>


CardsView::CardsView(QWidget* _parent) :
    QWidget(_parent),
    m_scene(new CardsScene(this)),
    m_view(new ScalableGraphicsView(this)),
    m_undoStack(new CardsUndoStack)
{
    initView();
    initConnections();
}

CardsView::~CardsView()
{
    delete m_undoStack;
}

void CardsView::setUseCorkboardBackground(bool _use)
{
    if (m_useCorkboardBackground != _use) {
        m_useCorkboardBackground = _use;
        updateBackgroundBrush();
    }
}

void CardsView::setBackgroundColor(const QColor& _color)
{
    if (m_backgroundColor != _color) {
        m_backgroundColor = _color;
        updateBackgroundBrush();
    }
}

void CardsView::setCanAddActs(bool _can)
{
    m_scene->setCanAddActs(_can);
}

void CardsView::setFixedMode(bool _isFixed)
{
    m_scene->setFixedMode(_isFixed);
}

void CardsView::addAct(const QString& _uuid, const QString& _title, const QString& _description, const QString& _colors)
{
    m_scene->addAct(_uuid, _title, _description, _colors);
}

void CardsView::insertAct(const QString& _uuid, const QString& _title, const QString& _description,
    const QString& _colors, const QString& _previousItemUuid)
{
    m_scene->insertAct(_uuid, _title, _description, _colors, _previousItemUuid);
}

void CardsView::addCard(const QString& _uuid, bool _isFolder, const QString& _title, const QString& _description,
    const QString& _state, const QString& _colors)
{
    const QPoint viewCenter = m_view->rect().center();
    const QPointF viewCenterMapped = m_view->mapToScene(viewCenter);
    addCard(_uuid, _isFolder, _title, _description, _state, _colors, viewCenterMapped);
}

void CardsView::addCard(const QString& _uuid, bool _isFolder, const QString& _title, const QString& _description, const QString& _state, const QString& _colors, const QPointF& _position)
{
    m_scene->addCard(_uuid, _isFolder, _title, _description, _state, _colors, _position);
}

void CardsView::insertCard(const QString& _uuid, bool _isFolder, const QString& _title, const QString& _description,
    const QString& _state, const QString& _colors, const QPointF& _position, const QString& _previousItemUuid)
{
    m_scene->insertCard(_uuid, _isFolder, _title, _description, _state, _colors, _position, _previousItemUuid);
}

void CardsView::removeAct(const QString& _uuid)
{
    m_scene->removeAct(_uuid);
}

void CardsView::removeCard(const QString& _uuid)
{
    m_scene->removeCard(_uuid);
}

void CardsView::refresh()
{
    m_scene->refresh();
}

QString CardsView::save() const
{
    return m_scene->save();
}

bool CardsView::load(const QString& _xml)
{
    return m_scene->load(_xml);
}

void CardsView::undo()
{
    if (m_undoStack->canUndo()) {
        m_scene->load(m_undoStack->undo());
    }
}

void CardsView::redo()
{
    if (m_undoStack->canRedo()) {
        m_scene->load(m_undoStack->redo());
    }
}

void CardsView::saveChanges(bool _hasChangesInText)
{
    const QString xml = save();

    //
    // Если есть изменения сцены
    //
    if (m_undoStack->hasChanges(xml)) {
        //
        // ... то добавляем состояние в стек
        //
        m_undoStack->addState(xml, _hasChangesInText);
    }
    //
    // А если изменений сцены нет
    //
    else {
        //
        // ... но есть изменения текста
        //
        if (_hasChangesInText) {
            //
            // ... очищаем стэк отмены действий, т.к. мы не предоставляем отмены действий для случаев, когда был изменён текст сценария
            //
            m_undoStack->clear();
        }
    }
}

void CardsView::initView()
{
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    m_view->setScene(m_scene);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->addWidget(m_view);
    setLayout(layout);
}

void CardsView::initConnections()
{
    connect(m_scene, &CardsScene::actAddRequest, this, &CardsView::actAddRequest);
    connect(m_scene, &CardsScene::cardAddRequest, this, &CardsView::cardAddRequest);
    connect(m_scene, &CardsScene::cardAddCopyRequest, this, &CardsView::cardAddCopyRequest);
    connect(m_scene, &CardsScene::actAdded, this, &CardsView::actAdded);
    connect(m_scene, &CardsScene::cardAdded, this, &CardsView::cardAdded);
    connect(m_scene, &CardsScene::actChangeRequest, this, &CardsView::actChangeRequest);
    connect(m_scene, &CardsScene::cardChangeRequest, this, &CardsView::cardChangeRequest);
    connect(m_scene, &CardsScene::actChanged, this, &CardsView::actChanged);
    connect(m_scene, &CardsScene::cardChanged, this, &CardsView::cardChanged);
    connect(m_scene, &CardsScene::actRemoveRequest, this, &CardsView::actRemoveRequest);
    connect(m_scene, &CardsScene::cardRemoveRequest, this, &CardsView::cardRemoveRequest);
    connect(m_scene, &CardsScene::actRemoved, this, &CardsView::actRemoved);
    connect(m_scene, &CardsScene::cardRemoved, this, &CardsView::cardRemoved);
    connect(m_scene, &CardsScene::cardMoved, this, &CardsView::cardMoved);
    connect(m_scene, &CardsScene::cardMovedToGroup, this, &CardsView::cardMovedToGroup);
    connect(m_scene, &CardsScene::cardColorsChanged, this, &CardsView::cardColorsChanged);

    connect(m_view, &ScalableGraphicsView::scaleChanged, m_scene, &CardsScene::refresh);
}

void CardsView::updateBackgroundBrush()
{
    if (m_useCorkboardBackground) {
        QBrush brush(QImage(":/Graphics/Images/corkboard.jpg"));
        brush.setTransform(QTransform().scale(0.4, 0.4));
        m_view->setBackgroundBrush(brush);
        m_view->setCacheMode(QGraphicsView::CacheBackground);
    } else {
        m_view->setBackgroundBrush(m_backgroundColor);
    }
}

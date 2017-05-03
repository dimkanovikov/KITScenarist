#ifndef CARDSVIEW_H
#define CARDSVIEW_H

#include <QWidget>

class CardsScene;
class CardsUndoStack;
class ScalableGraphicsView;


/**
 * @brief Представление редактора карточек
 */
class CardsView : public QWidget
{
    Q_OBJECT

public:
    explicit CardsView(QWidget* _parent = 0);
    ~CardsView();

    /**
     * @brief Запретить/разрешить возможность редактирования
     */
    void setReadOnly(bool _readOnly);

    /**
     * @brief Необходимо ли использовать в качестве фона пробковую доску
     */
    void setUseCorkboardBackground(bool _use);

    /**
     * @brief Установить цвет фона
     */
    void setBackgroundColor(const QColor& _color);

    // ****
    // Интерфейс для работы со сценой

    /**
     * @brief Очистить сцену
     */
    void clear();

    /**
     * @brief Задать размер карточек
     */
    void setCardsSize(const QSizeF& _size);

    /**
     * @brief Задать расстояние между элементами
     */
    void setCardsDistance(qreal _distance);

    /**
     * @brief Установить количество карточек в ряду
     */
    void setCardsInRow(int _count);

    /**
     * @brief Установить доступность возможности добавления актов
     */
    void setCanAddActs(bool _can);

    /**
     * @brief Установить режим привязки карточек к сетке
     */
    void setFixedMode(bool _isFixed);

    /**
     * @brief Получить идентификатор последнего элемента
     */
    QString lastItemUuid() const;

    /**
     * @brief Получить идентификатор карточки следующей перед заданной позицией
     */
    QString beforeNewItemUuid(const QPointF& _newCardPosition) const;

    /**
     * @brief Вставить акт после заданного элемента
     */
    void insertAct(const QString& _uuid, const QString& _title, const QString& _description,
        const QString& _colors, const QString& _previousItemUuid);

    /**
     * @brief Вставить карточку после заданного элемента
     */
    void insertCard(const QString& _uuid, bool _isFolder, int _number, const QString& _title,
        const QString& _description, const QString& _stamp, const QString& _colors,
        bool _isEmbedded, const QPointF& _position, const QString& _previousItemUuid);

    /**
     * @brief Обновить заданную карточку
     */
    void updateItem(const QString& _uuid, bool _isFolder, int _number, const QString& _title,
        const QString& _description, const QString& _stamp, const QString& _colors,
        bool _isEmbedded, bool _isAct);

    /**
     * @brief Удалить заданную карточку
     */
    void removeItem(const QString& _uuid);

    /**
     * @brief Удалить выделенный элемент
     */
    void removeSelectedItem();

    /**
     * @brief Обновить сцену
     */
    void refresh();

    /**
     * @brief Сформировать xml на основе сцены
     */
    QString save() const;

    /**
     * @brief Сохранить в картинку
     */
    void saveToImage(const QString& _filePath);

    /**
     * @brief Загрузить сцены из xml
     */
    bool load(const QString& _xml);

    // ****
    // Методы для работы непосредственно с доской

    /**
     * @brief Необходимо ли синхронизировать отмену последенго действия с текстом
     */
    bool needSyncUndo() const;

    /**
     * @brief Отменить последнее действие
     */
    void undo();

    /**
     * @brief Необходимо ли синхронизировать повтор последенго действия с текстом
     */
    bool needSyncRedo() const;

    /**
     * @brief Повторить последнее действие
     */
    void redo();

    /**
     * @brief Сохранить изменения схемы
     */
    void saveChanges(bool _hasChangesInText = false);

signals:
    /**
     * @brief Пользователь хочет добавить элемент в заданной позиции
     */
    /** @{ */
    void actAddRequest(const QPointF& _position);
    void cardAddRequest(const QPointF& _position);
    /** @} */

    /**
     * @brief Пользователь хочет добавить копию карточки в заданной позиции
     */
    void cardAddCopyRequest(bool _isFolder, const QString& _title, const QString& _description, const QString& _state,
        const QString& _colors, const QPointF& _position);

    /**
     * @brief Элемент был добавлен
     */
    /** @{ */
    void actAdded(const QString& _uuid);
    void cardAdded(const QString& _uuid);
    /** @} */

    /**
     * @brief Пользователь хочет изменить данные акта/карточки
     */
    /** @{ */
    void actChangeRequest(const QString& _uuid);
    void cardChangeRequest(const QString& _uuid);
    /** @} */

    /**
     * @brief Данные акта/карточки были изменены
     */
    /** @{ */
    void actChanged(const QString& _uuid);
    void cardChanged(const QString& _uuid);
    /** @} */

    /**
     * @brief Пользователь хочет удалить акт/карточку
     */
    /** @{ */
    void actRemoveRequest(const QString& _uuid);
    void cardRemoveRequest(const QString& _uuid);
    /** @} */

    /**
     * @brief Акт/карточка была удалена
     */
    /** @{ */
    void actRemoved(const QString& _uuid);
    void cardRemoved(const QString& _uuid);
    /** @} */

    /**
     * @brief Карточка перенесена
     * @param _cardId идентификатор карточки
     * @param _actId идентификатор акта, QString::null если карточка выносится за пределы акта
     * @param _previousCardId идентификатор карточки, после которой будет вставлена перетаскиваемая
     *                        карточка, QString::null если карточка вставляется в начало акта
     */
    void cardMoved(const QString& _cardId, const QString& _actId, const QString& _previousCardId);

    /**
     * @brief Карточка была вложена в группирующую карточку
     */
    void cardMovedToGroup(const QString& _cardId, const QString& _groupId);

    /**
     * @brief Изменились цвета карточки
     */
    void cardColorsChanged(const QString& _uuid, const QString& _colors);

    /**
     * @brief Запрос на изменение типа карточки
     */
    void cardTypeChanged(const QString& _uuid, bool _isFolder);

    /**
     * @brief Карточки были изменены
     */
    void cardsChanged();

protected:
    /**
     * @brief Переопределяем для обработки события смены палитры
     */
    bool event(QEvent* _event) override;

    /**
     * @brief Переопределяем для переупорядочивания карточек
     */
    void resizeEvent(QResizeEvent* _event) override;

private:
    /**
     * @brief Настройка представления
     */
    void initView();

    /**
     * @brief Настройка соединений
     */
    void initConnections();

    /**
     * @brief Настроить стиль
     */
    void initStyleSheet();

    /**
     * @brief Обновить фон
     */
    void updateBackgroundBrush();

private:
    /**
     * @brief Сцена
     */
    CardsScene* m_scene = nullptr;

    /**
     * @brief Представление сцены
     */
    ScalableGraphicsView* m_view = nullptr;

    /**
     * @brief Стек отмены/повтора последнего действия
     */
    CardsUndoStack* m_undoStack = nullptr;

    /**
     * @brief Использовать ли для фона заливку в виде пробковой доски
     */
    bool m_useCorkboardBackground = false;

    /**
     * @brief Цвет фона, когда не используется пробковая доска
     */
    QColor m_backgroundColor;
};

#endif // CARDSVIEW_H

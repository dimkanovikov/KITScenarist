#ifndef SCENARIOCARDSVIEW_H
#define SCENARIOCARDSVIEW_H

#include <QWidget>

class CardsView;
class FlatButton;
class QLabel;

namespace UserInterface {
    class CardsResizer;


    /**
     * @brief Представление редактора карт
     */
    class ScenarioCardsView : public QWidget
    {
        Q_OBJECT

    public:
        explicit ScenarioCardsView(bool _isDraft, QWidget* _parent = 0);

        /**
         * @brief Очистить схему
         */
        void clear();

        /**
         * @brief Отменить последнее действие
         */
        void undo();

        /**
         * @brief Повторить последнее действие
         */
        void redo();

        /**
         * @brief Необходимо ли использовать в качестве фона пробковую доску
         */
        void setUseCorkboardBackground(bool _use);

        /**
         * @brief Установить цвет фона
         */
        void setBackgroundColor(const QColor& _color);

        /**
         * @brief Загрузить схему из xml-строки
         */
        void load(const QString& _xml);

        /**
         * @brief Получить xml-строку текущей схемы
         */
        QString save() const;

        /**
         * @brief Сохранить изменения схемы
         */
        void saveChanges(bool _hasChangesInText);

        /**
         * @brief Вставить карточку после заданной
         */
        void insertCard(const QString& _uuid, bool _isFolder, int _number, const QString& _title,
            const QString& _description, const QString& _stamp, const QString& _colors,
            bool _isEmbedded, const QString& _previousCardUuid);

        /**
         * @brief Обновить карточку с заданным uuid
         */
        void updateCard(const QString& _uuid, bool _isFolder, int _number, const QString& _title,
            const QString& _description, const QString& _stamp, const QString& _colors,
            bool _isEmbedded, bool _isAct);

        /**
         * @brief Удалить карточку с заданным uuid
         */
        void removeCard(const QString& _uuid);

        /**
         * @brief Получить идентификатор последнего элемента
         */
        QString lastItemUuid() const;

        /**
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

    signals:
        /**
         * @brief Не удалось загрузить схему
         */
        void schemeNotLoaded();

        /**
         * @brief Запрос на отмену последнего действия
         */
        void undoRequest();

        /**
         * @brief Запрос на повтор последнего действия
         */
        void redoRequest();

        /**
         * @brief Нажата кнопка добавления карточки
         */
        void addCardClicked();

        /**
         * @brief Запрос на создание копии карточки
         */
        void addCopyCardRequest(bool _isFolder, const QString& _title, const QString& _description, const QString& _stamp, const QString& _colors);

        /**
         * @brief Запросы на изменение выделенной фигуры
         */
        /** @{ */
        void editCardRequest(const QString& _uuid);
        /** @} */

        /**
         * @brief Нажата кнопка удаления карточки
         */
        void removeCardRequest(const QString& _uuid);

        /**
         * @brief Карточка была перемещена
         */
        void cardMoved(const QString& _cardId, const QString& _actId, const QString& _previousCardId);

        /**
         * @brief Изменились цвета карточки
         */
        void cardColorsChanged(const QString& _uuid, const QString& _colors);

        /**
         * @brief Запрос на изменение типа карточки
         */
        void cardTypeChanged(const QString& _uuid, bool _isFolder);

        /**
         * @brief Запрос на переход в полноэкранный режим, или выход из него
         */
        void fullscreenRequest();

        /**
         * @brief Схема карточек изменена
         */
        void cardsChanged();

    private:
        /**
         * @brief Упорядочить карточки по сетке
         */
        void resortCards();

    private:
        /**
         * @brief Настроить представление
         */
        void initView(bool _isDraft);

        /**
         * @brief Настроить соединения
         */
        void initConnections();

        /**
         * @brief Настроить горячие клавиши
         */
        void initShortcuts();

        /**
         * @brief Настроить внешний вид
         */
        void initStyleSheet();

    private:
        /**
         * @brief Редактор карточек сценария
         */
        CardsView* m_cards = nullptr;

        /**
         * @brief Активированы ли карточки
         */
        FlatButton* m_active = nullptr;

        /**
         * @brief Кнопка добавления карточки
         */
        FlatButton* m_addCard = nullptr;

        /**
         * @brief Кнопка удаления карточки
         */
        FlatButton* m_removeCard = nullptr;

        /**
         * @brief Кнопка упорядочивания по таблице
         */
        FlatButton* m_sort = nullptr;

        /**
         * @brief Виджет настройки размера и упорядочивания карточек
         */
        CardsResizer* m_resizer = nullptr;

        /**
         * @brief Перейти в полноэкранный режим
         */
        FlatButton* m_fullscreen = nullptr;

        /**
         * @brief Метка, для закрашивания пространства в панели инструментов
         */
        QLabel* m_toolbarSpacer = nullptr;

        /**
         * @brief Позиция вставки новой карточки
         */
        QPointF m_newCardPosition;
    };
}

#endif // SCENARIOCARDSVIEW_H

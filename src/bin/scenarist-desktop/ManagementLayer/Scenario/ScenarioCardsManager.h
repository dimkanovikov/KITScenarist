#ifndef SCENARIOCARDSMANAGER_H
#define SCENARIOCARDSMANAGER_H

#include <QObject>

class QPrinter;

namespace Domain {
    class Scenario;
}

namespace BusinessLogic {
    class ScenarioModel;
}

namespace UserInterface {
    class PrintCardsDialog;
    class ScenarioCardsView;
    class ScenarioSchemeItemDialog;
}


namespace ManagementLayer
{
    /**
     * @brief Управляющий карточками сценария
     */
    class ScenarioCardsManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ScenarioCardsManager(QObject* _parent, QWidget* _parentWidget);

        QWidget* view() const;

        /**
         * @brief Перезагрузить настройки
         */
        void reloadSettings();

        /**
         * @brief Сохранить схему сценария
         */
        QString save() const;

        /**
         * @brief Сохранить изменения схемы
         */
        void saveChanges(bool _hasChangesInText);

        /**
         * @brief Загрузить заданную схему
         */
        void load(BusinessLogic::ScenarioModel* _model, const QString& _xml);

        /**
         * @brief Очистить данные схемы и модель
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
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

    signals:
        /**
         * @brief Запрос на отмену последнего действия
         */
        void undoRequest();

        /**
         * @brief Запрос на повтор последнего действия
         */
        void redoRequest();

        /**
         * @brief Схема карточек изменена
         */
        void cardsChanged();

        /**
         * @brief Запрос на добавление элемента
         */
        void addCardRequest(const QModelIndex& _afterItemIndex, int _type, const QString& _title,
            const QColor& _color, const QString& _description);

        /**
         * @brief Запрос на изменение элемента
         */
        void updateCardRequest(const QModelIndex& _index, int _type, const QString& _title,
            const QString& _colors, const QString& _description);

        /**
         * @brief Запрос на удаление элемента
         */
        void removeCardRequest(const QModelIndex& _index);

        /**
         * @brief Изменились цвета карточки
         */
        void cardColorsChanged(const QModelIndex& _index, const QString& _colors);

        /**
         * @brief Запрос на изменение типа карточки
         */
        void cardTypeChanged(const QModelIndex& _index, int _cardType);

        /**
         * @brief Запрос на переход в полноэкранный режим, или выход из него
         */
        void fullscreenRequest();

    private:
        /**
         * @brief Добавить элемент после выбранного
         */
        void addCard();

        /**
         * @brief Изменить карточку
         */
        void editCard(const QString& _uuid);

        /**
         * @brief Удалить карточку
         */
        void removeCard(const QString& _uuid);

        /**
         * @brief Переместить сцену в соответствии с перемещённой карточкой
         */
        void moveCard(const QString& _cardId, const QString& _actId, const QString& _previousCardId);

        /**
         * @brief Изменить цвета карточки
         */
        void changeCardColors(const QString& _uuid, const QString& _colors);

        /**
         * @brief Изменить тип карточки
         */
        void changeCardType(const QString& _uuid, bool _isFolder);

        /**
         * @brief Напечатать карточки
         */
        /** @{ */
        void print();
        void printCards(QPrinter* _printer);
        /** @} */

    private:
        /**
         * @brief Настроить соединения
         */
        void initConnections();

    private:
        /**
         * @brief Представление редактора карт
         */
        UserInterface::ScenarioCardsView* m_view = nullptr;

        /**
         * @brief Диалог добавления элемента
         */
        UserInterface::ScenarioSchemeItemDialog* m_addItemDialog = nullptr;

        /**
         * @brief Диалог печати
         */
        UserInterface::PrintCardsDialog* m_printDialog = nullptr;

        /**
         * @brief Сценарий
         */
        Domain::Scenario* m_scenario = nullptr;

        /**
         * @brief Модель сценария
         */
        BusinessLogic::ScenarioModel* m_model = nullptr;
    };
}

#endif // SCENARIOCARDSMANAGER_H

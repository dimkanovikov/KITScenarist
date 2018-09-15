#ifndef SCENARIOTEXTEDITMANAGER_H
#define SCENARIOTEXTEDITMANAGER_H

#include <QObject>

class QMenu;
class QTextCursor;


namespace UserInterface {
    class ScenarioTextEditWidget;
}

namespace BusinessLogic {
    class ScenarioTextDocument;
}

namespace ManagementLayer
{
    /**
     * @brief Управляющий редактированием сценария
     */
    class ScenarioTextEditManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ScenarioTextEditManager(QObject* _parent, QWidget* _parentWidget);

        QWidget* toolbar() const;
        QWidget* view() const;

        /**
         * @brief Установить документ для редактирования
         */
        void setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document, bool _isDraft = false);

        /**
         * @brief Установить хронометраж
         */
        void setDuration(const QString& _duration);

        /**
         * @brief Установить значения счётчиков
         */
        void setCountersInfo(const QStringList& _counters);

        /**
         * @brief Установить позицию курсора
         */
        void setCursorPosition(int _position);

        /**
         * @brief Перезагрузить параметры текстового редактора
         */
        void reloadTextEditSettings();

        /**
         * @brief Получить текущую позицию курсора
         */
        int cursorPosition() const;

        /**
         * @brief Установить список дополнительных курсоров для отрисовки
         */
        void setAdditionalCursors(const QMap<QString, int>& _cursors);

        /**
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

        /**
         * @brief Пролистать сценарий, чтобы был виден заданный курсор соавтора
         */
        void scrollToAdditionalCursor(int _additionalCursorIndex);

        /**
         * @brief Пролистать сценарий, чтобы был виден заданный курсор
         */
        void scrollToPosition(int _position);

        /**
         * @brief Задать, зафиксированы ли номера сцен
         */
        void setFixed(bool _fixed);

        /**
         * @brief Установить префикс номеров сцен
         */
        void setSceneNumbersPrefix(const QString& _prefix);

#ifdef Q_OS_MAC
        /**
         * @brief Сформировать меню "Правка"
         */
        void buildEditMenu(QMenu* _menu);
#endif

    public slots:
        /**
         * @brief Добавить элемент сценария в указанной позиции
         */
        void addScenarioItem(int _position, int _type, const QString& _name,
            const QString& _header, const QString& _description, const QColor& _color);

        /**
         * @brief Изменить элемент сценария
         */
        void editScenarioItem(int _startPosition, int _type, const QString& _name,
            const QString& _header, const QString& _colors);

        /**
         * @brief Удалить заданный текст сценария
         */
        void removeScenarioText(int _from, int _to);

        /**
         * @brief Установить в заданной позиции заданный тип блока
         */
        void changeItemType(int _position, int _type);

        /**
         * @brief Включить/выключить дзен режим
         */
        void setZenMode(bool _isZen);

    signals:
        /**
         * @brief Изменился текст сценария
         */
        void textChanged();

        /**
         * @brief Изменилась позиция курсора
         */
        void cursorPositionChanged(int _position);

        /**
         * @brief Запрос отмены действия
         */
        void undoRequest();

        /**
         * @brief Запрос повтора действия
         */
        void redoRequest();

        /**
         * @brief Изменился режим отображения сценария
         */
        void textModeChanged();

        /**
         * @brief Выйти из дзен режима
         */
        void quitFromZenMode();

        /**
         * @brief Пользователь хочет добавить закладку в заданном месте документа
         */
        void addBookmarkRequested(int _position);

        /**
         * @brief Пользователь хочет убрать закладку в заданном месте документа
         */
        void removeBookmarkRequested(int _position);

        /**
         * @brief Зафиксированы/расфиксированы номера сцен
         */
        void changeSceneNumbersLockingRequest();

        /**
         * @brief Пользователь хочет переименовать номер сцены
         */
        void renameSceneNumberRequested(const QString& _newSceneNumber, int _position);

    private slots:
        /**
         * @brief Реакция на изменение коэффициента масштабирования редактора сценария
         */
        void aboutTextEditZoomRangeChanged(qreal _zoomRange);

        /**
         * @brief Переименовать номер сцены
         */
        void renameSceneNumber(const QString& _oldSceneNumber, int _position);

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

    private:
        /**
         * @brief Редактор
         */
        UserInterface::ScenarioTextEditWidget* m_view;
    };
}

#endif // SCENARIOTEXTEDITMANAGER_H

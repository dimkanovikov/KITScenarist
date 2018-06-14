#ifndef SCENARIOMANAGER_H
#define SCENARIOMANAGER_H

#include <QObject>
#include <QTimer>
#include <QModelIndex>

class FlatButton;
class QComboBox;
class QLabel;
class QMenu;
class QSplitter;
class QStackedWidget;
class TabBar;

namespace BusinessLogic {
    class ScenarioDocument;
}

namespace ManagementLayer
{
    class ScenarioCardsManager;
    class ScenarioNavigatorManager;
    class ScenarioSceneDescriptionManager;
    class ScriptBookmarksManager;
    class ScriptDictionariesManager;
    class ScenarioTextEditManager;


    /**
     * @brief Управляющий сценарием
     */
    class ScenarioManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ScenarioManager(QObject* _parent, QWidget* _parentWidget);

        QWidget* view() const;
        QWidget* cardsView() const;

        /**
         * @brief Получить документ сценария
         */
        BusinessLogic::ScenarioDocument* scenario() const;

        /**
         * @brief Получить черновик сценария
         */
        BusinessLogic::ScenarioDocument* scenarioDraft() const;

        /**
         * @brief Загрузить состояние представления
         */
        void loadViewState();

        /**
         * @brief Получить текущую позицию курсора
         */
        int cursorPosition() const;

        /**
         * @brief Установть позицию курсора
         */
        void setCursorPosition(int _position) const;

        /**
         * @brief Загрузить данные текущего проекта
         */
        void loadCurrentProject();

        /**
         * @brief Сформировать карточки из сценария
         */
        void rebuildCardsFromScript();

        /**
         * @brief Запустить таймер сохранения изменений
         */
        void startChangesHandling();

        /**
         * @brief Загрузить настройки текущего проекта
         */
        void loadCurrentProjectSettings(const QString& _projectPath);

        /**
         * @brief Сохранить данные текущего проекта
         */
        void saveCurrentProject();

        /**
         * @brief Сохранить настройки текущего проекта
         */
        void saveCurrentProjectSettings(const QString& _projectPath);

        /**
         * @brief Закрыть текущий проект
         */
        void closeCurrentProject();

        /**
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

        /**
         * @brief Узнать текущий режим
         */
        bool workModeIsDraft() const;

        /**
         * @brief Установить префикс номеров сцен
         */
        void setSceneNumbersPrefix(const QString& _prefix);

        /**
         * @brief Установить стартовый номер сцен
         */
        void setSceneStartNumber(int _startNumber);

        /**
         * @brief Пользователь хочет переименовать номер сцены
         */
        void renameSceneNumberRequested(const QString& _newName, int _position);

#ifdef Q_OS_MAC
        /**
         * @brief Сформировать меню "Правка" для редактора сценария
         */
        void buildScriptEditMenu(QMenu* _menu);
#endif

        /**
         * @brief Обновить параметры редактора карточек
         */
        void aboutCardsSettingsUpdated();

        /**
         * @brief Обновить параметры текстового редактора
         */
        void aboutTextEditSettingsUpdated();

        /**
         * @brief Обновить параметры навигатора
         */
        void aboutNavigatorSettingsUpdated();

        /**
         * @brief Пересчитать хронометраж
         */
        void aboutChronometrySettingsUpdated();

        /**
         * @brief Обновить счётчики
         */
        void aboutCountersSettingsUpdated();

        /**
         * @brief Изменено имя персонажа
         */
        void aboutCharacterNameChanged(const QString& _oldName, const QString& _newName);

        /**
         * @brief Пересоздать персонажей
         */
        void aboutRefreshCharacters();

        /**
         * @brief Изменено название локации
         */
        void aboutLocationNameChanged(const QString& _oldName, const QString& _newName);

        /**
         * @brief Пересоздать локации
         */
        void aboutRefreshLocations();

        /**
         * @brief Применить патч к сценарию
         */
        /** @{ */
        void aboutApplyPatch(const QString& _patch, bool _isDraft);
        void aboutApplyPatches(const QList<QString>& _patches, bool _isDraft);
        /** @} */

        /**
         * @brief Очистить список курсоров пользователей
         */
        void clearAdditionalCursors();

        /**
         * @brief Получены новые позиции курсоров пользователей
         */
        void aboutCursorsUpdated(const QMap<QString, int>& _cursors, bool _isDraft);

        /**
         * @brief Пролистать сценарий, чтобы курсор соавтора был виден
         */
        void scrollToAdditionalCursor(int _additionalCursorIndex);

        /**
         * @brief Включить/выключить дзен режим
         */
        void setZenMode(bool _isZen);

        /**
         * @brief Загрузить сценарий из заданного xml
         */
        void setScriptXml(const QString& _xml);

    signals:
        /**
         * @brief Сценарий изменён
         */
        void scenarioChanged();

        /**
         * @brief Перейти в полноэкранный режим
         */
        void showFullscreen();

        /**
         * @brief Запрос на обновление текста сценария
         */
        void updateScenarioRequest();

        /**
         * @brief Запрос на обновление курсоров соавторов
         * @note Используется для отправки на сервер информации о курсоре
         */
        void updateCursorsRequest(int _position, bool _isDraft);

        /**
         * @brief Была активирована ссылка для перехода между модулями программы
         */
        void linkActivated(const QUrl& _url);

    private slots:
        /**
         * @brief Отменить последнее действие
         */
        void aboutUndo();

        /**
         * @brief Повторить последнее действие
         */
        void aboutRedo();

        /**
         * @brief Обновить хронометраж
         */
        /** @{ */
        void aboutRefreshDuration(int _cursorPosition);
        void aboutUpdateDuration(int _cursorPosition);
        /** @} */

        /**
         * @brief Обновить счётчики
         */
        /** @{ */
        void aboutRefreshCounters();
        void aboutUpdateCounters();
        /** @} */

        /**
         * @brief Обновить название и описание текущей сцены, если она сменилась
         */
        void aboutUpdateCurrentSceneTitleAndDescription(int _cursorPosition);

        /**
         * @brief Название текущей сцены было изменён
         */
        void aboutUpdateCurrentSceneTitle(const QString& _title);

        /**
         * @brief Скопировать описание сцены в текст сцены
         */
        void copySceneDescriptionToScript();

        /**
         * @brief Описание текущей сцены было изменён
         */
        void aboutUpdateCurrentSceneDescription(const QString& _description);

        /**
         * @brief Выделить текущую сцену в навигаторе
         */
        void aboutSelectItemInNavigator(int _cursorPosition);

        /**
         * @brief Сместить курсор к выбранной сцене
         */
        /** @{ */
        void aboutMoveCursorToItem(const QModelIndex& _index);
        void aboutMoveCursorToItem(int _itemPosition);
        /** @} */

        /**
         * @brief Перейти к тексту заданной сцены
         */
        void aboutGoToItemFromCards(const QModelIndex& _index);

        /**
         * @brief Добавить элемент, после заданного
         */
        /** @{ */
        void aboutAddItemFromCards(const QModelIndex& _afterItemIndex, int _itemType, const QString& _title,
            const QColor& _color, const QString& _description);
        void aboutAddItem(const QModelIndex& _afterItemIndex, int _itemType, const QString& _header,
            const QColor& _color, const QString& _description);
        /** @}*/

        /**
         * @brief Изменить заданный элемент
         */
        void aboutUpdateItemFromCards(const QModelIndex& _itemIndex, int _itemType, const QString& _header,
            const QString& _colors, const QString& _description);

        /**
         * @brief Удалить заданный элемент
         */
        /** @{ */
        void aboutRemoveItemFromCards(const QModelIndex& _itemIndex);
        void aboutRemoveItems(const QModelIndexList& _itemIndex);
        /** @} */

        /**
         * @brief Установить цвет элемента
         */
        void aboutSetItemsColors(const QModelIndexList& _indexes, const QString& _colors);

        /**
         * @brief Установить штамп элемента
         */
        void aboutSetItemStamp(const QModelIndex& _itemIndex, const QString& _stamp);

        /**
         * @brief Сменить тип элемента
         */
        void aboutChangeItemType(const QModelIndex& _index, int _type);

        /**
         * @brief Показать/скрыть заметки к сцене
         */
        void setDraftVisible(bool _visible);

        /**
         * @brief Показать/скрыть заметки к сцене
         */
        void setSceneDescriptionVisible(bool _visible);

        /**
         * @brief Показать/скрыть закладки сценария
         */
        void setScriptBookmarksVisible(bool _visible);

        /**
         * @brief Показать/скрыть справочники сценария
         */
        void setScriptDictionariesVisible(bool _visible);

        /**
         * @brief Показать/скрыть одну из панелей навигатора
         */
        void setNavigatorPanelVisible(int _panelIndex, bool _visible);

        /**
         * @brief Сохранить изменение текста
         */
        void aboutSaveScenarioChanges();

    private:
        /**
         * @brief Загрузить данные
         */
        void initData();

        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

        /**
         * @brief Настроить внешний вид
         */
        void initStyleSheet();

        /**
         * @brief Изменить текущий режим работы в зависимости от испустившего сигнал навигатора
         */
        void setWorkingMode(QObject* _sender);

        /**
         * @brief Получить документ сценария в соответсвии с режимом работы
         */
        BusinessLogic::ScenarioDocument* workingScenario() const;

        /**
         * @brief Зафиксировать номера сцен
         */
        void changeSceneNumbersLocking();

    private:
        /**
         * @brief Представление сценария
         */
        QWidget* m_view = nullptr;

        /**
         * @brief Разделители представленя
         */
        /** @{ */
        QSplitter* m_mainSplitter = nullptr;
        QSplitter* m_navigatorSplitter = nullptr;
        /** @} */

        /**
         * @brief Кнопка перехода в полноэкранный режим
         */
        FlatButton* m_showFullscreen;

        /**
         * @brief Панели инструментов редакторов
         */
        QStackedWidget* m_viewEditorsToolbars;

        /**
         * @brief Редакторы
         */
        QStackedWidget* m_viewEditors;

        /**
         * @brief Документ сценария
         */
        BusinessLogic::ScenarioDocument* m_scenario;

        /**
         * @brief Документ черновика сценария
         */
        BusinessLogic::ScenarioDocument* m_scenarioDraft;

        /**
         * @brief Управляющий карточками
         */
        ScenarioCardsManager* m_cardsManager;

        /**
         * @brief Управляющий навигацией по сценарию
         */
        ScenarioNavigatorManager* m_navigatorManager;

        /**
         * @brief Управляющий навигацией по черновику сценария
         */
        ScenarioNavigatorManager* m_draftNavigatorManager;

        /**
         * @brief Управляющий синопсисом сцены
         */
        ScenarioSceneDescriptionManager* m_sceneDescriptionManager;

        /**
         * @brief Менеджер закладок сценария
         */
        ScriptBookmarksManager* m_scriptBookmarksManager = nullptr;

        /**
         * @brief Управляющий справочниками сценария
         */
        ScriptDictionariesManager* m_scriptDictionariesManager = nullptr;

        /**
         * @brief Управляющий редактированием сценария
         */
        ScenarioTextEditManager* m_textEditManager;

        /**
         * @brief Текущий рабочий режим
         */
        bool m_workModeIsDraft;

        /**
         * @brief Зафиксированы ли номера сцен
         */
        /** @{ */
        bool m_fixedScenes = false;
        bool m_fixedScenesDraft = false;
        /** @} */

        /**
         * @brief Курсоры соавторов
         */
        /** @{ */
        QMap<QString, int> m_cleanCursors;
        QMap<QString, int> m_draftCursors;
        /** @} */

        /**
         * @brief Таймер для сохранения изменений сценария
         */
        QTimer m_saveChangesTimer;
    };
}

#endif // SCENARIOMANAGER_H

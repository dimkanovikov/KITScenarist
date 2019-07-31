#ifndef RESEARCHMANAGER_H
#define RESEARCHMANAGER_H

#include <QObject>
#include <QMap>

class QAbstractItemModel;

namespace UserInterface {
    class ResearchView;
    class ResearchItemDialog;
}

namespace Domain {
    class Research;
}

namespace BusinessLogic {
    class ResearchModel;
    class ResearchModelItem;
    class ScenarioDocument;
}


namespace ManagementLayer
{
    /**
     * @brief Управляющий разработкой
     */
    class ResearchManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ResearchManager(QObject* _parent, QWidget* _parentWidget);

        /**
         * @brief Получить представление контроллера
         */
        QWidget* view() const;

        /**
         * @brief Получить модель разработки
         */
        QAbstractItemModel* model() const;

        /**
         * @brief Загрузить данные текущего проекта
         */
        void loadCurrentProject();

        /**
         * @brief Загрузить данные о сценарии из хранилища
         */
        void loadScenarioData();

        /**
         * @brief Загрузить настройки текущего проекта
         */
        void loadCurrentProjectSettings(const QString& _projectPath);

        /**
         * @brief Очистить все загруженные данные
         */
        void closeCurrentProject();

        /**
         * @brief Сохранить настройки текущего проекта
         */
        void saveCurrentProjectSettings(const QString& _projectPath);

        /**
         * @brief Обновить параметры
         */
        void updateSettings();

        /**
         * @brief Сохранить разработки проекта
         */
        void saveResearch();

        /**
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

        /**
         * @brief Получить название сценария
         */
        QString scenarioName() const;

        /**
         * @brief Получить верхний колонтитул
         */
        QString scriptHeader() const;

        /**
         * @brief Получить нижний колонтитул
         */
        QString scriptFooter() const;

        /**
         * @brief Получить префикс номеров сцен
         */
        QString sceneNumbersPrefix() const;

        /**
         * @brief Получить стартовый номер сцен
         */
        int sceneStartNumber() const;

        /**
         * @brief Разрешить или запретить изменение начального номера сцен
         */
        void setSceneStartNumberEnabled(bool _disabled);

        /**
         * @brief Получить данные о сценарии
         */
        QMap<QString, QString> scenarioData() const;

    signals:
        /**
         * @brief Было изменено название проекта
         */
        void scriptNameChanged(const QString& _name);

        /**
         * @brief Был изменён верхний колонтитул
         */
        void scriptHeaderChanged(const QString& _header);

        /**
         * @brief Был изменён нижний колонтитул
         */
        void scriptFooterChanged(const QString& _footer);

        /**
         * @brief Был изменён префикс номеров сцен
         */
        void sceneNumbersPrefixChanged(const QString& _prefix);

        /**
         * @brief Был изменен стартовый номер сцен
         */
        void sceneStartNumberChanged(int _startNumber);

        /**
         * @brief Изменился список версий проекта
         */
        void versionsChanged();

        /**
         * @brief Была изменена разработка
         */
        void researchChanged();

        /**
         * @brief Обновить список персонажей
         */
        void refreshCharacters();

        /**
         * @brief Было изменено имя персонажа
         */
        void characterNameChanged(const QString& _oldName, const QString& _newName);

        /**
         * @brief Было изменено название локации
         */
        void locationNameChanged(const QString& _oldName, const QString& _newName);

        /**
         * @brief Обновить список локаций
         */
        void refreshLocations();

        /**
         * @brief Запрос на добавление версии сценария
         */
        void addScriptVersionRequested();

    private:
        /**
         * @brief Добавить разработку
         */
        void addResearch(const QModelIndex& _index, int _type = -1);

        /**
         * @brief Изменить разработку
         */
        void editResearch(const QModelIndex& _index);

        /**
         * @brief Удалить разработку
         */
        /** @{ */
        void removeResearch(const QModelIndex& _index);
        void removeResearchItem(BusinessLogic::ResearchModelItem* _item);
        /** @} */

        /**
         * @brief Обновить поддерево разработки
         */
        void refreshResearchSubtree(const QModelIndex& _index);

        /**
         * @brief Показать контекстное меню навигатора
         */
        void showNavigatorContextMenu(const QModelIndex& _index, const QPoint& _pos);

        /**
         * @brief Обновить данные сценария
         */
        void updateScenarioData(const QString& _key, const QString& _value);

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
         * @brief Представление
         */
        UserInterface::ResearchView* m_view;

        /**
         * @brief Диалог добавления элемента разработки
         */
        UserInterface::ResearchItemDialog* m_dialog;

        /**
         * @brief Данные сценария
         */
        QMap<QString, QString> m_scenarioData;

        /**
         * @brief Модель данных о разработке
         */
        BusinessLogic::ResearchModel* m_model;

        /**
         * @brief Текущий элемент разработки
         */
        BusinessLogic::ResearchModelItem* m_currentResearchItem;
        Domain::Research* m_currentResearch;

        /**
         * @brief Документ сценария для отображения версий
         */
        BusinessLogic::ScenarioDocument* m_script = nullptr;
    };
}

#endif // RESEARCHMANAGER_H

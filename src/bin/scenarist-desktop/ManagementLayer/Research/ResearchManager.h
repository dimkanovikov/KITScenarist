#ifndef RESEARCHMANAGER_H
#define RESEARCHMANAGER_H

#include <QObject>
#include <QMap>

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

        QWidget* view() const;

        /**
         * @brief Загрузить данные текущего проекта
         */
        void loadCurrentProject();

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
         * @brief Получить данные о сценарии
         */
        QMap<QString, QString> scenarioData() const;

    signals:
        /**
         * @brief Было изменено название проекта
         */
        void scenarioNameChanged(const QString& _name);

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

    private:
        /**
         * @brief Добавить разработку
         */
        void addResearch(const QModelIndex& _index);

        /**
         * @brief Изменить разработку
         */
        void editResearch(const QModelIndex& _index);

        /**
         * @brief Удалить разработку
         */
        void removeResearch(const QModelIndex& _index);

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
    };
}

#endif // RESEARCHMANAGER_H

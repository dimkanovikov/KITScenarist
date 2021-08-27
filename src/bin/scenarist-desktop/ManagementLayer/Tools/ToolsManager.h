#ifndef TOOLSMANAGER_H
#define TOOLSMANAGER_H

#include <QObject>
#include <QFuture>

namespace BusinessLogic {
    struct BackupInfo;
    class RestoreFromBackupTool;
    class ScenarioDocument;
}

namespace UserInterface {
    class ToolsView;
}


namespace ManagementLayer
{
    /**
     * @brief Управляющий инструментами
     */
    class ToolsManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ToolsManager(QObject* _parent, QWidget* _parentWidget);

        QWidget* view() const;

        /**
         * @brief Подготовить инструменты для работы с текущим проектом
         */
        void loadCurrentProjectSettings();

        /**
         * @brief Перезагрузить параметры текстового редактора
         */
        void reloadTextEditSettings();

    signals:
        /**
         * @brief Пользователь хочет применить заданный xml
         */
        void applyScriptRequested(const QString& _xml);

    private:
        /**
         * @brief Загрузить список доступных бэкапов из файла
         */
        void loadBackupsList();

        /**
         * @brief Отобразить информацию о списке бэкапов
         */
        void showBackupInfo(const BusinessLogic::BackupInfo& _backupInfo);

        /**
         * @brief Загрузить выбранный пользователем бэкап
         */
        void loadBackup(const QModelIndex& _backupItemIndex);

        //

        /**
         * @brief Загрузить список версий сценария и отобразить
         */
        void loadScriptVersions();

        /**
         * @brief Сравнить две версии и отобразить результат сравнения
         */
        void compareVersions(int firstVersionIndex, int secondVersionIndex);

        //

        /**
         * @brief Отобразить сценарий
         */
        void showScript(const QString& _script);

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
         * @brief Представление страницы инструментов
         */
        UserInterface::ToolsView* m_view = nullptr;

        /**
         * @brief Документ сценария
         */
        BusinessLogic::ScenarioDocument* m_script = nullptr;

        /**
         * @brief Инструмент восстановления из бэкапа
         */
        BusinessLogic::RestoreFromBackupTool* m_restoreFromBackupTool = nullptr;

        /**
         * @brief Состояние выполнения операции загрузки сценария
         */
        QFuture<void> m_backupLoadingProgress;
    };
}

#endif // TOOLSMANAGER_H

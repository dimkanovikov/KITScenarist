#include "ToolsManager.h"

#include <Domain/Scenario.h>

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/Tools/RestoreFromBackupTool.h>

#include <ManagementLayer/Project/ProjectsManager.h>

#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <UserInterfaceLayer/Tools/ToolsView.h>

#include <QStandardItemModel>
#include <QtConcurrent>

using BusinessLogic::RestoreFromBackupTool;
using BusinessLogic::ScenarioDocument;
using ManagementLayer::ToolsManager;
using UserInterface::ToolsView;

namespace {
    /**
     * @brief Виды инструментов
     */
    enum class Tool {
        /**
         * @brief Сравнение версий сценариев
         */
        CompareScript,

        /**
         * @brief Восстановление сценария из бэкапа
         */
        RestoreFromBackup
    };

    /**
     * @brief Формат времени создания бэкапа
     */
    const QString kBackupDateTimeFormat = "dd.MM.yyyy hh:mm:ss";
}


ToolsManager::ToolsManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new ToolsView(_parentWidget)),
    m_script(new ScenarioDocument(this)),
    m_restoreFromBackupTool(new RestoreFromBackupTool(this))
{
    initView();
    initConnections();
}

QWidget* ToolsManager::view() const
{
    return m_view;
}

void ToolsManager::loadBackupsList()
{
    m_view->showPlaceholderText(tr("Choose backup from list"));

    const auto& project = ManagementLayer::ProjectsManager::currentProject();
    const QString saveBackupsFolder =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/save-backups-folder",
                DataStorageLayer::SettingsStorage::ApplicationSettings);
    const QString backupFilePath = project.versionsBackupFileName(saveBackupsFolder);
    QtConcurrent::run(m_restoreFromBackupTool, &RestoreFromBackupTool::loadBackupInfo, backupFilePath);
}

void ToolsManager::showBackupInfo(const BusinessLogic::BackupInfo& _backupInfo)
{
    QStandardItemModel* model = new QStandardItemModel(m_view);
    for (const auto& backup : _backupInfo.versions) {
        QStandardItem* item = new QStandardItem(backup.datetime.toString(kBackupDateTimeFormat));
        model->appendRow(item);
    }
    m_view->setBackupsModel(model);
}

void ToolsManager::loadBackup(const QModelIndex& _backupItemIndex)
{
    const QString backupDateTimeText = _backupItemIndex.data().toString();
    const QDateTime backupDateTime = QDateTime::fromString(backupDateTimeText, kBackupDateTimeFormat);
    QtConcurrent::run(m_restoreFromBackupTool, &RestoreFromBackupTool::loadBackup, backupDateTime);
}

void ToolsManager::showBackup(const QString& _backup)
{
    Domain::Scenario* scenario = m_script->scenario();
    if (scenario == nullptr) {
        scenario = new Domain::Scenario(Domain::Identifier(), QString(), QString(), false);
    }
    scenario->setText(_backup);
    m_script->load(scenario);

    m_view->showScript();
}

void ToolsManager::initView()
{
    m_view->setScriptDocument(m_script->document());
}

void ToolsManager::initConnections()
{
    connect(m_view, &ToolsView::dataRequested, [this] (int _toolType) {
        switch (static_cast<Tool>(_toolType)) {
            case Tool::CompareScript:
            {
                break;
            }

            case Tool::RestoreFromBackup:
            {
                loadBackupsList();
                break;
            }

            default: break;
        }
    });
    connect(m_view, &ToolsView::backupSelected, this, &ToolsManager::loadBackup);

    connect(m_restoreFromBackupTool, &RestoreFromBackupTool::backupInfoLoaded, this, &ToolsManager::showBackupInfo);
    connect(m_restoreFromBackupTool, &RestoreFromBackupTool::backupLoaded, this, &ToolsManager::showBackup);
}

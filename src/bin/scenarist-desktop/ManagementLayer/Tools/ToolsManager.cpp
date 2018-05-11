#include "ToolsManager.h"

#include <Domain/Scenario.h>

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/Tools/RestoreFromBackupTool.h>

#include <ManagementLayer/Project/ProjectsManager.h>

#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <UserInterfaceLayer/Tools/ToolsView.h>

#include <QStandardItemModel>
#include <QtConcurrent>

using BusinessLogic::RestoreFromBackupTool;
using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioDocument;
using BusinessLogic::ScenarioTemplateFacade;
using ManagementLayer::ToolsManager;
using UserInterface::ToolsView;

namespace {
    /**
     * @brief Виды инструментов
     */
    enum class Tool {
        /**
         * @brief Восстановление сценария из бэкапа
         */
        RestoreFromBackup,

        /**
         * @brief Сравнение версий сценариев
         */
        CompareScript
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

void ToolsManager::loadCurrentProjectSettings()
{
    m_view->reset();
}

void ToolsManager::reloadTextEditSettings()
{
    m_view->setShowScenesNumbers(
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "scenario-editor/show-scenes-numbers",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt());
    m_view->setShowDialoguesNumbers(
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "scenario-editor/show-dialogues-numbers",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt());

    //
    // Цветовая схема
    //
    const bool useDarkTheme =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/use-dark-theme",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    const QString colorSuffix = useDarkTheme ? "-dark" : "";
    m_view->setTextEditColors(
                QColor(
                    DataStorageLayer::StorageFacade::settingsStorage()->value(
                        "scenario-editor/text-color" + colorSuffix,
                        DataStorageLayer::SettingsStorage::ApplicationSettings)
                    ),
                QColor(
                    DataStorageLayer::StorageFacade::settingsStorage()->value(
                        "scenario-editor/background-color" + colorSuffix,
                        DataStorageLayer::SettingsStorage::ApplicationSettings)
                    )
                );

    //
    // Умолчальный шрифт документа
    //
    m_script->document()->setDefaultFont(
        ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::Action).font());
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
        const QString backupDateTime = backup.datetime.toString(kBackupDateTimeFormat);
        const QString backupName = QString("%1 [%2 %3]").arg(backupDateTime).arg(backup.size).arg(tr("Bytes"));
        QStandardItem* item = new QStandardItem(backupName);
        item->setData(backupDateTime, Qt::WhatsThisRole);
        model->appendRow(item);
    }
    m_view->setBackupsModel(model);
}

void ToolsManager::loadBackup(const QModelIndex& _backupItemIndex)
{
    const QString backupDateTimeText = _backupItemIndex.data(Qt::WhatsThisRole).toString();
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
    reloadTextEditSettings();
}

void ToolsManager::initView()
{
    m_view->setScriptDocument(m_script->document());
}

void ToolsManager::initConnections()
{
    connect(m_view, &ToolsView::dataRequested, [this] (int _toolType) {
        switch (static_cast<Tool>(_toolType)) {
            case Tool::RestoreFromBackup:
            {
                loadBackupsList();
                break;
            }

            case Tool::CompareScript:
            {
                break;
            }

            default: break;
        }
    });
    connect(m_view, &ToolsView::backupSelected, this, &ToolsManager::loadBackup);
    connect(m_view, &ToolsView::applyScriptRequested, this, [this] { emit applyScriptRequested(m_script->save()); });

    connect(m_restoreFromBackupTool, &RestoreFromBackupTool::backupInfoLoaded, this, &ToolsManager::showBackupInfo);
    connect(m_restoreFromBackupTool, &RestoreFromBackupTool::backupInfoNotLoaded, m_view, [this] { m_view->setBackupsModel(nullptr); });
    connect(m_restoreFromBackupTool, &RestoreFromBackupTool::backupLoaded, this, &ToolsManager::showBackup);
}

#include "AddProjectDialog.h"
#include "ui_AddProjectDialog.h"

#include <BusinessLayer/Import/AbstractImporter.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

using UserInterface::AddProjectDialog;

namespace {
    /**
     * @brief Расширения файлов проекта
     */
    const QString PROJECT_FILE_EXTENSION = ".kitsp"; // kit scenarist project

    /**
     * @brief Получить путь к папке из настроек по заданному ключу
     */
    static QString folderPath(const QString& _key) {
        QString folderPath =
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    _key,
                    DataStorageLayer::SettingsStorage::ApplicationSettings);
        if (folderPath.isEmpty()) {
            folderPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        }
        return folderPath;
    }

    /**
     * @brief Сохранить путь к папке с заданным ключём
     */
    static void saveFolderPath(const QString& _key, const QString& _path) {
        DataStorageLayer::StorageFacade::settingsStorage()->setValue(
            _key,
            QFileInfo(_path).absoluteDir().absolutePath(),
            DataStorageLayer::SettingsStorage::ApplicationSettings);
    }

    /**
     * @brief Получить путь к папке с проектами
     */
    static QString projectsFolderPath() {
        return folderPath("application/project-files");
    }

    /**
     * @brief Сохранить путь к папке с проектами
     */
    static void saveProjectsFolderPath(const QString& _path) {
        saveFolderPath("application/project-files", _path);
    }

    /**
     * @brief Получить путь к папке импортируемых файлов
     */
    static QString importFolderPath() {
        return folderPath("import/file-path");
    }

    /**
     * @brief Сохранить путь к папке импортируемых файлов
     */
    static void saveImportFolderPath(const QString& _path) {
        saveFolderPath("import/file-path", _path);
    }
}


AddProjectDialog::AddProjectDialog(QWidget* _parent, bool _hasInternetConnection) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::AddProjectDialog),
    m_hasInternetConnection(_hasInternetConnection)
{
    m_ui->setupUi(this);

    initStyleSheet();
}

AddProjectDialog::~AddProjectDialog()
{
    delete m_ui;
}

void AddProjectDialog::setIsRemoteAvailable(bool _isAvailable, bool _isEnabled, bool _isSelected)
{
    m_ui->cloudBlocker->hide();

    if (_isAvailable) {
        m_ui->isRemote->setEnabled(_isEnabled);
        m_ui->isRemote->setChecked(_isSelected);

        //
        // Покажем сообщение о невозможности создания проекта в облаке при отсутствии подключения
        //
        const bool cantCreateInCloud = _isSelected && !m_hasInternetConnection;
        setCloudCreateBlockerVisible(cantCreateInCloud);
    } else {
        m_ui->isLocal->hide();
        m_ui->isRemote->hide();
    }
}

bool AddProjectDialog::isLocal() const
{
    return m_ui->isLocal->isChecked();
}

QString AddProjectDialog::projectName() const
{
    return m_ui->projectName->text();
}

QString AddProjectDialog::projectFilePath() const
{
    return m_ui->saveDir->text() + QDir::separator() + projectName() + PROJECT_FILE_EXTENSION;
}

QString AddProjectDialog::importFilePath() const
{
    if (!m_ui->importFile->text().isEmpty()) {
        return m_ui->importFile->text();
    }

    return QString::null;
}

QWidget* AddProjectDialog::focusedOnExec() const
{
    return m_ui->projectName;
}

void AddProjectDialog::setCloudCreateBlockerVisible(bool _visible)
{
    m_ui->cloudBlocker->setVisible(_visible);
    m_ui->namePanel->setVisible(!_visible);
    m_ui->advanced->setVisible(!_visible);
    if (m_ui->advanced->isChecked()) {
        m_ui->advancedPanel->setVisible(!_visible);
    }
    for (auto* button : m_ui->buttons->buttons()) {
        if (m_ui->buttons->buttonRole(button) == QDialogButtonBox::AcceptRole) {
            button->setEnabled(!_visible);
        }
    }
}

void AddProjectDialog::initView()
{
    m_ui->browseSaveDir->updateIcons();
    m_ui->browseImportFile->updateIcons();

    m_ui->advancedPanel->hide();
    m_ui->saveDir->setText(QDir::toNativeSeparators(::projectsFolderPath()));

    m_ui->existsLabel->hide();

    m_ui->buttons->addButton(tr("Create"), QDialogButtonBox::AcceptRole);

    //
    // Предустановить название проекта по-умолчанию
    //
    const QString projectName = tr("New Project");
    m_ui->projectName->setText(projectName);
    if (m_ui->isLocal->isChecked()) {
        int projectCopy = 0;
        while (QFile::exists(projectFilePath())) {
            ++projectCopy;
            m_ui->projectName->setText(QString("%1 (%2)").arg(projectName).arg(projectCopy));
        }
    }
    m_ui->projectName->selectAll();
}

void AddProjectDialog::initConnections()
{
    //
    // Настроим видимость возможности выбора папки сохранения файла
    //
    connect(m_ui->isLocal, &QRadioButton::toggled, [=] {
        const bool isLocal = m_ui->isLocal->isChecked();
        m_ui->saveDirLabel->setVisible(isLocal);
        m_ui->saveDir->setVisible(isLocal);
        m_ui->browseSaveDir->setVisible(isLocal);

        //
        // Покажем сообщение о невозможности создания проекта в облаке при отсутствии подключения
        //
        const bool cantCreateInCloud = !isLocal && !m_hasInternetConnection;
        setCloudCreateBlockerVisible(cantCreateInCloud);
    });

    //
    // Проверим не существует ли уже такого файла
    //
    connect(m_ui->projectName, &QLineEdit::textChanged, [=] {
        m_ui->existsLabel->setVisible(QFile::exists(projectFilePath()));
    });

    connect(m_ui->advanced, &QCheckBox::toggled, m_ui->advancedPanel, &QFrame::setVisible);

    connect(m_ui->browseSaveDir, &FlatButton::clicked, [=] {
        QString folderPath =
                QFileDialog::getExistingDirectory(
                    this,
                    tr("Choose file for new project"),
                    ::projectsFolderPath()
                    );

        if (!folderPath.isEmpty()) {
            //
            // Сохраним путь к файлу
            //
            m_ui->saveDir->setText(folderPath);
            ::saveProjectsFolderPath(folderPath);
        }
    });
    connect(m_ui->browseImportFile, &FlatButton::clicked, [=] {
        QString filePath =
                QFileDialog::getOpenFileName(this, tr("Choose file to import"),
                    ::importFolderPath(), BusinessLogic::AbstractImporter::filters());

        if (!filePath.isEmpty()) {
            //
            // Сохраним путь к файлу
            //
            m_ui->importFile->setText(filePath);
            ::saveImportFolderPath(filePath);
        }
    });

    connect(m_ui->buttons, &QDialogButtonBox::accepted, this, &AddProjectDialog::accept);
    connect(m_ui->buttons, &QDialogButtonBox::rejected, this, &AddProjectDialog::reject);
}

void AddProjectDialog::initStyleSheet()
{
    m_ui->browseSaveDir->setProperty("isBrowseButton", true);
    m_ui->browseImportFile->setProperty("isBrowseButton", true);
}

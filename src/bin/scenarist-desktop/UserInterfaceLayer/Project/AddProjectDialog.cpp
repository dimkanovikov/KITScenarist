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


AddProjectDialog::AddProjectDialog(QWidget* _parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::AddProjectDialog)
{
    m_ui->setupUi(this);
}

AddProjectDialog::~AddProjectDialog()
{
    delete m_ui;
}

void AddProjectDialog::setIsRemoteAvailable(bool _isAvailable, bool _isEnabled, bool _isSelected)
{
    if (_isAvailable) {
        m_ui->isRemote->setEnabled(_isEnabled);
        m_ui->isRemote->setChecked(_isSelected);
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

void AddProjectDialog::initView()
{
    m_ui->browseSaveDir->updateIcons();
    m_ui->browseImportFile->updateIcons();

    m_ui->advancedPanel->hide();
    m_ui->saveDir->setText(QDir::toNativeSeparators(::projectsFolderPath()));

    m_ui->existsLabel->hide();

    m_ui->buttons->addButton(tr("Create"), QDialogButtonBox::AcceptRole);
}

void AddProjectDialog::initConnections()
{
    //
    // Настроим видимость возможности выбора папки сохранения файла
    //
    connect(m_ui->isLocal, &QRadioButton::toggled, [=] {
        const bool  visible = m_ui->isLocal->isChecked();
        m_ui->saveDirLabel->setVisible(visible);
        m_ui->saveDir->setVisible(visible);
        m_ui->browseSaveDir->setVisible(visible);
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

#include "ImportDialog.h"
#include "ui_ImportDialog.h"

#include <format_manager.h>

#include <BusinessLayer/Import/AbstractImporter.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QPushButton>
#include <QStandardPaths>

using UserInterface::ImportDialog;

namespace {
    /**
     * @brief Получить путь к папке импортируемых файлов
     */
    static QString importFolderPath() {
        QString importFolderPath =
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "import/file-path",
                    DataStorageLayer::SettingsStorage::ApplicationSettings);
        if (importFolderPath.isEmpty()) {
            importFolderPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        }
        return importFolderPath;
    }

    /**
     * @brief Сохранить путь к папке импортируемых файлов
     */
    static void saveImportFolderPath(const QString& _path) {
        DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                    "import/file-path",
                    QFileInfo(_path).absoluteDir().absolutePath(),
                    DataStorageLayer::SettingsStorage::ApplicationSettings);
    }
}


ImportDialog::ImportDialog(QWidget *parent) :
    QLightBoxDialog(parent),
    m_ui(new Ui::ImportDialog)
{
    m_ui->setupUi(this);

    m_import = m_ui->buttons->addButton(tr("Import"), QDialogButtonBox::AcceptRole);
    initStyleSheet();
}

ImportDialog::~ImportDialog()
{
    delete m_ui;
}

BusinessLogic::ImportParameters ImportDialog::importParameters() const
{
    BusinessLogic::ImportParameters importParameters;
    importParameters.outline = m_ui->outline->isChecked();
    importParameters.filePath = m_ui->file->text();
    importParameters.removeScenesNumbers = m_ui->removeScenesNumbers->isChecked();
    if (m_ui->replaceScenario->isChecked()) {
        importParameters.insertionMode = BusinessLogic::ImportParameters::ReplaceDocument;
    } else if (m_ui->currentCursorPosition->isChecked()) {
        importParameters.insertionMode = BusinessLogic::ImportParameters::ToCursorPosition;
    } else {
        importParameters.insertionMode = BusinessLogic::ImportParameters::ToDocumentEnd;
    }
    importParameters.findCharactersAndLocations = m_ui->findCharactersAndLocations->isChecked();
    importParameters.saveReviewMarks = m_ui->saveReviewMarks->isChecked();

    return importParameters;
}

void ImportDialog::aboutChooseFile()
{
    QString filePath =
            QFileDialog::getOpenFileName(this, tr("Choose file to import"), ::importFolderPath(),
                                         BusinessLogic::AbstractImporter::filters());

    if (!filePath.isEmpty()) {
        //
        // Сохраним путь к файлу
        //
        m_ui->file->setText(filePath);
        ::saveImportFolderPath(filePath);
    }
}

void ImportDialog::aboutFileNameChanged()
{
    m_import->setEnabled(!m_ui->file->text().isEmpty());
}

void ImportDialog::initView()
{
    m_ui->browseFile->updateIcons();

    m_import->setEnabled(false);
}

void ImportDialog::initConnections()
{
    connect(m_ui->browseFile, SIGNAL(clicked()), this, SLOT(aboutChooseFile()));
    connect(m_ui->file, SIGNAL(textChanged(QString)), this, SLOT(aboutFileNameChanged()));

    connect(m_ui->buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_ui->buttons, SIGNAL(accepted()), this, SLOT(accept()));
}

void ImportDialog::initStyleSheet()
{
    m_ui->browseFile->setProperty("isBrowseButton", true);
}

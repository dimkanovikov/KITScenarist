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
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    m_import = ui->buttons->addButton(tr("Import"), QDialogButtonBox::AcceptRole);
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

BusinessLogic::ImportParameters ImportDialog::importParameters() const
{
    BusinessLogic::ImportParameters importParameters;
    importParameters.outline = ui->outline->isChecked();
    importParameters.filePath = ui->file->text();
    importParameters.removeScenesNumbers = ui->removeScenesNumbers->isChecked();
    if (ui->replaceScenario->isChecked()) {
        importParameters.insertionMode = BusinessLogic::ImportParameters::ReplaceDocument;
    } else if (ui->currentCursorPosition->isChecked()) {
        importParameters.insertionMode = BusinessLogic::ImportParameters::ToCursorPosition;
    } else {
        importParameters.insertionMode = BusinessLogic::ImportParameters::ToDocumentEnd;
    }
    importParameters.findCharactersAndLocations = ui->findCharactersAndLocations->isChecked();
    importParameters.saveReviewMarks = ui->saveReviewMarks->isChecked();

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
        ui->file->setText(filePath);
        ::saveImportFolderPath(filePath);
    }
}

void ImportDialog::aboutFileNameChanged()
{
    m_import->setEnabled(!ui->file->text().isEmpty());
}

void ImportDialog::initView()
{
    ui->browseFile->updateIcons();

    m_import->setEnabled(false);
}

void ImportDialog::initConnections()
{
    connect(ui->browseFile, SIGNAL(clicked()), this, SLOT(aboutChooseFile()));
    connect(ui->file, SIGNAL(textChanged(QString)), this, SLOT(aboutFileNameChanged()));

    connect(ui->buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->buttons, SIGNAL(accepted()), this, SLOT(accept()));
}

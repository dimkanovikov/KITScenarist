#include "ExportDialog.h"
#include "ui_ExportDialog.h"

#include <BusinessLayer/Export/AbstractExporter.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/FileHelper.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

using UserInterface::ExportDialog;

namespace {
    /**
     * @brief Получить путь к папке экспортируемых файлов
     */
    static QString exportFolderPath() {
        QString exportFolderPath =
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "export/file-path",
                    DataStorageLayer::SettingsStorage::ApplicationSettings);
        if (exportFolderPath.isEmpty()) {
            exportFolderPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        }
        return exportFolderPath;
    }

    /**
     * @brief Получить путь к экспортируемому файлу
     */
    static QString exportFilePath(const QString& _fileName) {
        QString filePath = exportFolderPath() + QDir::separator() + _fileName;
        return QDir::toNativeSeparators(filePath);
    }

    /**
     * @brief Сохранить путь к папке экспортируемых файлов
     */
    static void saveExportFolderPath(const QString& _path) {
        DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                    "export/file-path",
                    QFileInfo(_path).absoluteDir().absolutePath(),
                    DataStorageLayer::SettingsStorage::ApplicationSettings);
    }
}


ExportDialog::ExportDialog(QWidget* _parent) :
    QLightBoxDialog(_parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::setExportFilePath(const QString& _filePath)
{
    ui->file->setText(_filePath);
    QFileInfo fileInfo(_filePath);
    if (fileInfo.suffix() == "docx") {
        ui->docx->setChecked(true);
    } else if (fileInfo.suffix() == "pdf") {
        ui->pdf->setChecked(true);
    } else {
        ui->fdx->setChecked(true);
    }

    aboutFileNameChanged();
}

void ExportDialog::setExportFileName(const QString& _fileName)
{
    if (ui->file->text().isEmpty()
        && m_exportFileName != _fileName) {
        m_exportFileName = _fileName;
        ui->file->setText(::exportFilePath(_fileName));
        aboutFormatChanged();
    }

    aboutFileNameChanged();
}

void ExportDialog::setCheckPageBreaks(bool _check)
{
    ui->checkPageBreaks->setChecked(_check);
}

void ExportDialog::setStylesModel(QAbstractItemModel* _model)
{
    ui->styles->setModel(_model);
}

void ExportDialog::setCurrentStyle(const QString& _styleName)
{
    ui->styles->setCurrentText(_styleName);
}

void ExportDialog::setPageNumbering(bool _isChecked)
{
    ui->pageNumbering->setChecked(_isChecked);
}

void ExportDialog::setScenesNumbering(bool _isChecked)
{
    ui->scenesNumbering->setChecked(_isChecked);
}

void ExportDialog::setScenesPrefix(const QString& _prefix)
{
    ui->scenesPrefix->setText(_prefix);
}

void ExportDialog::setSaveReviewMarks(bool _save)
{
    ui->saveReviewMarks->setChecked(_save);
}

void ExportDialog::setPrintTitle(bool _isChecked)
{
    ui->printTitle->setChecked(_isChecked);
}

BusinessLogic::ExportParameters ExportDialog::exportParameters() const
{
    BusinessLogic::ExportParameters exportParameters;
    exportParameters.outline = ui->outline->isChecked();
    exportParameters.filePath = ui->file->text();
    exportParameters.checkPageBreaks = ui->checkPageBreaks->isChecked();
    exportParameters.style = ui->styles->currentText();
    exportParameters.printTilte = ui->printTitle->isChecked();
    exportParameters.printPagesNumbers = ui->pageNumbering->isChecked();
    exportParameters.printScenesNumbers = ui->scenesNumbering->isChecked();
    exportParameters.scenesPrefix = ui->scenesPrefix->text();
    exportParameters.saveReviewMarks = ui->saveReviewMarks->isChecked();

    return exportParameters;
}

void ExportDialog::aboutFormatChanged()
{
    QString format;
    if (ui->docx->isChecked()) {
        format = "docx";
    } else if (ui->pdf->isChecked()) {
        format = "pdf";
    } else {
        format = "fdx";
    }
    QString filePath = ui->file->text();

    //
    // Обновить имя файла, если оно уже задано в другом формате
    //
    if (!filePath.isEmpty()
        && !filePath.endsWith(format)) {
        QFileInfo fileInfo(filePath);
        //
        // Если у файла есть расширение
        //
        if (!fileInfo.suffix().isEmpty()) {
            filePath.replace(fileInfo.suffix(), format);
        } else {
            filePath.append("." + format);
        }

        ui->file->setText(filePath);
    }
}

void ExportDialog::aboutChooseFile()
{
    QString format;
    if (ui->docx->isChecked()) {
        format = "docx";
    } else if (ui->pdf->isChecked()) {
        format = "pdf";
    } else {
        format = "fdx";
    }
    QString filePath =
            QFileDialog::getSaveFileName(this, tr("Choose file to export scenario"),
                (!ui->file->text().isEmpty() ? ui->file->text() : ::exportFolderPath()),
                tr("%1 files (*%2)").arg(format.toUpper()).arg(format));

    if (!filePath.isEmpty()) {
        //
        // Сохраним путь к файлу
        //
        ui->file->setText(filePath);
        ::saveExportFolderPath(filePath);

        //
        // Обновим расширение файла
        //
        aboutFormatChanged();
    }
}

void ExportDialog::aboutFileNameChanged()
{
    int lastCursorPosition = ui->file->cursorPosition();
    ui->file->setText(FileHelper::systemSavebleFileName(ui->file->text()));
    ui->file->setCursorPosition(lastCursorPosition);

    ui->exportTo->setEnabled(!ui->file->text().isEmpty());
    ui->existsLabel->setVisible(QFile::exists(ui->file->text()));
}

void ExportDialog::initView()
{
    ui->browseFile->updateIcons();

    ui->additionalSettings->hide();

    resize(width(), sizeHint().height());

    QLightBoxDialog::initView();
}

void ExportDialog::initConnections()
{
    connect(ui->showAdditional, SIGNAL(toggled(bool)), ui->additionalSettings, SLOT(setVisible(bool)));

    connect(ui->styles, SIGNAL(currentTextChanged(QString)), this, SIGNAL(currentStyleChanged(QString)));
    connect(ui->docx, &QRadioButton::toggled, this, &ExportDialog::aboutFormatChanged);
    connect(ui->pdf, &QRadioButton::toggled, this, &ExportDialog::aboutFormatChanged);
    connect(ui->fdx, &QRadioButton::toggled, this, &ExportDialog::aboutFormatChanged);
    connect(ui->browseFile, SIGNAL(clicked()), this, SLOT(aboutChooseFile()));
    connect(ui->file, SIGNAL(textChanged(QString)), this, SLOT(aboutFileNameChanged()));

    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->printPreview, SIGNAL(clicked()), this, SIGNAL(printPreview()));
    connect(ui->exportTo, SIGNAL(clicked()), this, SLOT(accept()));

    QLightBoxDialog::initConnections();
}

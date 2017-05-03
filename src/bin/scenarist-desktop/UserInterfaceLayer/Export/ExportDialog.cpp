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
    m_ui(new Ui::ExportDialog)
{
    m_ui->setupUi(this);
}

ExportDialog::~ExportDialog()
{
    delete m_ui;
}

void ExportDialog::setExportFilePath(const QString& _filePath)
{
    m_ui->file->setText(_filePath);
    QFileInfo fileInfo(_filePath);
    if (fileInfo.suffix() == "docx") {
        m_ui->docx->setChecked(true);
    } else if (fileInfo.suffix() == "pdf") {
        m_ui->pdf->setChecked(true);
    } else {
        m_ui->fdx->setChecked(true);
    }

    aboutFileNameChanged();
}

void ExportDialog::setExportFileName(const QString& _fileName)
{
    if (m_ui->file->text().isEmpty()
        && m_exportFileName != _fileName) {
        m_exportFileName = _fileName;
        m_ui->file->setText(::exportFilePath(_fileName));
        aboutFormatChanged();
    }

    aboutFileNameChanged();
}

void ExportDialog::setCheckPageBreaks(bool _check)
{
    m_ui->checkPageBreaks->setChecked(_check);
}

void ExportDialog::setStylesModel(QAbstractItemModel* _model)
{
    m_ui->styles->setModel(_model);
}

void ExportDialog::setCurrentStyle(const QString& _styleName)
{
    m_ui->styles->setCurrentText(_styleName);
}

void ExportDialog::setPageNumbering(bool _isChecked)
{
    m_ui->pageNumbering->setChecked(_isChecked);
}

void ExportDialog::setScenesNumbering(bool _isChecked)
{
    m_ui->scenesNumbering->setChecked(_isChecked);
}

void ExportDialog::setScenesPrefix(const QString& _prefix)
{
    m_ui->scenesPrefix->setText(_prefix);
}

void ExportDialog::setSaveReviewMarks(bool _save)
{
    m_ui->saveReviewMarks->setChecked(_save);
}

void ExportDialog::setPrintTitle(bool _isChecked)
{
    m_ui->printTitle->setChecked(_isChecked);
}

BusinessLogic::ExportParameters ExportDialog::exportParameters() const
{
    BusinessLogic::ExportParameters exportParameters;
    exportParameters.outline = m_ui->outline->isChecked();
    exportParameters.filePath = m_ui->file->text();
    exportParameters.checkPageBreaks = m_ui->checkPageBreaks->isChecked();
    exportParameters.style = m_ui->styles->currentText();
    exportParameters.printTilte = m_ui->printTitle->isChecked();
    exportParameters.printPagesNumbers = m_ui->pageNumbering->isChecked();
    exportParameters.printScenesNumbers = m_ui->scenesNumbering->isChecked();
    exportParameters.scenesPrefix = m_ui->scenesPrefix->text();
    exportParameters.saveReviewMarks = m_ui->saveReviewMarks->isChecked();

    return exportParameters;
}

void ExportDialog::aboutFormatChanged()
{
    QString format;
    if (m_ui->docx->isChecked()) {
        format = "docx";
    } else if (m_ui->pdf->isChecked()) {
        format = "pdf";
    } else if (ui->fdx->isChecked()) {
        format = "fdx";
    } else {
        format = "fountain";
    }
    QString filePath = m_ui->file->text();

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

        m_ui->file->setText(filePath);
    }
}

void ExportDialog::aboutChooseFile()
{
    QString format;
    if (m_ui->docx->isChecked()) {
        format = "docx";
    } else if (m_ui->pdf->isChecked()) {
        format = "pdf";
    } else if (ui->fdx->isChecked()) {
        format = "fdx";
    } else {
        format = "fountain";
    }
    QString filePath =
            QFileDialog::getSaveFileName(this, tr("Choose file to export scenario"),
                (!m_ui->file->text().isEmpty() ? m_ui->file->text() : ::exportFolderPath()),
                tr("%1 files (*%2)").arg(format.toUpper()).arg(format));

    if (!filePath.isEmpty()) {
        //
        // Сохраним путь к файлу
        //
        m_ui->file->setText(filePath);
        ::saveExportFolderPath(filePath);

        //
        // Обновим расширение файла
        //
        aboutFormatChanged();
    }
}

void ExportDialog::aboutFileNameChanged()
{
    int lastCursorPosition = m_ui->file->cursorPosition();
    m_ui->file->setText(FileHelper::systemSavebleFileName(m_ui->file->text()));
    m_ui->file->setCursorPosition(lastCursorPosition);

    m_ui->exportTo->setEnabled(!m_ui->file->text().isEmpty());
    m_ui->existsLabel->setVisible(QFile::exists(m_ui->file->text()));
}

void ExportDialog::initView()
{
    m_ui->browseFile->updateIcons();

    m_ui->additionalSettings->setVisible(m_ui->showAdditional->isChecked());

    resize(width(), sizeHint().height());
}

void ExportDialog::initConnections()
{
    connect(m_ui->showAdditional, SIGNAL(toggled(bool)), m_ui->additionalSettings, SLOT(setVisible(bool)));

    connect(m_ui->styles, SIGNAL(currentTextChanged(QString)), this, SIGNAL(currentStyleChanged(QString)));
    connect(m_ui->docx, &QRadioButton::toggled, this, &ExportDialog::aboutFormatChanged);
    connect(m_ui->pdf, &QRadioButton::toggled, this, &ExportDialog::aboutFormatChanged);
    connect(m_ui->fdx, &QRadioButton::toggled, this, &ExportDialog::aboutFormatChanged);
    connect(m_ui->browseFile, SIGNAL(clicked()), this, SLOT(aboutChooseFile()));
    connect(m_ui->file, SIGNAL(textChanged(QString)), this, SLOT(aboutFileNameChanged()));

    connect(m_ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_ui->printPreview, SIGNAL(clicked()), this, SIGNAL(printPreview()));
    connect(m_ui->exportTo, SIGNAL(clicked()), this, SLOT(accept()));
}

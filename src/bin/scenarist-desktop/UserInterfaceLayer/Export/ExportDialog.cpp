#include "ExportDialog.h"
#include "ui_ExportDialog.h"

#include <BusinessLayer/Export/AbstractExporter.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/FileHelper.h>
#include <3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

using UserInterface::ExportDialog;

namespace {
    /**
     * @brief Ключ для доступа к папке с экспортируемыми документами
     */
    const QString EXPORT_FOLDER_KEY = "export/file-path";

    /**
     * @brief Индексы вкладок табов для экспорта
     */
    /** @{ */
    const int RESEARCH_TAB_INDEX = 0;
    const int OUTLINE_TAB_INDEX = 1;
    const int SCRIPT_TAB_INDEX = 1;
    /** @} */
}


ExportDialog::ExportDialog(QWidget* _parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::ExportDialog)
{
    m_ui->setupUi(this);

    initStyleSheet();
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
        const QString exportFilePath =
                DataStorageLayer::StorageFacade::settingsStorage()->documentFilePath(EXPORT_FOLDER_KEY, _fileName);
        m_ui->file->setText(exportFilePath);
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
    exportParameters.outline = m_ui->exportTypes->currentIndex() == OUTLINE_TAB_INDEX;
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

QString ExportDialog::exportFormat() const
{
    QString format;
    if (m_ui->docx->isChecked()) {
        format = "docx";
    } else if (m_ui->pdf->isChecked()) {
        format = "pdf";
    } else if (m_ui->fdx->isChecked()) {
        format = "fdx";
    } else {
        format = "fountain";
    }
    return format;
}

void ExportDialog::setResearchModel(QAbstractItemModel* _model)
{
    m_ui->researchExportTree->setModel(_model);
}

void ExportDialog::aboutFormatChanged()
{
    const QString format = exportFormat();
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
    } else if (m_ui->fdx->isChecked()) {
        format = "fdx";
    } else {
        format = "fountain";
    }
    const QString exportFolderPath =
            DataStorageLayer::StorageFacade::settingsStorage()->documentFolderPath(EXPORT_FOLDER_KEY);
    QString filePath =
            QFileDialog::getSaveFileName(this, tr("Choose file to export scenario"),
                (!m_ui->file->text().isEmpty() ? m_ui->file->text() : exportFolderPath),
                tr("%1 files (*%2)").arg(format.toUpper()).arg(format));

    if (!filePath.isEmpty()) {
        //
        // Сохраним путь к файлу
        //
        m_ui->file->setText(filePath);
        DataStorageLayer::StorageFacade::settingsStorage()->saveDocumentFolderPath(EXPORT_FOLDER_KEY, filePath);

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
    m_ui->exportParameters->setCurrentWidget(m_ui->researchExportPage);

    m_ui->exportTypes->addTab(tr("Export research"));
    m_ui->exportTypes->addTab(tr("Export outline"));
    m_ui->exportTypes->addTab(tr("Export script"));

    m_ui->researchExportTree->setItemDelegate(new TreeViewItemDelegate(m_ui->researchExportTree));

    m_ui->browseFile->updateIcons();
    QButtonGroup* scriptFormatTypeGroup = new QButtonGroup(this);
    scriptFormatTypeGroup->addButton(m_ui->docx);
    scriptFormatTypeGroup->addButton(m_ui->pdf);
    scriptFormatTypeGroup->addButton(m_ui->fdx);
    scriptFormatTypeGroup->addButton(m_ui->fountain);
    m_ui->additionalSettings->setVisible(m_ui->showAdditional->isChecked());

    m_ui->researchBrowseFile->updateIcons();

    resize(width(), sizeHint().height());
}

void ExportDialog::initConnections()
{
    //
    // Покажем страницу параметров экспорта в зависимости от выбранной вкладки
    //
    connect(m_ui->exportTypes, &TabBarExpanded::currentChanged, [this] (int _index) {
        if (_index == RESEARCH_TAB_INDEX) {
            m_ui->exportParameters->setCurrentWidget(m_ui->researchExportPage);
        } else {
            m_ui->exportParameters->setCurrentWidget(m_ui->scriptExportPage);
        }
    });

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

void ExportDialog::initStyleSheet()
{
    m_ui->toolbarLeftEmptyLabel->setProperty("inTopPanel", true);
    m_ui->toolbarLeftEmptyLabel->setProperty("topPanelTopBordered", true);
    m_ui->toolbarLeftEmptyLabel->setProperty("topPanelLeftBordered", true);
    m_ui->exportTypes->setProperty("inTopPanel", true);
    m_ui->toolbarRightEmptyLabel->setProperty("inTopPanel", true);
    m_ui->toolbarRightEmptyLabel->setProperty("topPanelTopBordered", true);
    m_ui->toolbarRightEmptyLabel->setProperty("topPanelRightBordered", true);
    m_ui->browseFile->setProperty("isBrowseButton", true);
    m_ui->researchBrowseFile->setProperty("isBrowseButton", true);
}

QWidget* ExportDialog::titleWidget() const
{
    return m_ui->toolbar;
}

#include "ExportDialog.h"
#include "ui_ExportDialog.h"

#include <BusinessLayer/Export/AbstractExporter.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

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
		QString filePath = ::exportFolderPath() + QDir::separator() + _fileName;
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

	initView();
	initConnections();
}

ExportDialog::~ExportDialog()
{
	delete ui;
}

void ExportDialog::setExportFilePath(const QString& _filePath)
{
	QString filePath = _filePath;
	if (filePath.endsWith("rtf")) {
		filePath = filePath.replace(".rtf", ".docx");
	}

	ui->file->setText(filePath);
	QFileInfo fileInfo(filePath);
	if (fileInfo.suffix() == "docx") {
		ui->docx->setChecked(true);
	} else {
		ui->pdf->setChecked(true);
	}
}

void ExportDialog::setExportFileName(const QString& _fileName)
{
	if (ui->file->text().isEmpty()
		&& m_exportFileName != _fileName) {
		m_exportFileName = _fileName;
		ui->file->setText(::exportFilePath(_fileName));
		aboutFormatChanged();
	}
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

QString ExportDialog::scenarioName() const
{
	return ui->name->text();
}

void ExportDialog::setScenarioName(const QString& _name)
{
	ui->name->setText(_name);
}

QString ExportDialog::scenarioAdditionalInfo() const
{
	return ui->additionalInfo->currentText();
}

void ExportDialog::setScenarioAdditionalInfo(const QString& _additionalInfo)
{
	ui->additionalInfo->setEditText(_additionalInfo);
}

QString ExportDialog::scenarioGenre() const
{
	return ui->genre->text();
}

void ExportDialog::setScenarioGenre(const QString& _genre)
{
	ui->genre->setText(_genre);
}

QString ExportDialog::scenarioAuthor() const
{
	return ui->author->text();
}

void ExportDialog::setScenarioAuthor(const QString _author)
{
	ui->author->setText(_author);
}

QString ExportDialog::scenarioContacts() const
{
	return ui->contacts->toPlainText();
}

void ExportDialog::setScenarioContacts(const QString& _contacts)
{
	ui->contacts->setPlainText(_contacts);
}

QString ExportDialog::scenarioYear() const
{
	return ui->year->text();
}

void ExportDialog::setScenarioYear(const QString& _year)
{
	ui->year->setText(_year);
}

BusinessLogic::ExportParameters ExportDialog::exportParameters() const
{
	BusinessLogic::ExportParameters exportParameters;
	exportParameters.filePath = ui->file->text();
	exportParameters.checkPageBreaks = ui->checkPageBreaks->isChecked();
	exportParameters.style = ui->styles->currentText();
	exportParameters.printTilte = ui->printTitle->isChecked();
	exportParameters.scenarioName = ui->name->text();
	exportParameters.scenarioAdditionalInfo = ui->additionalInfo->currentText();
	exportParameters.scenarioGenre = ui->genre->text();
	exportParameters.scenarioAuthor = ui->author->text();
	exportParameters.scenarioContacts = ui->contacts->toPlainText();
	exportParameters.scenarioYear = ui->year->text();
	exportParameters.printPagesNumbers = ui->pageNumbering->isChecked();
	exportParameters.printScenesNumbers = ui->scenesNumbering->isChecked();
	exportParameters.scenesPrefix = ui->scenesPrefix->text();
	exportParameters.saveReviewMarks = ui->saveReviewMarks->isChecked();

	return exportParameters;
}

void ExportDialog::aboutFormatChanged()
{
	const QString format = ui->docx->isChecked() ? "docx" : "pdf";
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
	const QString format = ui->docx->isChecked() ? "docx" : "pdf";
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
	ui->exportTo->setEnabled(!ui->file->text().isEmpty());
}

void ExportDialog::initView()
{
	ui->additionalSettings->setCurrentWidget(ui->commonTab);
	ui->additionalSettings->hide();

	resize(width(), sizeHint().height());

	QLightBoxDialog::initView();
}

void ExportDialog::initConnections()
{
	connect(ui->showAdditional, SIGNAL(toggled(bool)), ui->additionalSettings, SLOT(setVisible(bool)));

	connect(ui->styles, SIGNAL(currentTextChanged(QString)), this, SIGNAL(currentStyleChanged(QString)));
	connect(ui->docx, SIGNAL(toggled(bool)), this, SLOT(aboutFormatChanged()));
	connect(ui->pdf, SIGNAL(toggled(bool)), this, SLOT(aboutFormatChanged()));
	connect(ui->browseFile, SIGNAL(clicked()), this, SLOT(aboutChooseFile()));
	connect(ui->file, SIGNAL(textChanged(QString)), this, SLOT(aboutFileNameChanged()));

	connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->printPreview, SIGNAL(clicked()), this, SIGNAL(printPreview()));
	connect(ui->exportTo, SIGNAL(clicked()), this, SLOT(accept()));

	QLightBoxDialog::initConnections();
}

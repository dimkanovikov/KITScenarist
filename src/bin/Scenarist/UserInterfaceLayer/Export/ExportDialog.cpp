#include "ExportDialog.h"
#include "ui_ExportDialog.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QFileDialog>
#include <QFileInfo>

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
			exportFolderPath = QDir::homePath();
		}
		return exportFolderPath;
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
	QDialog(_parent),
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

void ExportDialog::setStylesModel(QAbstractItemModel* _model)
{
	ui->styles->setModel(_model);
}

void ExportDialog::setCurrentStyle(const QString& _styleName)
{
	ui->styles->setCurrentText(_styleName);
}

QString ExportDialog::exportFilePath() const
{
	return ui->file->text();
}

bool ExportDialog::printTitle() const
{
	return ui->title->isChecked();
}

bool ExportDialog::printPagesNumbering() const
{
	return ui->pageNumbering->isChecked();
}

bool ExportDialog::printScenesNumbering() const
{
	return ui->scenesNumbering->isChecked();
}

QString ExportDialog::scenesPrefix() const
{
	return ui->scenesPrefix->text();
}

void ExportDialog::aboutChooseFile()
{
	const QString format = ui->rtf->isChecked() ? "rtf" : "pdf";
	QString filePath =
			QFileDialog::getSaveFileName(this, tr("Choose file to export scenario"),
				::exportFolderPath(), tr ("%1 files (*%2)").arg(format.toUpper()).arg(format));

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
	ui->label_5->hide();
	ui->pageNumbering->hide();
}

void ExportDialog::aboutFormatChanged()
{
	const QString format = ui->rtf->isChecked() ? "rtf" : "pdf";
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

void ExportDialog::initConnections()
{
	connect(ui->styles, SIGNAL(currentTextChanged(QString)), this, SIGNAL(currentStyleChanged(QString)));
	connect(ui->rtf, SIGNAL(toggled(bool)), this, SLOT(aboutFormatChanged()));
	connect(ui->pdf, SIGNAL(toggled(bool)), this, SLOT(aboutFormatChanged()));
	connect(ui->browseFile, SIGNAL(clicked()), this, SLOT(aboutChooseFile()));
	connect(ui->file, SIGNAL(textChanged(QString)), this, SLOT(aboutFileNameChanged()));

	connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->exportTo, SIGNAL(clicked()), this, SLOT(accept()));
}

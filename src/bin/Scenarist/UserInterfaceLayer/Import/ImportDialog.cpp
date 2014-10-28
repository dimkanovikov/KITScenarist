#include "ImportDialog.h"
#include "ui_ImportDialog.h"

#include <BusinessLayer/Import/AbstractImporter.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QFileDialog>
#include <QFileInfo>
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
	QDialog(parent),
	ui(new Ui::ImportDialog)
{
	ui->setupUi(this);

	initView();
	initConnections();
}

ImportDialog::~ImportDialog()
{
	delete ui;
}

BusinessLogic::ImportParameters ImportDialog::importParameters() const
{
	BusinessLogic::ImportParameters importParameters;
	importParameters.filePath = ui->file->text();
	importParameters.removeScenesNumbers = ui->removeScenesNumbers->isChecked();
	if (ui->replaceScenario->isChecked()) {
		importParameters.insertionMode = BusinessLogic::ImportParameters::ReplaceDocument;
	} else if (ui->currentCursorPosition->isChecked()) {
		importParameters.insertionMode = BusinessLogic::ImportParameters::ToCursorPosition;
	} else {
		importParameters.insertionMode = BusinessLogic::ImportParameters::ToDocumentEnd;
	}
	return importParameters;
}

void ImportDialog::aboutChooseFile()
{
	const QString format = "rtf";
	QString filePath =
			QFileDialog::getOpenFileName(this, tr("Choose file to import"),
				::importFolderPath(), tr("%1 files (*%2)").arg(format.toUpper()).arg(format));

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
	ui->importTo->setEnabled(!ui->file->text().isEmpty());
}

void ImportDialog::initView()
{

}

void ImportDialog::initConnections()
{
	connect(ui->browseFile, SIGNAL(clicked()), this, SLOT(aboutChooseFile()));
	connect(ui->file, SIGNAL(textChanged(QString)), this, SLOT(aboutFileNameChanged()));

	connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->importTo, SIGNAL(clicked()), this, SLOT(accept()));
}

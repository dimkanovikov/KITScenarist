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

	/**
	 * @brief Получить список доступных к импорту видов файлов
	 */
	static QString filters() {
		QString filters;
		filters.append(QApplication::translate("UserInterface::ImportDialog", "All Supported Files") + QLatin1String(" (*.kitsp *.fdx *.docx *.doc *.odt)"));
        filters.append(";;");
        filters.append(QApplication::translate("UserInterface::ImportDialog","KIT Scenarist Project") + QLatin1String(" (*.kitsp)"));
		filters.append(";;");
		filters.append(QApplication::translate("UserInterface::ImportDialog","Final Draft screenplay") + QLatin1String(" (*.fdx)"));
		filters.append(";;");
        filters.append(QApplication::translate("UserInterface::ImportDialog","Trelby screenplay") + QLatin1String(" (*.trelby)"));
		filters.append(";;");
		filters.append(QApplication::translate("UserInterface::ImportDialog","Office Open XML") + QLatin1String(" (*.docx *.doc)"));
		filters.append(";;");
		filters.append(QApplication::translate("UserInterface::ImportDialog","OpenDocument Text") + QLatin1String(" (*.odt)"));

		return filters;
	}
}


ImportDialog::ImportDialog(QWidget *parent) :
	QLightBoxDialog(parent),
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
			QFileDialog::getOpenFileName(this, tr("Choose file to import"), ::importFolderPath(), ::filters());

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

	m_import = ui->buttons->addButton(tr("Import"), QDialogButtonBox::AcceptRole);
	m_import->setEnabled(false);

	QLightBoxDialog::initView();
}

void ImportDialog::initConnections()
{
	connect(ui->browseFile, SIGNAL(clicked()), this, SLOT(aboutChooseFile()));
	connect(ui->file, SIGNAL(textChanged(QString)), this, SLOT(aboutFileNameChanged()));

	connect(ui->buttons, SIGNAL(rejected()), this, SLOT(reject()));
	connect(ui->buttons, SIGNAL(accepted()), this, SLOT(accept()));

	QLightBoxDialog::initConnections();
}

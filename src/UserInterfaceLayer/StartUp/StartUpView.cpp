#include "StartUpView.h"
#include "ui_StartUpView.h"

#include "RecentFilesDelegate.h"

#include <QStandardItemModel>

using UserInterface::StartUpView;
using UserInterface::RecentFilesDelegate;


StartUpView::StartUpView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::StartUpView)
{
	ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

StartUpView::~StartUpView()
{
	delete ui;
}

void StartUpView::setRecentFiles(
		const QMap<QString, QString>& _recentFiles,
		const QMap<QString, QString>& _recentFilesUsing)
{
	//
	// Если в списке была установлена модель, удалим её
	//
	if (ui->recentFiles->model() != 0) {
		QAbstractItemModel* oldModel = ui->recentFiles->model();
		ui->recentFiles->setModel(0);
		delete oldModel;
		oldModel = 0;
	}

	//
	// Создаём новую модель
	//
	QStandardItemModel* newModel = new QStandardItemModel(ui->recentFiles);
	QStringList usingDates = _recentFilesUsing.values();
	qSort(usingDates.begin(), usingDates.end(), qGreater<QString>());
	foreach (const QString& usingDate, usingDates) {
		QString filePath = _recentFilesUsing.key(usingDate);
		QStandardItem* item = new QStandardItem;

		//
		// Название проекта
		//
		item->setData(_recentFiles.value(filePath), Qt::DisplayRole);
		//
		// Путь к файлу
		//
		item->setData(filePath, Qt::ToolTipRole);
		newModel->appendRow(item);
	}

	//
	// Устанавливаем модель
	//
	ui->recentFiles->setModel(newModel);
}

void StartUpView::aboutOpenRecentFileClicked()
{
	QModelIndex currentIndex = ui->recentFiles->currentIndex();
	QAbstractItemModel* recentFiles = ui->recentFiles->model();

	//
	// Получим путь к файлу для загрузки
	//
	QString clickedFilePath =
			recentFiles->data(currentIndex, Qt::ToolTipRole).toString();

	//
	// Уведомляем о том, что файл выбран
	//
	emit openRecentProjectClicked(clickedFilePath);
}

void StartUpView::initView()
{
    ui->version->setText(QApplication::applicationVersion());
	ui->recentFiles->setItemDelegate(new RecentFilesDelegate(ui->recentFiles));
}

void StartUpView::initConnections()
{
	connect(ui->createProject, SIGNAL(linkActivated(QString)), this, SIGNAL(createProjectClicked()));
	connect(ui->openProject, SIGNAL(linkActivated(QString)), this, SIGNAL(openProjectClicked()));
	connect(ui->help, SIGNAL(linkActivated(QString)), this, SIGNAL(helpClicked()));

	connect(ui->recentFiles, SIGNAL(clicked(QModelIndex)), this, SLOT(aboutOpenRecentFileClicked()));
}

void StartUpView::initStyleSheet()
{
	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);
}

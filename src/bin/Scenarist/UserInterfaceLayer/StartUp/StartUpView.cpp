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
		item->setData(filePath, Qt::WhatsThisRole);
		newModel->appendRow(item);
	}

	//
	// Устанавливаем модель
	//
	ui->recentFiles->setModel(newModel);
}

void StartUpView::updateLogo(bool _isDarkTheme)
{
	ui->logo->setPixmap(QPixmap(_isDarkTheme ? ":/Graphics/Images/logo-white.png" : ":/Graphics/Images/logo-black.png"));
}

void StartUpView::setUpdateInfo(const QString& _updateInfo)
{
	ui->updateInfo->setText(_updateInfo);
	ui->updateInfo->show();
}

bool StartUpView::eventFilter(QObject* _watched, QEvent* _event)
{
	bool result = false;

	//
	// Когда мышка входит или покидает список недавних файлов, нужно перерисовать его,
	// чтобы не осталось невыделенных/выделенных модулей
	//
	if (_watched == ui->recentFiles
		&& (_event->type () == QEvent::Enter || _event->type () == QEvent::Leave))
	{
		ui->recentFiles->repaint ();
		result = true;
	}
	//
	// Для всех остальных событий используем реализацю базовового класса
	//
	else
	{
		result = QWidget::eventFilter(_watched, _event);
	}

	return result;
}

void StartUpView::aboutOpenRecentFileClicked()
{
	QModelIndex currentIndex = ui->recentFiles->currentIndex();
	QAbstractItemModel* recentFiles = ui->recentFiles->model();

	//
	// Получим путь к файлу для загрузки
	//
	QString clickedFilePath =
			recentFiles->data(currentIndex, Qt::WhatsThisRole).toString();

	//
	// Уведомляем о том, что файл выбран
	//
	emit openRecentProjectClicked(clickedFilePath);
}

void StartUpView::initView()
{
    ui->version->setText(QApplication::applicationVersion());

	ui->updateInfo->hide();

	ui->recentFiles->setItemDelegate(new RecentFilesDelegate(ui->recentFiles));
	ui->recentFiles->setMouseTracking(true);
	ui->recentFiles->installEventFilter(this);
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

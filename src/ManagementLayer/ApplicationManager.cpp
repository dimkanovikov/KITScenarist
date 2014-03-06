#include "ApplicationManager.h"

#include "StartUp/StartUpManager.h"
#include "Scenario/ScenarioManager.h"

#include <DataLayer/Database/Database.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <3rd_party/Widgets/SideBar/SideBar.h>

#include <QStackedLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QMenu>




//#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
//#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
//#include <BusinessLayer/Export/PdfExporter.h>
//#include <BusinessLayer/Export/RtfExporter.h>
//#include <BusinessLayer/Chronometry/ChronometerFacade.h>

//#include <QTextEdit>
//#include <UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEdit.h>
//#include <UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemDelegate.h>
//#include <QTreeView>
//#include <QPushButton>
//#include <QHBoxLayout>
//#include <QVBoxLayout>
//#include <QLabel>






using namespace ManagementLayer;

namespace {
	const QString PROJECT_FILE_EXTENSION = ".kitsp"; // kit scenarist project

	/**
	 * @brief Отключить некоторые вкладки
	 *
	 * Используется при старте приложения, пока не загружен какой-либо проект
	 */
	static void disableTabs(SideTabBar* _tabBar) {
		QList<QAction*> tabs = _tabBar->tabs();
		foreach (QAction* tab, tabs) {
			if (tab != tabs.first()) {
				tab->setEnabled(false);
			}
		}
	}

	/**
	 * @brief Активировать все вкладки
	 */
	static void enableTabs(SideTabBar* _tabBar) {
		foreach (QAction* tab, _tabBar->tabs()) {
			tab->setEnabled(true);
		}
	}
}


ApplicationManager::ApplicationManager(QObject *parent) :
	QObject(parent),
	m_view(new QWidget),
	m_menu(new QToolButton(m_view)),
	m_tabs(new SideTabBar(m_view)),
	m_tabsWidgets(new QStackedLayout),
	m_startUpManager(new StartUpManager(this, m_view)),
	m_scenarioManager(new ScenarioManager(this, m_view))
{
	initView();
	initConnections();
}

ApplicationManager::~ApplicationManager()
{
	saveIfNeeded();

	delete m_view;
	m_view = 0;
}

void ApplicationManager::exec()
{
//	document = new BusinessLogic::ScenarioDocument(this);
//	textEdit = new ScenarioTextEdit(0, document->document());
//	QTreeView* view = new QTreeView;
//	view->setItemDelegate(new ScenarioNavigatorItemDelegate(view));
//	view->setDragDropMode(QAbstractItemView::DragDrop);
//	view->setDragEnabled(true);
//	view->setDropIndicatorShown(true);
//	view->setModel(document->model());
//	QPushButton* btn = new QPushButton("Print");
//	connect(btn, SIGNAL(clicked()), this, SLOT(print()));
//	label = new QLabel;
//	connect(textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(updatePositionDuration()));
//	QVBoxLayout* leftLayout = new QVBoxLayout;
//	leftLayout->addWidget(view);
//	leftLayout->addWidget(btn);
//	leftLayout->addWidget(label);

//	QHBoxLayout* layout = new QHBoxLayout(m_view);
//	layout->addLayout(leftLayout);
//	layout->addWidget(textEdit);



	m_view->resize(800,600);
	m_view->show();
}

void ApplicationManager::aboutCreateNew()
{
	//
	// Если нужно сохранить проект
	//
	saveIfNeeded();

	//
	// Получим имя файла для нового проекта
	//
	QString newProjectFileName =
			QFileDialog::getSaveFileName(
				m_view,
				tr("Choose file for new project"),
				QString(),
				tr ("Scenarist project files (*%1)").arg(PROJECT_FILE_EXTENSION)
				);

	//
	// Если файл выбран
	//
	if (!newProjectFileName.isEmpty()) {
		//
		// ... установим расширение, если не задано
		//
		if (!newProjectFileName.endsWith(PROJECT_FILE_EXTENSION)) {
			newProjectFileName.append(PROJECT_FILE_EXTENSION);
		}

		//
		// ... очистим все загруженные на текущий момент данные
		//
		DataStorageLayer::StorageFacade::clearStorages();

		//
		// ... если файл существовал, удалим его для удаления данных в нём
		//
		if (QFile::exists(newProjectFileName)) {
			QFile::remove(newProjectFileName);
		}

		//
		// ... создаём новую базу данных в файле
		//
		DatabaseLayer::Database::setCurrentFile(newProjectFileName);

		//
		// ... перейдём к редактированию
		//
		goToEditCurrentProject();
	}
}

void ApplicationManager::aboutSaveAs()
{
	//
	// Получим имя файла для сохранения
	//
	QString saveAsProjectFileName =
			QFileDialog::getSaveFileName(
				m_view,
				tr("Choose file for save project"),
				QString(),
				tr ("Scenarist project files (*%1)").arg(PROJECT_FILE_EXTENSION)
				);

	//
	// Если файл выбран
	//
	if (!saveAsProjectFileName.isEmpty()) {
		//
		// ... если пользователь хочет просто пересохранить проект
		//
		if (saveAsProjectFileName == DatabaseLayer::Database::currentFile()) {
			aboutSave();
		}
		//
		// ... если сохраняем в новый файл
		//
		else {
			//
			// ... если файл существовал, удалим его для удаления данных в нём
			//
			if (QFile::exists(saveAsProjectFileName)) {
				QFile::remove(saveAsProjectFileName);
			}

			//
			// ... скопируем текущую базу в указанный файл
			//
			QFile::copy(DatabaseLayer::Database::currentFile(), saveAsProjectFileName);

			//
			// ... переключаемся на использование другого файла
			//
			DatabaseLayer::Database::setCurrentFile(saveAsProjectFileName);

			//
			// ... сохраняем изменения
			//
			aboutSave();
		}
	}
}

void ApplicationManager::aboutSave()
{
	//
	// Управляющие должны сохранить несохранённые данные
	//
	m_scenarioManager->saveCurrentProject();

	//
	// Добавим проект к недавно используемым
	//
	saveCurrentProjectInRecent();

	//
	// Изменим статус окна на сохранение изменений
	//
	m_view->setWindowModified(false);
}

void ApplicationManager::aboutLoad(const QString& _fileName)
{
	//
	// Если нужно сохранить проект
	//
	saveIfNeeded();

	//
	// Имя файла для загрузки
	//
	QString loadProjectFileName = _fileName;

	//
	// Если имя файла не определено, выберем его в диалоге выбора файла
	//
	if (loadProjectFileName.isEmpty()) {
		loadProjectFileName =
				QFileDialog::getOpenFileName(
					m_view,
					tr("Choose project file to open"),
					QString(),
					tr ("Scenarist project files (*%1)").arg(PROJECT_FILE_EXTENSION)
					);
	}

	//
	// Если файл выбран
	//
	if (!loadProjectFileName.isEmpty()) {
		//
		// ... очистим все загруженные на текущий момент данные
		//
		DataStorageLayer::StorageFacade::clearStorages();

		//
		// ... переключаемся на работу с выбранным файлом
		//
		DatabaseLayer::Database::setCurrentFile(loadProjectFileName);

		//
		// ... перейдём к редактированию
		//
		goToEditCurrentProject();
	}
}

void ApplicationManager::saveIfNeeded()
{
	//
	// Если какие-то данные изменены
	//
	if (m_view->isWindowModified()) {
		//
		// ... спрашиваем пользователя, хочет ли он сохранить изменения
		//
		int questionResult = QMessageBox::question(m_view, tr("Save project changes?"),
												   tr("Project was modified. Save changes?"),
												   QMessageBox::Yes | QMessageBox::No);
		//
		// ... и сохраняем, если хочет
		//
		if (questionResult == QMessageBox::Yes) {
			aboutSave();
		} else {
			m_view->setWindowModified(false);
		}
	}
}

void ApplicationManager::saveCurrentProjectInRecent()
{
	//
	// Сохраним текущий проект в недавно использованых
	//
	m_startUpManager->addRecentFile(DatabaseLayer::Database::currentFile());
}

void ApplicationManager::goToEditCurrentProject()
{
	//
	// Активируем вкладки
	//
	::enableTabs(m_tabs);

	//
	// Загрузить данные из файла
	//
	m_scenarioManager->loadCurrentProject();

	//
	// Установим заголовок
	//
	QString projectFileName = DatabaseLayer::Database::currentFile();
	projectFileName = projectFileName.split(QDir::separator()).last();
	m_view->setWindowTitle(tr("%1[*] - Scenarist").arg(projectFileName));

	//
	// Добавим проект к недавно используемым
	//
	saveCurrentProjectInRecent();

	//
	// Перейти на вкладку редактирования сценария
	//
	m_tabs->setCurrent(1);
}

void ApplicationManager::initView()
{
	//
	// Настроим меню
	//
	m_menu->setText(tr("Menu"));
	m_menu->setPopupMode(QToolButton::MenuButtonPopup);
	m_menu->setMenu(createMenu());

	//
	// Настроим боковую панель
	//
	m_tabs->addTab(tr("Start"), QIcon(":/Graphics/Icons/start.png"));
	m_tabs->addTab(tr("Scenario"), QIcon(":/Graphics/Icons/script.png"));
	m_tabs->addTab(tr("Characters"), QIcon(":/Graphics/Icons/characters.png"));
	m_tabs->addTab(tr("Locations"), QIcon(":/Graphics/Icons/locations.png"));
	m_tabs->addTab(tr("Settings"), QIcon(":/Graphics/Icons/settings.png"));
	::disableTabs(m_tabs);

	//
	// Настроим виджеты соответствующие вкладкам
	//
	m_tabsWidgets->addWidget(m_startUpManager->view());
	m_tabsWidgets->addWidget(m_scenarioManager->view());

	//
	// Расположим всё на форме
	//
	QVBoxLayout* leftLayout = new QVBoxLayout;
	leftLayout->setContentsMargins(QMargins());
	leftLayout->setSpacing(0);
	leftLayout->addWidget(m_menu);
	leftLayout->addWidget(m_tabs);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(leftLayout);
	layout->addLayout(m_tabsWidgets);

	m_view->setLayout(layout);
}

QMenu* ApplicationManager::createMenu()
{
	//
	// Сформируем меню
	//
	QMenu* menu = new QMenu(m_menu);
	QAction* createNewProject = menu->addAction(tr("New"));
	QAction* openProject = menu->addAction(tr("Open"));
	QAction* saveProject = menu->addAction(tr("Save"));
	QAction* saveProjectAs = menu->addAction(tr("Save As..."));

	//
	// Настроим соединения
	//
	connect(createNewProject, SIGNAL(triggered()), this, SLOT(aboutCreateNew()));
	connect(openProject, SIGNAL(triggered()), this, SLOT(aboutLoad()));
	connect(saveProject, SIGNAL(triggered()), this, SLOT(aboutSave()));
	connect(saveProjectAs, SIGNAL(triggered()), this, SLOT(aboutSaveAs()));

	return menu;
}

void ApplicationManager::initConnections()
{
	connect(m_menu, SIGNAL(clicked()), m_menu, SLOT(showMenu()));
	connect(m_tabs, SIGNAL(currentChanged(int)), m_tabsWidgets, SLOT(setCurrentIndex(int)));
	connect(m_startUpManager, SIGNAL(createProjectRequested()), this, SLOT(aboutCreateNew()));
	connect(m_startUpManager, SIGNAL(openProjectRequested()), this, SLOT(aboutLoad()));
	connect(m_startUpManager, SIGNAL(openRecentProjectRequested(QString)), this, SLOT(aboutLoad(QString)));
}

//void ApplicationManager::print()
//{
//	BusinessLogic::PdfExporter exporter;
//	exporter.exportTo(document->document(), "/home/dimkanovikov/1.pdf");
//	BusinessLogic::RtfExporter exporter1;
//	exporter1.exportTo(document->document(), "/home/dimkanovikov/1.rtf");
//}

//void ApplicationManager::updatePositionDuration()
//{
//	label->setText(
//				ChronometerFacade::secondsToTime(
//					document->durationAtPosition(textEdit->textCursor().position()))
//				);
//}

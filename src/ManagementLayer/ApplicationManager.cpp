#include "ApplicationManager.h"

#include "StartUp/StartUpManager.h"

#include <DataLayer/Database/Database.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <QFileDialog>
#include <QMessageBox>




#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/Export/PdfExporter.h>
#include <BusinessLayer/Export/RtfExporter.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <QTextEdit>
#include <UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEdit.h>
#include <UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemDelegate.h>
#include <QTreeView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>






using namespace ManagementLayer;


ApplicationManager::ApplicationManager(QObject *parent) :
	QObject(parent),
	m_view(new QWidget),
	m_startUpManager(new StartUpManager(this, m_view))
{
	initConnections();
}

ApplicationManager::~ApplicationManager()
{
	delete m_view;
	m_view = 0;
}

namespace {
	BusinessLogic::ScenarioDocument* document = 0;
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

	QHBoxLayout* layout = new QHBoxLayout(m_view);
	layout->addWidget(m_startUpManager->view());

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
				tr ("Scenarist project files (*.ksp)") // kit scenarist project
				);

	//
	// Если файл выбран
	//
	if (!newProjectFileName.isEmpty()) {
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
		// TODO: проинициилизовать контроллеры
		//

		//
		// Добавим проект к недавно используемым
		//
		saveCurrentProjectInRecent();
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
				tr ("Scenarist project files (*.ksp)") // kit scenarist project
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
	// TODO: управляющие должны сохранить несохранённые данные
	//


	//
	// Добавим проект к недавно используемым
	//
	saveCurrentProjectInRecent();
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
					tr ("Scenarist project files (*.ksp)") // kit scenarist project
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
		// TODO: загрузить данные из файла
		//
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

void ApplicationManager::initConnections()
{
	connect(m_startUpManager, SIGNAL(createProjectRequested()), this, SLOT(aboutCreateNew()));
	connect(m_startUpManager, SIGNAL(openProjectRequested()), this, SLOT(aboutLoad()));
	connect(m_startUpManager, SIGNAL(openRecentProjectRequested(QString)), this, SLOT(aboutLoad(QString)));
}

void ApplicationManager::print()
{
	BusinessLogic::PdfExporter exporter;
	exporter.exportTo(document->document(), "/home/dimkanovikov/1.pdf");
	BusinessLogic::RtfExporter exporter1;
	exporter1.exportTo(document->document(), "/home/dimkanovikov/1.rtf");
}

void ApplicationManager::updatePositionDuration()
{
	label->setText(
				ChronometerFacade::secondsToTime(
					document->durationAtPosition(textEdit->textCursor().position()))
				);
}

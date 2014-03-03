#include "StartUpManager.h"

#include <UserInterfaceLayer/StartUp/StartUpView.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

using ManagementLayer::StartUpManager;
using UserInterface::StartUpView;

namespace {
	const QString RECENT_FILES_SETTINGS_KEY = "application/recent-files";
}


StartUpManager::StartUpManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new StartUpView(_parentWidget))
{
	initData();
	initView();
	initConnections();
}

StartUpManager::~StartUpManager()
{
	//
	// Сохраним последние используемые файлы приложения
	//
	DataStorageLayer::StorageFacade::settingsStorage()->setValues(
				m_recentFiles,
				RECENT_FILES_SETTINGS_KEY,
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
}

QWidget* StartUpManager::view() const
{
	return m_view;
}

void StartUpManager::addRecentFile(const QString& _filePath, const QString& _projectName)
{
	//
	// Определим название проекта, если оно не задано
	//
	QString projectName = _projectName;
	if (projectName.isEmpty()) {
		QFileInfo fileInfo(_filePath);
		projectName = fileInfo.baseName();
	}

	//
	// Добавим файл в список
	//
	m_recentFiles.insert(_filePath, projectName);

	//
	// Обновим список файлов в представлении
	//
	m_view->setRecentFiles(m_recentFiles);
}

void StartUpManager::aboutOpenRecentProjectRequested(const QString& _filePath)
{
	//
	// Если выбранного файла не существует
	//
	if (!QFile::exists(_filePath)) {
		//
		// ... уведомим об этом пользователя
		//
		QMessageBox::information(
					m_view,
					tr("Can't open choosed file"),
					tr("File <b>%1</b> isn't exist. File will be removed from recent list.").arg(_filePath),
					QMessageBox::Ok
					);

		//
		// ... удалим его из списка
		//
		m_recentFiles.remove(_filePath);

		//
		// ... обновим список файлов в представлении
		//
		m_view->setRecentFiles(m_recentFiles);
	}
	//
	// Если выбранный файл существует, испускаем соответствующий сигнал
	//
	else {
		emit openRecentProjectRequested(_filePath);
	}
}

void StartUpManager::initData()
{
	m_recentFiles =
			DataStorageLayer::StorageFacade::settingsStorage()->values(
				RECENT_FILES_SETTINGS_KEY,
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
}

void StartUpManager::initView()
{
	m_view->setRecentFiles(m_recentFiles);
}

void StartUpManager::initConnections()
{
	connect(m_view, SIGNAL(createProjectClicked()), this, SIGNAL(createProjectRequested()));
	connect(m_view, SIGNAL(openProjectClicked()), this, SIGNAL(openProjectRequested()));
	connect(m_view, SIGNAL(helpClicked()), this, SIGNAL(helpRequested()));

	connect(m_view, SIGNAL(openRecentProjectClicked(QString)),
			this, SLOT(aboutOpenRecentProjectRequested(QString)));
}

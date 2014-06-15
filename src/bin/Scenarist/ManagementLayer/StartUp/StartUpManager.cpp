#include "StartUpManager.h"

#include <UserInterfaceLayer/StartUp/StartUpView.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

using ManagementLayer::StartUpManager;
using UserInterface::StartUpView;

namespace {
	const int MAX_RECENT_FILES_COUNT = 7;
	const QString RECENT_FILES_LIST_SETTINGS_KEY = "application/recent-files/list";
	const QString RECENT_FILES_USING_SETTINGS_KEY = "application/recent-files/using";
}


StartUpManager::StartUpManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new StartUpView(_parentWidget))
{
	initData();
	initView();
	initConnections();

	checkNewVersion();
}

StartUpManager::~StartUpManager()
{
	//
	// Сохраним последние используемые файлы приложения
	//
	DataStorageLayer::StorageFacade::settingsStorage()->setValues(
				m_recentFiles,
				RECENT_FILES_LIST_SETTINGS_KEY,
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValues(
				m_recentFilesUsing,
				RECENT_FILES_USING_SETTINGS_KEY,
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
	// Если в списке больше допустимого кол-ва используемых файлов удалим давно используемый
	//
	if (m_recentFiles.size() >= MAX_RECENT_FILES_COUNT
		&& !m_recentFiles.contains(_filePath)) {
		QStringList usingDates = m_recentFilesUsing.values();
		qSort(usingDates);
		QString oldestProject = m_recentFilesUsing.key(usingDates.first());
		m_recentFiles.remove(oldestProject);
		m_recentFilesUsing.remove(oldestProject);
	}

	//
	// Добавим файл в список
	//
	m_recentFiles.insert(_filePath, projectName);

	//
	// Сохраним информацию о последнем использовании
	//
	m_recentFilesUsing.insert(_filePath, QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz"));

	//
	// Обновим список файлов в представлении
	//
	m_view->setRecentFiles(m_recentFiles, m_recentFilesUsing);
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
		m_recentFilesUsing.remove(_filePath);

		//
		// ... обновим список файлов в представлении
		//
		m_view->setRecentFiles(m_recentFiles, m_recentFilesUsing);
	}
	//
	// Если выбранный файл существует, испускаем соответствующий сигнал
	//
	else {
		emit openRecentProjectRequested(_filePath);
	}
}

void StartUpManager::aboutLoadUpdatesInfo(QNetworkReply* _reply)
{
	if (_reply != 0) {
		QString updatesPageData = _reply->readAll().simplified();

		//
		// Извлекаем все версии и формируем ссылку на последнюю из них
		//
#ifdef Q_OS_WIN
		QRegularExpression rx_updateFiner("scenarist-setup-(\\d+.\\d+.\\d+).exe");
#elif defined Q_OS_LINUX
		QRegularExpression rx_updateFiner("scenarist-setup-(\\d+.\\d+.\\d+)_i386.deb");
#elif defined Q_OS_MAC
		QRegularExpression rx_updateFiner("scenarist-setup-(\\d+.\\d+.\\d+).dmg");
#endif

		QRegularExpressionMatch match = rx_updateFiner.match(updatesPageData);
		QList<QString> versions;
		while (match.hasMatch()) {
			versions.append(match.captured(1));
			match = rx_updateFiner.match(updatesPageData, match.capturedEnd(1));
		}

		//
		// Если версии найдены
		//
		if (versions.count() > 0) {
			//
			// Сортируем
			//
			qSort(versions);

			//
			// Извлекаем последнюю версию
			//
			QString maxVersion = versions.last();

			//
			// Если она больше текущей версии программы, выводим информацию
			//
			if (QApplication::applicationVersion() < maxVersion) {
				QString updateInfo =
						tr("Released version %1 ").arg(maxVersion)
#ifdef Q_OS_WIN
						+ "<a href=\"http://dimkanovikov.pro/kit/scenarist/downloads/windows/scenarist-setup-%1.exe\" "
#elif defined Q_OS_LINUX
						+ "<a href=\"http://dimkanovikov.pro/kit/scenarist/downloads/linux/\" "
#elif defined Q_OS_MAC
						+ "<a href=\"http://dimkanovikov.pro/kit/scenarist/downloads/windows/scenarist-setup-%1.dmg\" "
#endif
						+ tr("style=\"color:#2b78da;\">download</a> "
							 "or <a href=\"http://dimkanovikov.pro/kit/scenarist/news.html\" "
							 "style=\"color:#2b78da;\">read more</a>.");
				m_view->setUpdateInfo(updateInfo);
			}
		}
	}
}

void StartUpManager::initData()
{
	m_recentFiles =
			DataStorageLayer::StorageFacade::settingsStorage()->values(
				RECENT_FILES_LIST_SETTINGS_KEY,
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	m_recentFilesUsing =
			DataStorageLayer::StorageFacade::settingsStorage()->values(
				RECENT_FILES_USING_SETTINGS_KEY,
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
}

void StartUpManager::initView()
{
	m_view->setRecentFiles(m_recentFiles, m_recentFilesUsing);
}

void StartUpManager::initConnections()
{
	connect(m_view, SIGNAL(createProjectClicked()), this, SIGNAL(createProjectRequested()));
	connect(m_view, SIGNAL(openProjectClicked()), this, SIGNAL(openProjectRequested()));
	connect(m_view, SIGNAL(helpClicked()), this, SIGNAL(helpRequested()));

	connect(m_view, SIGNAL(openRecentProjectClicked(QString)),
			this, SLOT(aboutOpenRecentProjectRequested(QString)));
}

void StartUpManager::checkNewVersion()
{
	QNetworkAccessManager* manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(aboutLoadUpdatesInfo(QNetworkReply*)));

#ifdef Q_OS_WIN
	manager->get(QNetworkRequest(QUrl("http://dimkanovikov.pro/kit/scenarist/downloads/windows/")));
#elif defined Q_OS_LINUX
	manager->get(QNetworkRequest(QUrl("http://dimkanovikov.pro/kit/scenarist/downloads/linux/")));
#elif defined Q_OS_MAC
	//
	// TODO: macos updates
	//
#endif
}

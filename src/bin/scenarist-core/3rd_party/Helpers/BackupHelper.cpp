#include "BackupHelper.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>

#include <Domain/Scenario.h>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace {
	/**
	 * @brief Расширение файла "Кит сценарист резервная копия"
	 */
	const QString BACKUP_VERSIONS_EXTANSION = "kitsrc";

	/**
	 * @brief Название соединения для БД резервных копий версий сценария
	 */
	const QString BACKUPDB_CONNECTION_NAME = "backup_versions";
}


BackupHelper::BackupHelper() :
	m_isActive(false)
{
}

void BackupHelper::setIsActive(bool _isActive)
{
	if (m_isActive != _isActive) {
		m_isActive = _isActive;
	}
}

void BackupHelper::setBackupDir(const QString& _dir)
{
	if (m_backupDir != _dir) {
		m_backupDir = _dir;
	}
}

void BackupHelper::saveBackup(const QString& _filePath)
{
	if (m_isActive) {
		//
		// Сформируем путь к резервной копии
		//
		QString backupPath = m_backupDir;
		if (!backupPath.endsWith(QDir::separator())) {
			backupPath.append(QDir::separator());
		}

		QFileInfo fileInfo(_filePath);
		const QString tmpBackupFileName =
				QString("%1%2.backup.tmp.%3").arg(backupPath, fileInfo.completeBaseName(), fileInfo.completeSuffix());
		const QString backupFileName =
				QString("%1%2.full.backup.%3").arg(backupPath, fileInfo.completeBaseName(), fileInfo.completeSuffix());
		const QString backupVersionsFileName =
				QString("%1%2.versions.backup.%3").arg(backupPath, fileInfo.completeBaseName(), BACKUP_VERSIONS_EXTANSION);

		//
		// Копируем файл во временную резервную копию
		//
		if (QFile::copy(_filePath, tmpBackupFileName)) {
			//
			// Если скопировать удалось, переименовываем временную копию
			//
			QFile::remove(backupFileName);
			QFile::rename(tmpBackupFileName, backupFileName);
		}

		//
		// Добавляем версию сценария в файл с резервными копиями версий текста сценария
		//
		{
			//
			// NOTE: Как быть с быстродействием?
			//
			{
				QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", BACKUPDB_CONNECTION_NAME);
				db.setDatabaseName(backupVersionsFileName);
				db.open();
				//
				// Храним всего 100 копий, удаляя более старые
				//
				QSqlQuery backuper(db);
				backuper.exec("CREATE TABLE IF NOT EXISTS versions (id INTEGER, version TEXT NOT NULL, datetime TEXT NOT NULL)");
				backuper.exec("UPDATE versions SET id = (id + 1)");
				backuper.exec("DELETE FROM versions WHERE id = 101");

				backuper.prepare("INSERT INTO versions VALUES(1, ?, ?)");
				backuper.addBindValue(DataStorageLayer::StorageFacade::scenarioStorage()->current()->text());
				backuper.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
				backuper.exec();
			}

			QSqlDatabase::removeDatabase(BACKUPDB_CONNECTION_NAME);
		}
	}
}

#include "BackupHelper.h"

#include <QDir>
#include <QFileInfo>


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
				QString("%1%2.backup.tmp.%3").arg(backupPath, fileInfo.baseName(), fileInfo.completeSuffix());
		const QString backupFileName =
				QString("%1%2.backup.%3").arg(backupPath, fileInfo.baseName(), fileInfo.completeSuffix());

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
	}
}

#include "SettingsMapper.h"

#include <DataLayer/Database/Database.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

using namespace DataMappingLayer;
using namespace DatabaseLayer;


void SettingsMapper::setValue(const QString& _key, const QString& _value)
{
	QSqlQuery q_loader = Database::query();
	q_loader.prepare("INSERT INTO system_variables VALUES (?, ?)");
	q_loader.addBindValue(_key);
	q_loader.addBindValue(_value);
	q_loader.exec();
}

QString SettingsMapper::value(const QString& _key)
{
	QSqlQuery q_loader = Database::query();
	q_loader.prepare("SELECT value FROM system_variables WHERE variable = ?");
	q_loader.addBindValue(_key);
	q_loader.exec();
	q_loader.next();
	return q_loader.value("value").toString();
}

SettingsMapper::SettingsMapper()
{
}

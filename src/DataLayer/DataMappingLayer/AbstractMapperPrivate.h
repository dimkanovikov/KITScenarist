#ifndef ABSTRACTMAPPERPRIVATE_P_H
#define ABSTRACTMAPPERPRIVATE_P_H

#include <QThread>
#include <QSqlQuery>
#include <QMutex>
#include <QQueue>

class QMutex;


namespace DataMappingLayer
{
	/**
	 * @brief Класс для асинхронного выполнения операций с базой данных
	 *
	 * Используется для сохранения, обновления и удаления записей в базе данных.
	 */
	class SqlExecutingQueuedThread : public QThread
	{
		Q_OBJECT

	public:
		explicit SqlExecutingQueuedThread(QObject* _parent = 0);

		/**
		 * @brief Выполнить запрос
		 *
		 * Если в данный момент выполняется другой запрос, то переданный будет поставлен в очередь
		 */
		void executeSql(const QSqlQuery& _sqlQuery);

	protected:
		void run();

	private:
		/**
		 * @brief Мьютекс для блокировки очереди запросов
		 */
		QMutex m_mutex;

		/**
		 * @brief Очередь запросов для выполнения
		 */
		QQueue<QSqlQuery> m_sqlQueue;
	};
}

#endif // ABSTRACTMAPPERPRIVATE_P_H

#ifndef STATISTICSMANAGER_H
#define STATISTICSMANAGER_H

#include <QObject>

namespace UserInterface {
	class StatisticsView;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий статистикой
	 */
	class StatisticsManager : public QObject
	{
		Q_OBJECT

	public:
		explicit StatisticsManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

	private:
		/**
		 * @brief Представление для страницы со статистикой
		 */
		UserInterface::StatisticsView* m_view;
	};
}

#endif // STATISTICSMANAGER_H

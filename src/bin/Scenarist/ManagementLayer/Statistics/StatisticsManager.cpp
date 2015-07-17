#include "StatisticsManager.h"

#include <UserInterfaceLayer/Statistics/StatisticsView.h>

using ManagementLayer::StatisticsManager;
using UserInterface::StatisticsView;


StatisticsManager::StatisticsManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new StatisticsView(_parentWidget))
{

}

QWidget* StatisticsManager::view() const
{
	return m_view;
}


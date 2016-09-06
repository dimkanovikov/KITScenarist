#ifndef SCENARIOCARDSMANAGER_H
#define SCENARIOCARDSMANAGER_H

#include <QObject>

namespace UserInterface {
	class ScenarioCardsView;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий карточками сценария
	 */
	class ScenarioCardsManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ScenarioCardsManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

	private:
		UserInterface::ScenarioCardsView* m_view;
	};
}

#endif // SCENARIOCARDSMANAGER_H

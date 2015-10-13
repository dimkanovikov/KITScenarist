#ifndef SCENARIOMANAGER_H
#define SCENARIOMANAGER_H

#include <QObject>

namespace UserInterface {
	class ScenarioTextView;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий сценарием
	 */
	class ScenarioManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ScenarioManager(QObject* _parent, QWidget* _parentWidget);

		/**
		 * @brief Панель инструментов представления
		 */
		QWidget* toolbar() const;

		/**
		 * @brief Представление
		 */
		QWidget* view() const;

	private:
		UserInterface::ScenarioTextView* m_view;
	};
}

#endif // SCENARIOMANAGER_H

#ifndef SCENARIOTEXTEDITMANAGER_H
#define SCENARIOTEXTEDITMANAGER_H

#include <QObject>


namespace UserInterface {
	class ScenarioTextEditWidget;
}

namespace BusinessLogic {
	class ScenarioTextDocument;
}

namespace ManagementLayer
{
	/**
	 * @brief Управляющий редактированием сценария
	 */
	class ScenarioTextEditManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ScenarioTextEditManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Установить документ для редактирования
		 */
		void setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document);

		/**
		 * @brief Установить хронометраж
		 */
		void setDuration(const QString& _duration);

	signals:
		/**
		 * @brief Изменилась позиция курсора
		 */
		void cursorPositionChanged(int _position);

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Редактор
		 */
		UserInterface::ScenarioTextEditWidget* m_view;
	};
}

#endif // SCENARIOTEXTEDITMANAGER_H

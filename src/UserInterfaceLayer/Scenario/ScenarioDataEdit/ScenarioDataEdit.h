#ifndef SCENARIODATAEDIT_H
#define SCENARIODATAEDIT_H

#include <QWidget>

namespace Ui {
	class ScenarioDataEdit;
}


namespace UserInterface
{
	/**
	 * @brief Редактор информации о сценарие
	 */
	class ScenarioDataEdit : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioDataEdit(QWidget* _parent = 0);
		~ScenarioDataEdit();

		QWidget* toolbar() const;

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		Ui::ScenarioDataEdit* ui;
	};
}

#endif // SCENARIODATAEDIT_H

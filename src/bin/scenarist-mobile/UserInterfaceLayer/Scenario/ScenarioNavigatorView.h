#ifndef SCENARIONAVIGATORVIEW_H
#define SCENARIONAVIGATORVIEW_H

#include <QWidget>

class QAbstractItemModel;

namespace Ui {
	class ScenarioNavigatorView;
}


namespace UserInterface
{
	class ScenarioNavigatorItemDelegate;


	/**
	 * @brief Представление навигатора по сценарию
	 */
	class ScenarioNavigatorView : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioNavigatorView(QWidget *parent = 0);
		~ScenarioNavigatorView();

		/**
		 * @brief Панель инструментов представления
		 */
		QWidget* toolbar() const;

		/**
		 * @brief Задать название сценария
		 */
		void setScenarioName(const QString& _name);

//		/**
//		 * @brief Установить количество сцен
//		 */
//		void setScenesCount(int _scenesCount);

		/**
		 * @brief Установить модель навигации
		 */
		void setModel(QAbstractItemModel* _model);

		/**
		 * @brief Установить текущий элемент
		 */
		void setCurrentIndex(const QModelIndex& _index);

		/**
		 * @brief Настроить отображение номеров сцен
		 */
		void setShowSceneNumber(bool _show);

		/**
		 * @brief Настроить отображение примечания сцен
		 */
		void setShowSceneDescription(bool _show);

		/**
		 * @brief Настроить что отображать в примечании к сцене, её текст (true) или синопсис (false)
		 */
		void setSceneDescriptionIsSceneText(bool _isSceneText);

		/**
		 * @brief Настроить высоту поля примечания
		 */
		void setSceneDescriptionHeight(int _height);

		/**
		 * @brief Пересоздать делегат отображения элементов в навигаторе
		 * @note Приходится именно пересоздавать навигатор, т.к. другого рабочего способа для
		 *		 обновления делегата не нашёл. Проблемы возникают при изменении размера виджета,
		 *		 который рисует делегат
		 */
		void resetView();

		/**
		 * @brief Убрать выделение
		 */
		void clearSelection();

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Нажата кнопка показать текст сценария
		 */
		void showTextClicked();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения для формы
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Интерфейс представления
		 */
		Ui::ScenarioNavigatorView *m_ui;

		/**
		 * @brief Делегат дерева
		 */
		ScenarioNavigatorItemDelegate* m_navigatorDelegate;
	};
}

#endif // SCENARIONAVIGATORVIEW_H

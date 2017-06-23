#ifndef SCENARIONAVIGATORITEMDELEGATE_H
#define SCENARIONAVIGATORITEMDELEGATE_H

#include <QStyledItemDelegate>


namespace UserInterface
{
	/**
	 * @brief Делегат для отрисовки элементов навигатора
	 */
	class ScenarioNavigatorItemDelegate : public QStyledItemDelegate
	{
        Q_OBJECT

	public:
		explicit ScenarioNavigatorItemDelegate(QObject* _parent = 0);

		void paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const;
		QSize sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const;

		/**
		 * @brief Настроить способ отображения
		 */
		/** @{ */
		void setShowSceneNumber(bool _show);
		void setShowSceneTitle(bool _show);
		void setShowSceneDescription(bool _show);
		void setSceneDescriptionIsSceneText(bool _isSceneText);
		void setSceneDescriptionHeight(int _height);
		/** @} */

	private:
		/**
		 * @brief Отображать номер сцены
		 */
		bool m_showSceneNumber;

		/**
		 * @brief Отображать название сцены
		 */
		bool m_showSceneTitle;

		/**
		 * @brief Отображать описание сцены
		 */
		bool m_showSceneDescription;

		/**
		 * @brief Описанием сцены является текст сцены (true) или синопсис (false)
		 */
		bool m_sceneDescriptionIsSceneText;

		/**
		 * @brief Высота поля для отображения описания сцены
		 */
		int m_sceneDescriptionHeight;
	};
}

#endif // SCENARIONAVIGATORITEMDELEGATE_H

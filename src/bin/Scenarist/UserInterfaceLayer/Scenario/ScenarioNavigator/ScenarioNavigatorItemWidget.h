#ifndef SCENARIONAVIGATORITEMWIDGET_H
#define SCENARIONAVIGATORITEMWIDGET_H

#include <QWidget>

class QLabel;
class ElidedLabel;


namespace UserInterface
{
	/**
	 * @brief Виджет для отображения элементов навигатора
	 */
	class ScenarioNavigatorItemWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioNavigatorItemWidget(QWidget *parent = 0);

		void setIcon(const QPixmap& _icon);
		void setHeader(const QString& _header);
		void setDescription(const QString& _description);
		void setDuration(int _duration);

		/**
		 * @brief Вид отображения виджета
		 */
		enum Type {
			OnlyHeader,
			HeaderAndDescription
		};

		/**
		 * @brief Установить вид отображения
		 */
		void setType(Type _type, int _descriptionHeight = 0);

	private:
		/**
		 * @brief Значёк элемента
		 */
		QLabel* m_icon;

		/**
		 * @brief Заголовок
		 */
		ElidedLabel* m_header;

		/**
		 * @brief Описание
		 */
		ElidedLabel* m_description;

		/**
		 * @brief Длительность
		 */
		QLabel* m_duration;
	};
}

#endif // SCENARIONAVIGATORITEMWIDGET_H

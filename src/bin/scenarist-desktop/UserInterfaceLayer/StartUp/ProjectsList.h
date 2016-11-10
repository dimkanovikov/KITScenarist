#ifndef PROJECTSLIST_H
#define PROJECTSLIST_H

#include <QScrollArea>

class QAbstractItemModel;


namespace UserInterface
{
	/**
	 * @brief Виджет списка проектов
	 */
	class ProjectsList : public QScrollArea
	{
		Q_OBJECT

	public:
		explicit ProjectsList(QWidget* _parent = 0);

		/**
		 * @brief Установить модель проектов
		 */
		void setModel(QAbstractItemModel* _model);

		/**
		 * @brief Получить модель проектов
		 */
		QAbstractItemModel* model() const;

	signals:
		/**
		 * @brief Был выбран проект для открытия
		 */
		void clicked(const QModelIndex&);

	private:
		/**
		 * @brief Обработать выбор проекта
		 */
		void handleProjectClick();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

	private:
		/**
		 * @brief Модель списка проектов
		 */
		QAbstractItemModel* m_model;
	};
}

#endif // PROJECTSLIST_H

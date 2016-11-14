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
		void setModel(QAbstractItemModel* _model, bool _isRemote);

		/**
		 * @brief Получить модель проектов
		 */
		QAbstractItemModel* model() const;

	signals:
		/**
		 * @brief Был выбран проект для открытия
		 */
		void clicked(const QModelIndex& _projectIndex);

		/**
		 * @brief Удалить пользователя с заданным адресом электронной почты
		 */
		void removeUserRequested(const QModelIndex& _projectIndex, const QString& _email);

	private:
		/**
		 * @brief Обработать выбор проекта
		 */
		void handleProjectClick();

		/**
		 * @brief Обработать закрытие доступа к проекту для пользователя
		 */
		void handleRemoveUserRequest(const QString& _email);

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

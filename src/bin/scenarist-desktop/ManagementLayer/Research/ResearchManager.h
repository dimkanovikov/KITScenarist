#ifndef RESEARCHMANAGER_H
#define RESEARCHMANAGER_H

#include <QObject>

namespace UserInterface {
	class ResearchView;
	class ResearchItemDialog;
}

namespace Domain {
	class Research;
}

namespace BusinessLogic {
	class ResearchModel;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий разработкой
	 */
	class ResearchManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ResearchManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить данные текущего проекта
		 */
		void loadCurrentProject();

		/**
		 * @brief Очистить все загруженные данные
		 */
		void closeCurrentProject();

		/**
		 * @brief Сохранить локации проекта
		 */
		void saveResearch();

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Была изменена локация
		 */
		void researchChanged();

		/**
		 * @brief Было изменено название локации
		 */
		void researchNameChanged(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Обновить список локаций
		 */
		void refreshResearch();

	private:
		/**
		 * @brief Добавить локацию
		 */
		void addResearch(const QModelIndex& _index);

		/**
		 * @brief Изменить локацию
		 */
		void editResearch(const QModelIndex& _index);

		/**
		 * @brief Удалить локации
		 */
		void removeResearch(const QModelIndex& _index);

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
		 * @brief Представление
		 */
		UserInterface::ResearchView* m_view;

		/**
		 * @brief Диалог добавления элемента разработки
		 */
		UserInterface::ResearchItemDialog* m_dialog;

		/**
		 * @brief Модель данных о разработке
		 */
		BusinessLogic::ResearchModel* m_model;

		/**
		 * @brief Текущий элемент разработки
		 */
		Domain::Research* m_currentResearch;
	};
}

#endif // RESEARCHMANAGER_H

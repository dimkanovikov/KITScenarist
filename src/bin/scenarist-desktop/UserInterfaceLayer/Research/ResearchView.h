#ifndef RESEARCHVIEW_H
#define RESEARCHVIEW_H

#include <QWidget>

namespace Ui {
	class ResearchView;
}

class QAbstractItemModel;


namespace UserInterface
{
	/**
	 * @brief Представление разработки
	 */
	class ResearchView : public QWidget
	{
		Q_OBJECT

	public:
		explicit ResearchView(QWidget *parent = 0);
		~ResearchView();

		/**
		 * @brief Загрузить модель разработки
		 */
		void setResearchModel(QAbstractItemModel* _model);

		/**
		 * @brief Текущий выбранный элемент в навигаторе
		 */
		QModelIndex currentResearchIndex() const;

		/**
		 * @brief Выделить элемент с заданным индексом
		 */
		void selectItem(const QModelIndex& _index);

		/**
		 * @brief Включить режим редактирования сценария
		 */
		void editScenario(const QString& _name, const QString& _logline);

		/**
		 * @brief Включить режим редактирования титульной страницы
		 */
		void editTitlePage(const QString& _name, const QString& _additionalInfo,
			const QString& _genre, const QString& _author, const QString& _contacts,
			const QString& _year);

		/**
		 * @brief Включить режим редактирования синопсиса
		 */
		void editSynopsis(const QString& _synopsis);

		/**
		 * @brief Включить режим редактирования корня папки разработки
		 */
		void editResearchRoot();

		/**
		 * @brief Включить режим редактирования тестового объекта разработки
		 */
		void editText(const QString& _name, const QString& _description);

	signals:
		/**
		 * @brief Нажата кнопка добавления элемента разработки
		 */
		void addResearchRequested(const QModelIndex& _currentIndex);

		/**
		 * @brief Нажата кнопка удаления элемента разработки
		 */
		void removeResearchRequested(const QModelIndex& _index);

		/**
		 * @brief Выбрана разработка для изменения
		 */
		void editResearchRequested(const QModelIndex& _index);

		/**
		 * @brief Сигналы об изменении данных
		 */
		/** @{ */
		void scenarioNameChanged(const QString& _name);
		void scenarioLoglineChanged(const QString& _logline);
		void titlePageAdditionalInfoChanged(const QString& _additionalInfo);
		void titlePageGenreChanged(const QString& _genre);
		void titlePageAuthorChanged(const QString& _author);
		void titlePageContactsChanged(const QString& _contacts);
		void titlePageYearChanged(const QString& _year);
		void synopsisTextChanged(const QString& _synopsis);
		void textNameChanged(const QString& _name);
		void textDescriptionChanged(const QString& _description);
		/** @{ */

	private:
		/**
		 * @brief Скрыть/показать кнопки добавления и удаления разработки
		 */
		void setResearchManageButtonsVisible(bool _isVisible);

		/**
		 * @brief Изменён текущий объект разработки
		 */
		void currentResearchChanged();

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
		Ui::ResearchView* m_ui;
	};
}

#endif // RESEARCHVIEW_H

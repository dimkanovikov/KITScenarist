#ifndef STARTUPVIEW_H
#define STARTUPVIEW_H

#include <QWidget>

class QAbstractItemModel;

namespace Ui {
	class StartUpView;
}


namespace UserInterface
{
	/**
	 * @brief Класс формы приветствия программы
	 */
	class StartUpView : public QWidget
	{
		Q_OBJECT

	public:
		explicit StartUpView(QWidget *parent = 0);
		~StartUpView();

		/**
		 * @brief Установить недавно использованные файлы
		 */
		void setRecentProjects(QAbstractItemModel* _recentProjectsModel);

		/**
		 * @brief Обновить информацию о доступности обновлений
		 */
		void setUpdateInfo(const QString& _updateInfo);

		/**
		 * @brief Установить информацию о том, авторизован пользователь или нет
		 */
		void setUserLogged(bool isLogged, const QString& _userName = QString::null);

		/**
		 * @brief Установить список доступных проектов
		 */
		void setRemoteProjects(QAbstractItemModel* _remoteProjectsModel);

	signals:
		/**
		 * @brief Нажата кнопка войти
		 */
		void loginClicked();

		/**
		 * @brief Нажата кнопка выйти
		 */
		void logoutClicked();

		/**
		 * @brief Нажата кнопка создать проект
		 */
		void createProjectClicked();

		/**
		 * @brief Нажата кнопка открыть проект
		 */
		void openProjectClicked();

		/**
		 * @brief Нажата кнопка помощь
		 */
		void helpClicked();

		/**
		 * @brief Выбран один из недавно используемых проектов для открытия
		 */
		void openRecentProjectClicked(const QModelIndex& _projectIndex);

		/**
		 * @brief Выбран один из проектов из облака для открытия
		 */
		void openRemoteProjectClicked(const QModelIndex& _projectIndex);

		/**
		 * @brief Нажата кнопка обновления недавних файлов
		 */
		void refreshProjects();

	protected:
		/**
		 * @brief Переопределяем для обновления цвета иконок, при смене палитры
		 */
		bool event(QEvent* _event);

	private slots:
		/**
		 * @brief Пользователь сменил источник отображаемых проектов
		 */
		void aboutFilesSourceChanged();

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

		/**
		 * @brief Настроить цвет иконок
		 */
		void initIconsColor();

	private:
		Ui::StartUpView *ui;
	};
}

#endif // STARTUPVIEW_H

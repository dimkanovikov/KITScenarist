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
		void setRecentFiles(
				const QMap<QString, QString>& _recentFiles,
				const QMap<QString, QString>& _recentFilesUsing);

		/**
		 * @brief Обновить информацию о доступности обновлений
		 */
		void setUpdateInfo(const QString& _updateInfo);

		/**
		 * @brief Установить информацию о том, авторизован пользователь или нет
		 */
		void setUserLogged(bool isLogged, const QString& _userName = QString::null);

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
		 * @brief Выбран один из недавних файлов для открытия
		 */
		void openRecentProjectClicked(const QString& _filePath);

		/**
		 * @brief Нажата кнопка обновления недавних файлов
		 */
		void refreshRecentFiles();

	protected:
		/**
		 * @brief Переопределяется для фиксации события когда мышка покидает виджет недавних проектов
		 */
		bool eventFilter(QObject* _watched, QEvent* _event);

	private slots:
		/**
		 * @brief Пользователь выбрал один из недавних файлов для открытия
		 */
		void aboutOpenRecentFileClicked();

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
		Ui::StartUpView *ui;
	};
}

#endif // STARTUPVIEW_H

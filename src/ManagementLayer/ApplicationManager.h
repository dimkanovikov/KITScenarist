#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QObject>

class ScenarioTextEdit;
class QLabel;

namespace ManagementLayer
{
	class StartUpManager;

	/**
	 * @brief Управляющий приложением
	 */
	class ApplicationManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ApplicationManager(QObject *parent = 0);
		~ApplicationManager();

		/**
		 * @brief Запуск приложения
		 */
		void exec();

	private slots:
		/**
		 * @brief Создать новый
		 */
		void aboutCreateNew();

		/**
		 * @brief Сохранить как...
		 */
		void aboutSaveAs();

		/**
		 * @brief Сохранить в файл
		 */
		void aboutSave();

		/**
		 * @brief Загрузить
		 *
		 * Если имя файла не задано, то будет вызван диалог выбора файла
		 */
		void aboutLoad(const QString& _fileName = QString());

	private:
		/**
		 * @brief Если проект был изменён, но не сохранён предложить пользователю сохранить его
		 */
		void saveIfNeeded();

		/**
		 * @brief Сохранить текущий проект в недавно используемых
		 */
		void saveCurrentProjectInRecent();

	private:
		void initConnections();

	private:
		/**
		 * @brief Главное окно приложения
		 */
		QWidget* m_view;

		/**
		 * @brief Управляющий стартовой страницей
		 */
		StartUpManager* m_startUpManager;



		/// tests

	public slots:
		void print();
		void updatePositionDuration();

	private:
		ScenarioTextEdit* textEdit;
		QLabel* label;
	};
}

#endif // APPLICATIONMANAGER_H

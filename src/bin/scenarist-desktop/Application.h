#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QTimer>

namespace ManagementLayer {
	class ApplicationManager;
}


/**
 * @brief Класс приложения переопределяется для обработки события открытия файла в Mac OS
 */
class Application : public QApplication
{
	Q_OBJECT

public:
	explicit Application(int& _argc, char** _argv);

	/**
	 * @brief Запуск приложения
	 */
	void setupManager(ManagementLayer::ApplicationManager* _manager);

	/**
	 * @brief Переопределяется для определения события простоя приложения (idle)
	 */
	bool notify(QObject* _object, QEvent* _event);

protected:
	/**
	 * @brief Переопределяется для обработки события открытия файла в Mac OS
	 */
	bool event(QEvent* _event);

private:
	/**
	 * @brief Настроить перевод приложения
	 */
	void initTranslation();

private:
	/**
	 * @brief Управляющий приложением
	 */
	ManagementLayer::ApplicationManager* m_applicationManager;

	/**
	 * @brief Таймер для определения события простоя
	 */
	QTimer m_idleTimer;
};

#endif // APPLICATION_H

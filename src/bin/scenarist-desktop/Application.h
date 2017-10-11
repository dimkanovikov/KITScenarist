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
    ~Application();

    /**
     * @brief Настроить перевод приложения
     */
    void updateTranslation();

    /**
     * @brief Запустить приложение
     */
    void startApp();

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

private:
	/**
	 * @brief Управляющий приложением
	 */
    ManagementLayer::ApplicationManager* m_applicationManager = nullptr;

	/**
	 * @brief Таймер для определения события простоя
	 */
	QTimer m_idleTimer;

    /**
     * @brief Файл, который надо открыть с запуском приложения
     */
    QString m_fileToOpen;
};

#endif // APPLICATION_H

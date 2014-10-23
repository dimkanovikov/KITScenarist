#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

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

protected:
    /**
     * @brief Переопределяется для обработки события открытия файла в Mac OS
     */
    bool event(QEvent* _event);

private:
    /**
     * @brief Управляющий приложением
     */
    ManagementLayer::ApplicationManager* m_applicationManager;
};

#endif // APPLICATION_H

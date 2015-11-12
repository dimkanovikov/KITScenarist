#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

namespace ManagementLayer {
    class ApplicationManager;
}


/**
 * @brief Класс приложения
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
};

#endif // APPLICATION_H

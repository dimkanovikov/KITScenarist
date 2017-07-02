#ifndef ADDPROJECTDIALOG_H
#define ADDPROJECTDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
    class AddProjectDialog;
}

class QPushButton;


namespace UserInterface
{
    /**
     * @brief Класс диалога авторизации пользователя
     */
    class AddProjectDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit AddProjectDialog(QWidget* _parent, bool _hasInternetConnection);
        ~AddProjectDialog();

        /**
         * @brief Установить возможность добавления проектов из облака
         * @param _isAvailable - показывать ли переключатель облачных проектов
         * @param _isEnabled - доступен ли переключатель облачных проектов
         * @param _isSelected - выбран ли в данных момент переключатель облачных проектов
         */
        void setIsRemoteAvailable(bool _isAvailable, bool _isEnabled = false, bool _isSelected = false);

        /**
         * @brief Создаётся локальный файл (true) или в облаке (false)
         */
        bool isLocal() const;

        /**
         * @brief Название создаваемого проекта
         */
        QString projectName() const;

        /**
         * @brief Путь создаваемого файла
         */
        QString projectFilePath() const;

        /**
         * @brief Файл из которого нужно экспоритровать проект
         */
        QString importFilePath() const;

    protected:
        /**
         * @brief Переопределяем для установки фокуса
         */
        QWidget* focusedOnExec() const override;

    private:
        /**
         * @brief Настроить видимость метки с сообщением о невозможности создать проект в облаке
         */
        void setCloudCreateBlockerVisible(bool _visible);

    private:
        /**
         * @brief Настроить представление
         */
        void initView() override;

        /**
         * @brief Настроить соединения для формы
         */
        void initConnections() override;

        /**
         * @brief Настроить стили
         */
        void initStyleSheet();

    private:
        /**
         * @brief Интерфейс
         */
        Ui::AddProjectDialog* m_ui;

        /**
         * @brief Есть ли подключение к облачному сервису
         */
        bool m_hasInternetConnection = false;

        /**
         * @brief КНопка создания проекта
         */
        QPushButton* m_createProjectButton = nullptr;
    };
}

#endif // ADDPROJECTDIALOG_H

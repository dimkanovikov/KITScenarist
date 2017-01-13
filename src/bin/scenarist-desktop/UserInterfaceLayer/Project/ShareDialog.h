#ifndef SHAREDIALOG_H
#define SHAREDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
    class ShareDialog;
}

namespace UserInterface
{
    /**
     * @brief Класс диалога открытия доступа к проекту
     */
    class ShareDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit ShareDialog(QWidget *parent = 0);
        ~ShareDialog();

        /**
         * @brief Адрес электронной почты, для которого открывается доступ к проекту
         */
        QString email() const;

        /**
         * @brief Роль пользователя в проекте
         */
        int role() const;

    protected:
        /**
         * @brief Переопределяем, чтобы фокус устанавливался в поле ввода email
         */
        QWidget* focusedOnExec() const override;

    private:
        /**
         * @brief Настроить представление
         */
        void initView() override;

        /**
         * @brief Настроить соединения для формы
         */
        void initConnections() override;

    private:
        /**
         * @brief Интерфейс
         */
        Ui::ShareDialog* m_ui;
    };
}

#endif // SHAREDIALOG_H

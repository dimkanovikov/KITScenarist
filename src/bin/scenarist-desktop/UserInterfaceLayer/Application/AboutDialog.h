#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
    class AboutDialog;
}


namespace UserInterface
{
    /**
     * @brief Диалог с информацией о программе
     */
    class AboutDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit AboutDialog(QWidget* _parent = nullptr);
        ~AboutDialog();

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
        Ui::AboutDialog* m_ui = nullptr;
    };
}

#endif // ABOUTDIALOG_H

#ifndef PRINTCARDSDIALOG_H
#define PRINTCARDSDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
    class PrintCardsDialog;
}

namespace UserInterface
{
    /**
     * @brief Диалог печати карточек
     */
    class PrintCardsDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit PrintCardsDialog(QWidget *parent = 0);
        ~PrintCardsDialog();

        /**
         * @brief Количество карточек на странице
         */
        int cardsCount() const;

        /**
         * @brief Портретная ли ориентация страницы
         */
        bool isPortrait() const;

        /**
         * @brief Печатать карточки цветными
         */
        bool printColorCards() const;

    signals:
        /**
         * @brief Запрос на предварительный просмотр печатаемых карточек
         */
        void printPreview();

    private:
        /**
         * @brief Настроить представление
         */
        void initView() override;

        /**
         * @brief Настроить соединения
         */
        void initConnections() override;

    private:
        /**
         * @brief Интерфейс
         */
        Ui::PrintCardsDialog* m_ui;
    };
}

#endif // PRINTCARDSDIALOG_H

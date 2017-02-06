#ifndef RESEARCHITEMDIALOG_H
#define RESEARCHITEMDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
    class ResearchItemDialog;
}


namespace UserInterface
{
    /**
     * @brief Диалог добаления элемента разработки
     */
    class ResearchItemDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit ResearchItemDialog(QWidget* _parent = 0);
        ~ResearchItemDialog();

        /**
         * @brief Очистить диалог
         */
        void clear();

        /**
         * @brief Установить возможность вставки элемент разработки в родителя
         * @note Должен устанавливаться перед setInsertAllow, т.к. он настраивает доступность вставки
         */
        void setInsertParent(const QString& _parentName = QString::null);

        /**
         * @brief Установить возможность добавления персонажа и локации
         */
        void setInsertAllow(bool _isCharacterAllow, bool _isLocationAllow);

        /**
         * @brief Задать тип разработки
         */
        void setResearchType(int _type);

        /**
         * @brief Тип разработки
         */
        int researchType() const;

        /**
         * @brief Название разработки
         */
        QString researchName() const;

        /**
         * @brief Нужно ли встраивать в родительский элемент
         */
        bool insertResearchInParent() const;

    protected:
        /**
         * @brief Указываем виджет на который нужно установить фокус при отображении диалога
         */
        QWidget* focusedOnExec() const override;

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
        Ui::ResearchItemDialog* m_ui;
    };
}

#endif // RESEARCHITEMDIALOG_H

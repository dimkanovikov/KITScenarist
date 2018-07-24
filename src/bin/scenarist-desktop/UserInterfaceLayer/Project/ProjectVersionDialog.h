#ifndef PROJECTVERSIONDIALOG_H
#define PROJECTVERSIONDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
    class ProjectVersionDialog;
}


namespace UserInterface
{
    /**
     * @brief Диалог сохранения версии сценария
     */
    class ProjectVersionDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit ProjectVersionDialog(QWidget *parent = nullptr);
        ~ProjectVersionDialog();

        /**
         * @brief Дата и время версии
         */
        QDateTime versionDateTime() const;

        /**
         * @brief Название версии
         */
        QString versionName() const;

        /**
         * @brief Цвет версии
         */
        QColor versionColor() const;

        /**
         * @brief Описание версии
         */
        QString versionDescription() const;

    protected:
        /**
         * @brief Переопределяем для установки фокуса
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

        /**
         * @brief Настроить стили
         */
        void initStyleSheet();

    private:
        /**
         * @brief Интерфейс
         */
        Ui::ProjectVersionDialog* m_ui = nullptr;
    };
}

#endif // PROJECTVERSIONDIALOG_H

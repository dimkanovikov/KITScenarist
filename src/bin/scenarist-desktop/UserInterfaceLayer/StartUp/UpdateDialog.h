#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
class UpdateDialog;
}

namespace UserInterface {
    class UpdateDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit UpdateDialog(QWidget *parent = 0);
        ~UpdateDialog();

        /**
         * @brief Донастроить окно и показать его
         */
        int showUpdate(const QString& _version, const QString& _text,
                       bool _isBeta);

        /**
         * @brief Обновить прогресс загрузки
         */
        void updateProgress(int _progress);

        /**
         * @brief Завершился прогресс загрузки
         */
        void downloadFinished();

    signals:
        /**
         * @brief Пропустить обновление
         */
        void skipUpdate();

        /**
         * @brief Загрузить обновление
         */
        void downloadUpdate();

        /**
         * @brief Установить обновление
         */
        void installUpdate();

    private:
        /**
         * @brief Первоначальная настройка окна
         */
        void initView();

        /**
         * @brief Первоначальные коннекты
         */
        void initConnections();

        /**
         * @brief Первоначальные стили
         */
        void initStylesheet();

    private:
        Ui::UpdateDialog *m_ui;

        /**
         * @brief Файл уже загрузился
         */
        bool m_downloaded;

        /**
         * @brief Пришло бета обновление или нет
         */
        bool m_isBeta;
    };
}

#endif // UPDATEDIALOG_H

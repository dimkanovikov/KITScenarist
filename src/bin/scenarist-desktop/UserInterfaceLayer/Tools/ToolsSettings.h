#ifndef TOOLSSETTINGS_H
#define TOOLSSETTINGS_H

#include <QWidget>

class FlatButton;
class QAbstractItemModel;
class QLightBoxProgress;

namespace Ui {
    class ToolsSettings;
}


namespace UserInterface
{
    class ToolsSettings : public QWidget
    {
        Q_OBJECT

    public:
        explicit ToolsSettings(QWidget *parent = 0);
        ~ToolsSettings();

        /**
         * @brief Установить заголовок
         */
        void setTitle(const QString& _title);

        /**
         * @brief Установить текущий вид инструмента
         */
        void setCurrentType(int _index);

        /**
         * @brief Установить список бэкапов
         */
        void setBackupsModel(QAbstractItemModel* _model);

    signals:
        /**
         * @brief Пользователь нажал кнопку выйти из настроек инструментов
         */
        void backPressed();

        /**
         * @brief Пользователь выбрал бэкап для восстановления
         */
        void backupSelected(const QModelIndex& _backupItemIndex);

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения для формы
         */
        void initConnections();

        /**
         * @brief Настроить внешний вид
         */
        void initStyleSheet();

    private:
        /**
         * @brief Интерфейс
         */
        Ui::ToolsSettings* m_ui = nullptr;

        /**
         * @brief Кнопка "назад к списку инструментов"
         */
        FlatButton* m_back = nullptr;

        /**
         * @brief Виджет отображения прогресса загрузки данных в компоненте
         */
        QLightBoxProgress* m_loadingIndicator = nullptr;
    };
}

#endif // TOOLSSETTINGS_H

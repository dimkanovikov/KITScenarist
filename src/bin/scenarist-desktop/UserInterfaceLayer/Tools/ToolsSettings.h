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
        explicit ToolsSettings(QWidget* _parent = nullptr);
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

        /**
         * @brief Установить список версий
         */
        void setScriptVersionsModel(QAbstractItemModel* _model);

    signals:
        /**
         * @brief Пользователь нажал кнопку выйти из настроек инструментов
         */
        void backPressed();

        /**
         * @brief Пользователь выбрал бэкап для восстановления
         */
        void backupSelected(const QModelIndex& _backupItemIndex);

        /**
         * @brief Выбраны версии для сравнения
         */
        void versionsForCompareSelected(int _firstVersionIndex, int _secondVersionIndex);

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

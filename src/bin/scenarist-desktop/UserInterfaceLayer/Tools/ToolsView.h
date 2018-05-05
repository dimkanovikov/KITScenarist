#ifndef TOOLSVIEW_H
#define TOOLSVIEW_H

#include <QWidget>

class FlatButton;
class QAbstractItemModel;
class QLabel;
class QTreeWidget;
class QTreeWidgetItem;
class QStackedWidget;
class ScalableWrapper;

namespace BusinessLogic {
    class ScenarioTextDocument;
}

namespace UserInterface
{
    class ScenarioTextEdit;
    class ToolsSettings;


    /**
     * @brief Представление страницы инструментов
     */
    class ToolsView : public QWidget
    {
        Q_OBJECT

    public:
        explicit ToolsView(QWidget* _parent = nullptr);

        /**
         * @brief Сбросить представление
         */
        void reset();

        /**
         * @brief Включить/выключить отображение номеров сцен
         */
        void setShowScenesNumbers(bool _show);

        /**
         * @brief Включить/выключить отображение номеров реплик
         */
        void setShowDialoguesNumbers(bool _show);

        /**
         * @brief Настроить цвета текстового редактора
         */
        void setTextEditColors(const QColor& _textColor, const QColor& _backgroundColor);

        /**
         * @brief Настроить масштабирование редактора сценария
         */
        void setTextEditZoomRange(qreal _zoomRange);

        /**
         * @brief Задать поясняющий текст панели с данными
         */
        void showPlaceholderText(const QString& _text);

        /**
         * @brief Отобразить редактор сценария
         */
        void showScript();

        /**
         * @brief Задать документ сценария
         */
        void setScriptDocument(BusinessLogic::ScenarioTextDocument* _document);

        /**
         * @brief Установить список бэкапов
         */
        void setBackupsModel(QAbstractItemModel* _model);

    signals:
        /**
         * @brief Необходимо загрузить данные для заданного инструмента
         */
        void dataRequested(int _toolIndex);

        /**
         * @brief Пользователь выбрал бэкап для восстановления
         */
        void backupSelected(const QModelIndex& _backupItemIndex);

        /**
         * @brief Пользователь хочет применить текущий сценарий
         */
        void applyScriptRequested();

    private:
        /**
         * @brief Активировать инстумент
         */
        void activateTool(QTreeWidgetItem* _toolItem);

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
         * @brief Заполнители верхней панели
         */
        /** @{ */
        QLabel* m_leftTopEmptyLabel = nullptr;
        QLabel* m_rightTopEmptyLabel = nullptr;
        /** @} */

        /**
         * @brief Кнопка "настройки инструмента"
         */
        FlatButton* m_settings = nullptr;

        /**
         * @brief Кнопка "восстановить"
         */
        FlatButton* m_restore = nullptr;

        /**
         * @brief Панель навигации
         */
        QStackedWidget* m_navigation = nullptr;

        /**
         * @brief Панель с видами инструментов
         */
        QTreeWidget* m_toolsTypes = nullptr;

        /**
         * @brief Панель параметров инструментов
         */
        ToolsSettings* m_toolsSettings = nullptr;

        /**
         * @brief Панель данных инструментов
         */
        QStackedWidget* m_content = nullptr;

        /**
         * @brief Вспомогательный текст для правой панели с контентом
         */
        QLabel* m_placeholder = nullptr;

        /**
         * @brief Редактор текста сценария для отображения результатов работы инструментов
         */
        ScenarioTextEdit* m_editor = nullptr;

        /**
         * @brief Обёртка редактора, позволяющая его масштабировать
         */
        ScalableWrapper* m_editorWrapper = nullptr;
    };
}

#endif // TOOLSVIEW_H

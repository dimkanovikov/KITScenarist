#ifndef SCENARIOITEMDIALOG_H
#define SCENARIOITEMDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

#include <BusinessLayer/ScenarioDocument/ScenarioModelItem.h>

class QPushButton;

namespace Ui {
    class ScenarioSchemeItemDialog;
}


namespace UserInterface
{
    /**
     * @brief Диалог редактирования элементов сценария
     */
    class ScenarioItemDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit ScenarioItemDialog(QWidget* _parent = nullptr);
        ~ScenarioItemDialog();

        /**
         * @brief Подготовить к добавлению
         */
        void prepareForAdding(bool useFolders = true);

        /**
         * @brief Подготовить к изменению
         */
        void prepareForEditing();

        /**
         * @brief Восстановить значения по умолчанию
         */
        void clear();

        /**
         * @brief Получить тип элемента
         */
        BusinessLogic::ScenarioModelItem::Type itemType() const;

        /**
         * @brief Установить тип элемента
         * @note При этом блокируется возможность выбора типа
         */
        void setItemType(BusinessLogic::ScenarioModelItem::Type _type);

        /**
         * @brief Получить название элемента
         */
        QString itemName() const;

        /**
         * @brief Установить название элемента
         */
        void setItemName(const QString& _name);

        /**
         * @brief Получить заголовок элемента
         */
        QString itemHeader() const;

        /**
         * @brief Установить заголовок элемента
         */
        void setItemHeader(const QString& _header);

        /**
         * @brief Получить описание элемента
         */
        QString itemDescription() const;

        /**
         * @brief Установить описание элемента
         */
        void setItemDescription(const QString& _description);

        /**
         * @brief Получить цвет элемента
         */
        QString itemColor() const;

        /**
         * @brief Установить цвет элемента
         */
        void setItemColor(const QString& _color);

    protected:
        /**
         * @brief Виджет на который нужно установить фокус при отображении
         */
        QWidget* focusedOnExec() const override;

        /**
         * @brief Табы в заголовке окна
         */
        QWidget* titleWidget() const override;

    private:
        /**
         * @brief Настроить вкладки
         */
        void initTabs();

        /**
         * @brief Настроить представление
         */
        void initView() override;

        /**
         * @brief Настроить соединения
         */
        void initConnections() override;

        /**
         * @brief Настроить внешний вид диалога
         */
        void initStyleSheet();

        /**
         * @brief Проверить возможно ли сохранение
         */
        void checkSavingAvailable();

    private:
        /**
         * @brief Интерфейс
         */
        Ui::ScenarioSchemeItemDialog* m_ui = nullptr;

        /**
         * @brief Кнопка сохранения
         */
        QPushButton* m_saveButton = nullptr;
    };
}

#endif // SCENARIOITEMDIALOG_H

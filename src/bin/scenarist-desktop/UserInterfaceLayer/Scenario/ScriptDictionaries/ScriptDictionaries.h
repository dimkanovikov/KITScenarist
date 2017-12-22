#ifndef SCRIPTDICTIONARIES_H
#define SCRIPTDICTIONARIES_H

#include <QWidget>

class FlatButton;
class QComboBox;
class QListView;


namespace UserInterface
{
    /**
     * @brief Представление редактора справочников сценария
     */
    class ScriptDictionaries : public QWidget
    {
        Q_OBJECT

    public:
        explicit ScriptDictionaries(QWidget* _parent = nullptr);

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

        /**
         * @brief Настроить внешний вид
         */
        void initStyleSheet();

    private:
        /**
         * @brief Количество сцен в сценарии
         */
        QComboBox* m_types = nullptr;

        /**
         * @brief Кнопка добавления элемента
         */
        FlatButton* m_addItem = nullptr;

        /**
         * @brief Кнопка удаления выбранного элемента
         */
        FlatButton* m_removeItem = nullptr;

        /**
         * @brief Дерево навигации
         */
        QListView* m_items = nullptr;
    };
}

#endif // SCRIPTDICTIONARIES_H

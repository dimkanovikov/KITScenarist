#ifndef SCRIPTDICTIONARIESMANAGER_H
#define SCRIPTDICTIONARIESMANAGER_H

#include <QObject>

namespace UserInterface {
    class ScriptDictionaries;
}


namespace ManagementLayer
{
    /**
     * @brief Управляющий редактором справочников сценария
     */
    class ScriptDictionariesManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ScriptDictionariesManager(QObject* _parent, QWidget* _parentWidget);

        QWidget* view() const;

        /**
         * @brief Сбросить состояние представления
         */
        void refresh();

        /**
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

    private:
        /**
         * @brief Сменить отображаемый справочник
         */
        void changeDictionary(int _type);

        /**
         * @brief Добавить элемент
         */
        void addItem();

        /**
         * @brief Удалить элемент
         */
        void removeItem(const QModelIndex& _index);

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения
         */
        void initConnection();

    private:
        /**
         * @brief Представление
         */
        UserInterface::ScriptDictionaries* m_view = nullptr;

        /**
         * @brief Последний используемый тип справочника
         */
        int m_currentType = 0;
    };
}

#endif // SCRIPTDICTIONARIESMANAGER_H

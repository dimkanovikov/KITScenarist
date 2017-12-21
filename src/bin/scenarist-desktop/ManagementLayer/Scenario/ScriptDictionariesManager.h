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
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

    private:
        void initView();

    private:
        /**
         * @brief Представление
         */
        UserInterface::ScriptDictionaries* m_view = nullptr;
    };
}

#endif // SCRIPTDICTIONARIESMANAGER_H

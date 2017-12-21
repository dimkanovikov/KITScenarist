#ifndef SCRIPTDICTIONARIES_H
#define SCRIPTDICTIONARIES_H

#include <QWidget>


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

    signals:

    public slots:
    };
}

#endif // SCRIPTDICTIONARIES_H

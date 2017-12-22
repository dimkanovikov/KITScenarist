#ifndef SCRIPTDICTIONARIES_H
#define SCRIPTDICTIONARIES_H

#include <QWidget>

class FlatButton;
class QAbstractItemModel;
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
        /**
         * @brief Типы справочников
         */
        enum class Type {
            SceneIntros,
            SceneTimes,
            SceneDays,
            CharacterStates,
            Transitions
        };

    public:
        explicit ScriptDictionaries(QWidget* _parent = nullptr);

        /**
         * @brief Установить модель справочника
         */
        void setModel(QAbstractItemModel* _model);

        /**
         * @brief Установить режим работы со сценарием
         */
        void setCommentOnly(bool _isCommentOnly);

    signals:
        /**
         * @brief Сменился тип справочика
         */
        void dictionaryChanged(int _type);

        /**
         * @brief Пользователь хочет добавить новый элемент
         */
        void addItemRequested();

        /**
         * @brief Пользователь хочет удалить выбранный элемент
         */
        void removeItemRequested(const QModelIndex& _index);

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

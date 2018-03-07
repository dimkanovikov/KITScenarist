#ifndef SCRIPTZENMODECONTROLS_H
#define SCRIPTZENMODECONTROLS_H

#include <QFrame>
#include <QTimer>

class FlatButton;
class QCheckBox;
class QGraphicsOpacityEffect;
class QPropertyAnimation;
class QPushButton;
class QLabel;


namespace UserInterface
{
    class ScenarioTextEdit;


    /**
     * @brief Виджет элементов управления в дзен режиме
     */
    class ScriptZenModeControls : public QFrame
    {
        Q_OBJECT

    public:
        explicit ScriptZenModeControls(QWidget* _parent = nullptr);

        /**
         * @brief Установить редактор
         */
        void setEditor(ScenarioTextEdit* _editor);

        /**
         * @brief Переформировать список быстрых форматов в соответствии с текущим стилем
         */
        void reinitBlockStyles();

        /**
         * @brief Сменить стиль блока
         */
        void changeStyle();

        /**
         * @brief Обновить кнопки стилей
         */
        void updateStyleButtons();

        /**
         * @brief Настроить элементы управления для работы
         */
        void activate(bool _active);

        /**
         * @brief Установить счетчики
         */
        void setCountersInfo(const QString& _counters);

    signals:
        /**
         * @brief Нажата кнопка выхода из дзен режима
         */
        void quitPressed();

    protected:
        /**
         * @brief Переопределяем, чтобы следить за размером родителя и движением мыши
         */
        bool eventFilter(QObject* _watched, QEvent* _event) override;

    private:
        /**
         * @brief Плавно показать
         */
        void showAnimated();

        /**
         * @brief Плавно скрыть
         */
        void hideAnimated();

        /**
         * @brief Настроить анимацию прозрачности виджета
         */
        QPropertyAnimation* configureOpacityAnimation(qreal _startOpacity, qreal _endOpacity);

    private:
        /**
         * @brief Активен ли элемент управления в данный момент
         */
        bool m_active = false;

        /**
         * @brief Редактор сценария для форматирования блоков
         */
        ScenarioTextEdit* m_editor = nullptr;

        /**
         * @brief Кнопка выхода из дзен режима
         */
        FlatButton* m_quit = nullptr;

        /**
         * @brief Список кнопок
         */
        QList<QPushButton*> m_buttons;

        /**
         * @brief Счетчики
         */
        QLabel* m_countersInfo;

        /**
         * @brief Переключатель воспроизведения звука клавиатуры
         */
        QCheckBox* m_keyboardSound = nullptr;

        /**
         * @brief Таймер скрытия виджета
         */
        QTimer m_hideTimer;
    };
}

#endif // SCRIPTZENMODECONTROLS_H

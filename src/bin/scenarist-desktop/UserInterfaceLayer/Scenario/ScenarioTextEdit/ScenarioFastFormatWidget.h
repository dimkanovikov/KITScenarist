#ifndef SCENARIOFASTFORMATWIDGET_H
#define SCENARIOFASTFORMATWIDGET_H

#include <QFrame>

class QPushButton;
class QCheckBox;


namespace UserInterface
{
    class ScenarioTextEdit;


    /**
     * @brief Виджет быстрого форматирования
     */
    class ScenarioFastFormatWidget : public QFrame
    {
        Q_OBJECT

    public:
        explicit ScenarioFastFormatWidget(QWidget *parent = 0);

        /**
         * @brief Установить редактор
         */
        void setEditor(ScenarioTextEdit* _editor);

        /**
         * @brief Переформировать список быстрых форматов в соответствии с текущим стилем
         */
        void reinitBlockStyles();

    signals:
        /**
         * @brief Фокус смещён к редактору
         */
        void focusMovedToEditor();

    private slots:
        /**
         * @brief Перейти к следующему блоку
         */
        void aboutGoToNextBlock();

        /**
         * @brief Перейти к предыдущему блоку
         */
        void aboutGoToPrevBlock();

        /**
         * @brief Сменить стиль блока
         */
        void aboutChangeStyle();

        /**
         * @brief Сменилась позиция курсора
         */
        void aboutCurrentStyleChanged();

    private:
        /**
         * @brief Редактор сценария для форматирования блоков
         */
        ScenarioTextEdit* m_editor = nullptr;

        /**
         * @brief Список кнопок
         */
        QList<QPushButton*> m_buttons;
    };
}

#endif // SCENARIOFASTFORMATWIDGET_H

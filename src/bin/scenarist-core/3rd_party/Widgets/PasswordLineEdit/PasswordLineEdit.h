#ifndef PASSWORDLINEEDIT_H
#define PASSWORDLINEEDIT_H

#include <QLineEdit>

class QToolButton;

class PasswordLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    PasswordLineEdit(QWidget* _parent = 0);

    /**
     * @brief Сбрасывает поле к звездочкам
     */
    void resetAsterisk();

protected:
    /**
     * @brief Переопределим, чтобы добавить глаз
     */
    void resizeEvent(QResizeEvent* _event) override;

private slots:
    /**
     * @brief Пользователь нажал на глаз
     *        Сменим отображение
     */
    void eyeClicked();

private:
    /**
     * @brief Кнопка-глаз переключения
     */
    QToolButton* m_eye;

    /**
     * @brief Сейчас отображаются звездочки или нет
     */
    bool m_isAsterisk;
};

#endif // PASSWORDLINEEDIT_H

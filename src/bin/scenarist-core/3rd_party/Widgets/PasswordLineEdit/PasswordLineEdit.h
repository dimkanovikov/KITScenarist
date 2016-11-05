/*
* Copyright (C) 2016 Alexey Polushkin, armijo38@yandex.ru
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* Full license: http://dimkanovikov.pro/license/GPLv3
*/

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

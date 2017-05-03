#ifndef QLIGHTBOXDIALOG_H
#define QLIGHTBOXDIALOG_H

#include "qlightboxwidget.h"

class QFrame;
class QLabel;
class QProgressBar;


/**
 * @brief Диалог перекрывающий родительское окно
 */
class QLightBoxDialog : public QLightBoxWidget
{
    Q_OBJECT

public:
    /**
     * @brief Коды возврата
     */
    enum DialogCode {
        Accepted = 1,
        Rejected = 0
    };

public:
    explicit QLightBoxDialog(QWidget *parent = 0, bool _followToHeadWidget = true,
        bool _isContentStretchable = false);

public slots:
    /**
     * @brief Открыть диалог заблокировав выполнение цикла событий приложения
     */
    int exec();

    /**
     * @brief Переопределяем, чтобы инициилизировать диалог, перед отображением
     */
    void setVisible(bool _visible);

    /**
     * @brief Позитивное и негативное закрытие диалога
     */
    /** @{ */
    void accept();
    void reject();
    /** @} */

    /**
     * @brief Закрыть диалог с установленным кодом возврата
     */
    void done(int _result);

    /**
     * @brief Виден ли прогрессбар в данный момент
     */
    bool isProressVisible() const;

    /**
     * @brief Показать прогрессбар с установленными параметрами
     */
    void showProgress(int _minimumValue = 0, int _maximumValue = 0);

    /**
     * @brief Установить текущее значение прогресса
     */
    void setProgressValue(int _value);

    /**
     * @brief Скрыть прогрессбар
     */
    void hideProgress();

signals:
    /**
     * @brief Сигналы о позитивном и негативном закрытии диалога
     */
    /** @{ */
    void accepted();
    void rejected();
    /** @} */

    /**
     * @brief Диалог закрыт с заданным кодом возврата
     */
    void finished(int _result);

protected:
    /**
     * @brief Переопределяется для перехвата нажатия Enter и Escape
     */
    bool event(QEvent* _event);

    /**
     * @brief Настроить представление
     */
    virtual void initView() = 0;

    /**
     * @brief Настроить соединения
     */
    virtual void initConnections() = 0;

    /**
     * @brief Виджет заголовок
     */
    virtual QWidget* titleWidget() const;

    /**
     * @brief Виджет на который нужно установить фокус при отображении
     */
    virtual QWidget* focusedOnExec() const;

private:
    /**
     * @brief Инициилизировать диалог
     */
    void init();

    /**
     * @brief Обновить заголовок
     */
    void updateTitle();

private:
    /**
     * @brief Инициилизирован ли диалог
     */
    bool m_initialized;

    /**
     * @brief Заголовок диалога
     */
    QLabel* m_title;

    /**
     * @brief Виджет диалога
     */
    QFrame* m_centralWidget;

    /**
     * @brief Прогрессбар для диалога
     */
    QProgressBar* m_progress;

    /**
     * @brief Нужно ли расстягивать виджет с содержимым диалога
     */
    bool m_isContentStretchable;

    /**
     * @brief Актуальный код возврата
     */
    int m_execResult;
};

#endif // QLIGHTBOXDIALOG_H

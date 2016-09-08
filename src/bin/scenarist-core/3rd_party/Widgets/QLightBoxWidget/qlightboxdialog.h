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
	 * @brief Выполнить диалог
	 */
	int exec();

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
	virtual void initView();

	/**
	 * @brief Настроить соединения
	 */
	virtual void initConnections();

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
	 * @brief Обновить заголовок
	 */
	void updateTitle();

	/**
	 * @brief Анимировать отображение/сокрытие диалога
	 */
	/** @{ */
	void animateShow();
	void animateHide();
	void animate(bool _forward);
	/** @} */

private:
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

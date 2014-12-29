#ifndef PAGESTEXTEDIT_H
#define PAGESTEXTEDIT_H

#include <QTextEdit>

#include "PageMetrics.h"


/**
 * @brief Редактор текста, с возможностью постраничного отображения содержимого
 */
class PagesTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit PagesTextEdit(QWidget* parent = 0);

	/**
	 * @brief Установить формат страницы
	 */
	void setPageFormat(QPageSize::PageSizeId _pageFormat);

	/**
	 * @brief Настроить поля страницы
	 */
	void setPageMargins(const QMarginsF& _margins);

	/**
	 * @brief Получить режим отображения текста
	 */
	bool usePageMode() const;

public slots:
	/**
	 * @brief Установить режим отображения текста
	 */
	void setUsePageMode(bool _use);

	/**
	 * @brief Установить значение необходимости добавления дополнительной прокрутки снизу
	 */
	void setAddSpaceToBottom(bool _addSpace);

protected:
	/**
	 * @brief Переопределяется для корректировки документа и прорисовки оформления страниц
	 */
	void paintEvent(QPaintEvent* _event);

	/**
	 * @brief Переопределяется, для корректного обновления размеров вьювпорта
	 */
	void resizeEvent(QResizeEvent* _event);

private:
	/**
	 * @brief Обновить внутреннюю геометрию
	 */
	void updateInnerGeometry();

	/**
	 * @brief Обновить интервал вертикальной прокрутки
	 */
	void updateVerticalScrollRange();

	/**
	 * @brief Нарисовать оформление страниц документа
	 */
	void paintPagesView();

private slots:
	/**
	 * @brief Изменился интервал вертикальной прокрутки
	 */
	void aboutVerticalScrollRangeChanged(int _minimum, int _maximum);

private:
	/**
	 * @brief Режим отображения текста
	 *
	 * true - постраничный
	 * false - сплошной
	 */
	bool m_usePageMode;

	/**
	 * @brief Необходимо ли добавлять пространство снизу в обычном режиме
	 */
	bool m_addBottomSpace;

	/**
	 * @brief Метрика страницы редактора
	 */
	PageMetrics m_pageMetrics;

	/**
	 * @brief Указатель на документ
	 */
	QTextDocument* m_document;
};

#endif // PAGESTEXTEDIT_H

#ifndef REPORTBUTTON_H
#define REPORTBUTTON_H

#include <BusinessLayer/Statistics/AbstractReport.h>

#include <QRadioButton>


namespace UserInterface
{
	/**
	 * @brief Кнопка с шевроном
	 */
	class ReportButton : public QRadioButton
	{
		Q_OBJECT

	public:
		explicit ReportButton(const QString& _text,
			BusinessLogic::ReportParameters::Type _type = BusinessLogic::ReportParameters::Report,
			int _subtype = 0, QWidget* _parent = 0);

		/**
		 * @brief Получить тип кнопки
		 */
		BusinessLogic::ReportParameters::Type type() const;

		/**
		 * @brief Получить подтип кнопки
		 */
		int subtype() const;

	protected:
		/**
		 * @brief Переопределяется для прорисовки шеврона
		 */
		void paintEvent(QPaintEvent* _event);

	private:
		/**
		 * @brief Настроить виджет
		 */
		void init();

	private:
		/**
		 * @brief Тип кнопки
		 */
		BusinessLogic::ReportParameters::Type m_type;

		/**
		 * @brief Подтип отчёта или графика
		 */
		int m_subtype;
	};
}

#endif // REPORTBUTTON_H

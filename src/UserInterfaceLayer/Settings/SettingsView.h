#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QWidget>

class QSplitter;

namespace Ui {
	class SettingsView;
}

namespace UserInterface
{
	class SettingsView : public QWidget
	{
		Q_OBJECT

	public:
		explicit SettingsView(QWidget *parent = 0);
		~SettingsView();

		/**
		 * @brief Разделитель представления
		 */
		QSplitter* splitter() const;

		/**
		 * @brief Текущий тип рассчёта хронометража
		 */
		int chronometryCurrentType() const;

		/**
		 * @brief Установка параметров
		 */
		/** @{ */
		void setTextSpellCheck(bool _value);

		void setNavigatorShowScenesNumbers(bool  _value);

		void setChronometryCurrentType(int _value);
		void setChronometryPagesSeconds(int  _value);
		void setChronometryCharactersCharacters(int  _value);
		void setChronometryCharactersSeconds(int  _value);
		void setChronometryConfigurableSecondsForParagraphTimeAndPlace(double  _value);
		void setChronometryConfigurableSecondsFor50TimeAndPlace(double  _value);
		void setChronometryConfigurableSecondsForParagraphAction(double  _value);
		void setChronometryConfigurableSecondsFor50Action(double  _value);
		void setChronometryConfigurableSecondsForParagraphDialog(double  _value);
		void setChronometryConfigurableSecondsFor50Dialog(double  _value);
		/** @} */

	signals:
		/**
		 * @brief Изменения параметров
		 */
		/** @{ */
		void textSpellCheckChanged(bool);

		void navigatorShowScenesNumbersChanged(bool);
		void navigatorShowScenesTextChanged(bool);

		void chronometryCurrentTypeChanged();
		void chronometryPagesSecondsChanged(int);
		void chronometryCharactersCharactersChanged(int);
		void chronometryCharactersSecondsChanged(int);
		void chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double);
		void chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double);
		void chronometryConfigurableSecondsForParagraphActionChanged(double);
		void chronometryConfigurableSecondsFor50ActionChanged(double);
		void chronometryConfigurableSecondsForParagraphDialogChanged(double);
		void chronometryConfigurableSecondsFor50DialogChanged(double);
		/** @} */

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Форма
		 */
		Ui::SettingsView *ui;
	};
}

#endif // SETTINGSVIEW_H

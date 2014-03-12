#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>

namespace UserInterface {
	class SettingsView;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий настройками программы
	 */
	class SettingsManager : public QObject
	{
		Q_OBJECT

	public:
		explicit SettingsManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить состояние окна
		 */
		void loadViewState();

		/**
		 * @brief Сохранить состояние окна
		 */
		void saveViewState();

	signals:
		/**
		 * @brief Обновления настроек
		 */
		/** @{ */
		void textEditSettingsUpdated();
		void navigatorSettingsUpdated();
		void chronometrySettingsUpdated();
		/** @} */

	private slots:
		/**
		 * @brief Изменения параметров
		 */
		/** @{ */
		void textSpellCheckChanged(bool  _value);
		void navigatorShowScenesNumbersChanged(bool  _value);
		void chronometryCurrentTypeChanged();
		void chronometryPagesSecondsChanged(int  _value);
		void chronometryCharactersCharactersChanged(int  _value);
		void chronometryCharactersSecondsChanged(int  _value);
		void chronometryConfigurableSecondsForParagraphTimeAndPlaceChanged(double  _value);
		void chronometryConfigurableSecondsFor50TimeAndPlaceChanged(double  _value);
		void chronometryConfigurableSecondsForParagraphActionChanged(double  _value);
		void chronometryConfigurableSecondsFor50ActionChanged(double  _value);
		void chronometryConfigurableSecondsForParagraphDialogChanged(double  _value);
		void chronometryConfigurableSecondsFor50DialogChanged(double  _value);
		/** @} */

	private:
		void storeValue(const QString& _key, bool _value);
		void storeValue(const QString& _key, int _value);
		void storeValue(const QString& _key, double _value);
		void storeValue(const QString& _key, const QString& _value);

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
		UserInterface::SettingsView* m_view;
	};
}

#endif // SETTINGSMANAGER_H

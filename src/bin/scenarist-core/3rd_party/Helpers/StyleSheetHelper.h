#ifndef STYLESHEETHELPER_H
#define STYLESHEETHELPER_H

#include <QApplication>
#include <QString>
#include <QRegularExpression>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#else
#include <QScreen>
#endif


/**
 * @brief Вспомогательный класс для работы со стилями
 */
class StyleSheetHelper
{
public:
	/**
	 * @brief Обновить стиль, изменив все метрики на девайсонезависимые пиксели
	 * @note Метрики к замене должны быть указаны как 24dp
	 */
	static QString computeDeviceInpedendentSize(const QString& _styleSheet) {
		//
		// Заменяем все единицы на корректные в пикселах для текущего устройства
		//
		const QString oneLineStyleSheet = _styleSheet.simplified();
		QRegularExpression re("(\\d{1,})dp");
		QRegularExpressionMatch match = re.match(oneLineStyleSheet);
		int matchOffset = 0;
		QString resultStyleSheet = _styleSheet;
		while (match.hasMatch()) {
			const QString sourceValue = match.captured();
			const int targetValueInDp = match.captured(1).toInt();
			const QString targetValue = QString("%1px").arg(dpToPx(targetValueInDp));
			resultStyleSheet.replace(sourceValue, targetValue);

			matchOffset = match.capturedEnd();
			match = re.match(oneLineStyleSheet, matchOffset);
		}
		return resultStyleSheet;
	}

	/**
	 * @brief Преобразовать значение в девайсонезависимые пиксели
	 */
	static qreal dpToPx(int _pixelValue) {
		return _pixelValue * dp();
	}

private:
	/**
	 * @brief Кэшированное значение коэффициента девайсонезависимых пикселей
	 */
	static qreal dp() {
		static qreal s_dp = [](){
			//
			// Рассчитаем dp
			//
#ifdef Q_OS_ANDROID
			//  BUG with dpi on some androids: https://bugreports.qt-project.org/browse/QTBUG-35701
			//  Workaround:
			QAndroidJniObject qtActivity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
			QAndroidJniObject resources = qtActivity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");
			QAndroidJniObject displayMetrics = resources.callObjectMethod("getDisplayMetrics", "()Landroid/util/DisplayMetrics;");
			int density = displayMetrics.getField<int>("densityDpi");
#else
			QScreen *screen = QApplication::primaryScreen();
			float density = screen->physicalDotsPerInch();
#endif
			qreal dp =
				density < 180 ? 1
				: density < 270 ? 1.5
				: density < 360 ? 2 : 3;
			return dp;
		}();

		return s_dp;
	}
};

#endif // STYLESHEETHELPER_H

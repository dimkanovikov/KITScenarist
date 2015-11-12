#include "ScenarioTextEditHelpers.h"

#include <QObject>


namespace {
	static QString replaceKnownFooterWords(const QString& _text) {
		QString sourceText = _text.toUpper();
		return  sourceText
				.replace(QString::fromUtf8("ПАПКА"), QString::fromUtf8("ПАПКИ"))
				.replace(QString::fromUtf8("ГРУППА"), QString::fromUtf8("ГРУППЫ"))
				.replace(QString::fromUtf8("ВОСПОМИНАНИЕ"), QString::fromUtf8("ВОСПОМИНАНИЯ"))
				.replace(QString::fromUtf8("ВСТАВКА"), QString::fromUtf8("ВСТАВКИ"));
	}
}

QString Helpers::footerText(const QString& _headerText)
{
	QString footerText;

	footerText += QObject::tr("END OF", "ScenarioTextEditHelpers");
	footerText += " ";
	footerText += replaceKnownFooterWords(_headerText);

	return footerText;
}

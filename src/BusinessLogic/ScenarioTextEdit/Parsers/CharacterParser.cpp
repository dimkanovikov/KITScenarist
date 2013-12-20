#include "CharacterParser.h"

#include <QString>
#include <QRegularExpression>


QString CharacterParser::name(const QString& _text)
{
	//
	// В блоке персонажа так же могут быть указания, что он говорит за кадром и т.п.
	// эти указания даются в скобках
	//

	QString name = _text;
	return name.remove(QRegularExpression("[(](.*)[)]")).simplified();
}

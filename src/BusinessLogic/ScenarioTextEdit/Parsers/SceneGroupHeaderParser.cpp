#include "SceneGroupHeaderParser.h"

#include <QString>
#include <QStringList>


SceneGroupHeaderSection SceneGroupHeaderParser::section(const QString& _text)
{
	SceneGroupHeaderSection section = SceneGroupHeaderSectionUndefined;

	if (_text.split(":").count() > 1) {
		section = SceneGroupHeaderSectionName;
	} else {
		section = SceneGroupHeaderSectionDescription;
	}

	return section;
}

/*
 *  This file is part of Quartica.
 *
 *  Copyright (c) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
 *
 *  Quartica is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Quartica is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Quartica.  If not, see <http://www.gnu.org/licenses/>.
 */

// Qt4 Headers
#include <QDomDocument>
#include <QFileInfo>
#include <QRegExp>

// Quartica File Info
#include "freedesktopmime.h"

// =============================================================================
//  FreeDesktopMime: INTERNAL Methods
// =============================================================================
static int hex2int (int c) {
	if (!isascii((unsigned char) c))
		return -1;
	if (isdigit((unsigned char) c))
		return c - '0';
	if ((c >= 'a') && (c <= 'f'))
		return c + 10 - 'a';
	if ((c >= 'A') && (c <= 'F'))
		return c + 10 - 'A';
	return -1;
}

static void extract_offset (const QString& offset, int *startOffset, int *endOffset) {
	int index = offset.indexOf(':');

	if (index < 0) {
		*startOffset = offset.toInt();
		*endOffset = *startOffset;
	} else {
		*startOffset = offset.mid(0, index).toInt();
		*endOffset = *startOffset + offset.mid(index + 1).toInt();
	}
}

static QByteArray convert_string (const char *source) {
	QByteArray value;
	char c, val;

	while ((c = *source++) != '\0') {
		if (c == '\\') {
			switch (c = *source++) {
				case '\0': return(value);

				case 'n':
					value += '\n';
					break;
				case 'r':
					value += '\r';
					break;
				case 'b':
					value += '\b';
					break;
				case 't':
					value += '\t';
					break;
				case 'f':
					value += '\f';
					break;
				case 'v':
					value += '\v';
					break;

				/* \ and up to 3 octal digits */
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					val = c - '0';
					c = *source++;  /* try for 2 */
					if (c >= '0' && c <= '7') {
						val = (val << 3) | (c - '0');
						c = *source++;  /* try for 3 */
						if (c >= '0' && c <= '7')
							val = (val << 3) | (c - '0');
						else
							--source;
					}
					else
						--source;

					value += val;
					break;

					/* \x and up to 2 hex digits */
					case 'x':
						val = 'x';	/* Default if no digits */
						c = hex2int(*source++);	/* Get next char */
						if (c >= 0) {
							val = c;
							c = hex2int(*source++);
							if (c >= 0)
								val = (val << 4) + c;
							else
								--source;
						} else
							--source;
						value += val;
						break;

				default:
					value += c;
					break;
				}
			} else {
				value += c;
			}
		}

	return(value);
}


unsigned long convert_number (const QByteArray& text) {
	if (text.startsWith("0x")) {
		bool ok;
		unsigned long num = text.toULong(&ok, 16);
		return(ok ? num : -1);
	}

	if (text.startsWith("0"))
		return(text.toULong(NULL, 8));

	return(text.toULong());
}

/* Parse the value and mask attributes of a <match> element with a
 * numerical type (anything except "string").
 */
static void parse_int_value(int bytes, const char *in, const char *in_mask,
				QByteArray *parsed_value, QByteArray *parsed_mask,
				bool big_endian)
{
	unsigned long value = convert_number(in);

	for (int b = 0; b < bytes; b++) {
		int shift = (big_endian ? (bytes - b - 1) : b) * 8;
		parsed_value->append((value >> shift) & 0xff);
	}

	if ((bytes == 1 && (value & ~0xff)) ||
		(bytes == 2 && (value & ~0xffff)))
	{
		qWarning("Number out-of-range");
		return;
	}

	if (in_mask) {
		unsigned long mask = convert_number(in_mask);

		parsed_mask->fill(0, bytes);
		for (int b = 0; b < bytes; b++) {
			int shift = (big_endian ? (bytes - b - 1) : b) * 8;
			parsed_mask->data()[b] = (mask >> shift) & 0xff;
		}
	}

	if (parsed_mask->length() > 0 && parsed_mask->at(0) == 0)
		parsed_mask->clear();
}

/* 'len' is the length of the value. The mask created will be the same
 * length.
 */
static QByteArray parse_string_mask(const char *mask, int len)
{
	if (mask == NULL || len < 1) return(QByteArray());

	if (mask[0] != '0' || mask[1] != 'x')
	{
		qWarning("parse_string_mask(): String masks must be in base 16 (starting with 0x)");
		return QByteArray();
	}
	mask += 2;

	QByteArray parsed_mask(len, 0);

	for (int i = 0; mask[i]; ++i)
	{
		int c = hex2int(mask[i]);
		if (c == -1)
		{
			qWarning("parse_string_mask(): is not a valid hex digit");
			return QByteArray();
		}

		if (i >= len * 2)
		{
			qWarning("parse_string_mask(): Mask is longer than value");
			return QByteArray();
		}

		parsed_mask[i >> 1] = parsed_mask[i >> 1] | ((i & 1) ? c : (c << 4));
	}

	if (parsed_mask.length() > 0 && (char) parsed_mask[0] == 0)
		parsed_mask.clear();

	return parsed_mask;
}

// =============================================================================
//  FreeDesktopMime: PRIVATE Class
// =============================================================================
class QFreeDesktopMime::Private {
	public:
		QDomDocument xmlDocument;
		QDomElement mimeNode;
};

// =============================================================================
//  FreeDesktopMime: PUBLIC Constructors/Destructors
// =============================================================================
QFreeDesktopMime::QFreeDesktopMime (QObject *parent)
	: QObject(parent), d(new QFreeDesktopMime::Private)
{
	// Load Xml Freedesktop
	QFile xml(":/QFreeDesktopMime/mimetypes.xml");
	if (xml.open(QIODevice::ReadOnly)) {
		d->xmlDocument.setContent(&xml);
		xml.close();
	}
}

QFreeDesktopMime::~QFreeDesktopMime() {
	delete d;
	d = NULL;
}

// =============================================================================
//  FreeDesktopMime: PUBLIC Methods (MIME From File)
// =============================================================================
QString QFreeDesktopMime::fromFileName (const QString& fileName) {
	QFileInfo fileInfo(fileName);

	QDomElement root = d->xmlDocument.documentElement();
	QDomNodeList globList = root.elementsByTagName("glob");
	for (int i = 0; i < globList.length(); ++i) {
		QDomElement globNode = globList.item(i).toElement();

		QRegExp regExp(globNode.attribute("pattern"));
		regExp.setPatternSyntax(QRegExp::Wildcard);
		if (regExp.exactMatch(fileInfo.fileName())) {
			d->mimeNode = globNode.parentNode().toElement();
			return(d->mimeNode.attribute("type"));
		}
	}

	return(QString());
}

QString QFreeDesktopMime::fromFile (const QString& fileName) {
	QFileInfo fileInfo(fileName);
	if (fileInfo.isDir()) {
		getMimeNode("inode/directory");
		return("inode/directory");
	}

	QFile file(fileName);

	if (!file.open(QIODevice::ReadOnly))
		return(QString());

	QString mimeType = fromFile(&file);

	file.close();

	return(mimeType);
}

QString QFreeDesktopMime::fromFile (QFile *file) {
	QDomElement root = d->xmlDocument.documentElement();
	QDomNodeList magicList = root.elementsByTagName("magic");

	QList<QDomElement> matchMagicList;

	for (int i = 0; i < magicList.length(); ++i) {
		QDomElement magicNode = magicList.item(i).toElement();
		if (checkMagic(magicNode, file)) matchMagicList.append(magicNode);
	}

	if (matchMagicList.size() > 0) {
		QDomElement magic = matchMagicList[0];
		for (int i = 1; i < matchMagicList.size(); ++i) {
			if (!matchMagicList[i].hasAttribute("priority")) {
				magic = matchMagicList[i];
				break;
			} else if (magic.hasAttribute("priority") && magic.attribute("priority") < matchMagicList[i].attribute("priority")) {
				magic = matchMagicList[i];
			}
		}

		d->mimeNode = magic.parentNode().toElement();
		return(d->mimeNode.attribute("type"));
	}

	return(fromFileName(file->fileName()));
}

// =============================================================================
//  FreeDesktopMime: PUBLIC Methods (Specify Mime Type)
// =============================================================================
QString QFreeDesktopMime::genericIconName (const QString& mimeType) {
	return(getMimeNode(mimeType) ? genericIconName() : QString());
}

QString QFreeDesktopMime::expandedAcronym (const QString& mimeType) {
	return(getMimeNode(mimeType) ? expandedAcronym() : QString());
}

QString QFreeDesktopMime::description (const QString& mimeType) {
	return(getMimeNode(mimeType) ? description() : QString());
}

QString QFreeDesktopMime::subClassOf (const QString& mimeType) {
	return(getMimeNode(mimeType) ? subClassOf() : QString());
}

QString QFreeDesktopMime::acronym (const QString& mimeType) {
	return(getMimeNode(mimeType) ? acronym() : QString());
}

QString QFreeDesktopMime::alias (const QString& mimeType) {
	return(getMimeNode(mimeType) ? alias() : QString());
}

// =============================================================================
//  FreeDesktopMime: PUBLIC Methods
// =============================================================================
QString QFreeDesktopMime::genericIconName (void) const {
	QDomNodeList nodeList = d->mimeNode.elementsByTagName("generic-icon");
	return((nodeList.length() > 0) ? nodeList.item(0).toElement().attribute("name") : QString());
}

QString QFreeDesktopMime::expandedAcronym (void) const {
	QDomNodeList nodeList = d->mimeNode.elementsByTagName("expanded-acronym");
	return((nodeList.length() > 0) ? nodeList.item(0).toElement().text() : QString());
}

QString QFreeDesktopMime::description (void) const {
	// NOTE: For the Future... Support Multi Language
	QDomNodeList nodeList = d->mimeNode.elementsByTagName("comment");
	return((nodeList.length() > 0) ? nodeList.item(0).toElement().text() : QString());
}

QString QFreeDesktopMime::subClassOf (void) const {
	QDomNodeList nodeList = d->mimeNode.elementsByTagName("sub-class-of");
	return((nodeList.length() > 0) ? nodeList.item(0).toElement().attribute("type") : QString());
}

QString QFreeDesktopMime::mimeType (void) const {
	return(d->mimeNode.isNull() ? QString() : d->mimeNode.attribute("type"));
}

QString QFreeDesktopMime::acronym (void) const {
	QDomNodeList nodeList = d->mimeNode.elementsByTagName("acronym");
	return((nodeList.length() > 0) ? nodeList.item(0).toElement().text() : QString());
}

QString QFreeDesktopMime::alias (void) const {
	QDomNodeList nodeList = d->mimeNode.elementsByTagName("alias");
	return((nodeList.length() > 0) ? nodeList.item(0).toElement().attribute("type") : QString());
}

// =============================================================================
//  FreeDesktopMime: PROTECTED Methods
// =============================================================================
bool QFreeDesktopMime::getMimeNode (const QString& mimeType) {
	if (!d->mimeNode.isNull() && d->mimeNode.attribute("type") == mimeType)
		return(true);

	QDomElement root = d->xmlDocument.documentElement();
	QDomNodeList mimeList = root.elementsByTagName("mime-type");
	for (int i = 0; i < mimeList.length(); ++i) {
		QDomElement xmlNode = mimeList.item(i).toElement();
		if (xmlNode.attribute("type") == mimeType) {
			d->mimeNode = xmlNode;
			return(true);
		}
	}

	return(false);
}

bool QFreeDesktopMime::checkMagic (const QDomElement& magicNode, QFile *file) {
	QDomNodeList matchList = magicNode.elementsByTagName("match");
	return(checkMagic(matchList, file));
}

bool QFreeDesktopMime::checkMagic (const QDomNodeList& matchList, QFile *file) {
	for (int i = 0; i < matchList.length(); ++i) {
		QDomElement matchNode = matchList.item(i).toElement();
		if (matchNode.parentNode().nodeName() != "magic")
			continue;

		if (checkMatch(matchNode, file)) {
			QDomNodeList subMatchList = matchNode.elementsByTagName("match");
			return((subMatchList.length() > 0) ? checkSubMagic(subMatchList, file) : true);
		}
	}

	return(false);
}

bool QFreeDesktopMime::checkSubMagic (const QDomNodeList& matchList, QFile *file) {
	for (int i = 0; i < matchList.length(); ++i) {
		QDomElement matchNode = matchList.item(i).toElement();
		if (matchNode.parentNode().nodeName() != "match")
			continue;

		if (checkMatch(matchNode, file)) {
			QDomNodeList subMatchList = matchNode.elementsByTagName("match");
			return((subMatchList.length() > 0) ? checkSubMagic(subMatchList, file) : true);
		}
	}

	return(false);
}

bool QFreeDesktopMime::checkMatch (const QDomElement& matchNode, QFile *file) {
	QString value = matchNode.attribute("value");
	QString mask = matchNode.attribute("mask");
	QString type = matchNode.attribute("type");
	QByteArray parsedValue, parsedMask;

	if (type == "string") {
		parsedValue = convert_string(value.toUtf8());
		if (!mask.isEmpty()) parsedMask = parse_string_mask(mask.toUtf8(), parsedValue.length());
	} else if (type.contains("16")) {
		parse_int_value(2, value.toUtf8(), mask.toUtf8(), &parsedValue, &parsedMask, type[0] != 'l');
	} else if (type.contains("32")) {
		parse_int_value(4, value.toUtf8(), mask.toUtf8(), &parsedValue, &parsedMask, type[0] != 'l');
	} else if (type == "byte") {
		parse_int_value(1, value.toUtf8(), mask.toUtf8(), &parsedValue, &parsedMask, false);
	} else {
		qWarning("QFreeDesktopMime::checkMagic(): Type Not Found. You shoudn't be here!");
		return(false);
	}

	return(checkMagicString(matchNode.attribute("offset"), parsedValue, parsedMask, file));
}

// =============================================================================
//  FreeDesktopMime: PRIVATE Methods
// =============================================================================
bool QFreeDesktopMime::checkMagicString (const QString offset, const QByteArray value, const QByteArray mask, QFile *file) {
	int startOffset, endOffset;
	extract_offset(offset, &startOffset, &endOffset);

	int valueLength = value.length();
	if (valueLength < 1) return(false);

	if (!file->seek(startOffset)) return(false);
	QByteArray fullData = file->read(endOffset + valueLength + 1);
	if (fullData.length() < valueLength) return(false);

	for (int i = 0; (startOffset + i) <= endOffset; ++i) {
		QByteArray data = fullData.mid(i, valueLength);
		if (data.length() < valueLength) break;

		bool match = true;
		if (mask.isEmpty()) {
			match = (data == value);
		} else {
			for (int i = 0; i < valueLength; ++i) {
				if ((value[i] & mask[i]) != (data[i] & mask[i])) {
					match = false;
					break;
				}
			}
		}

		if (match) return(true);
	}

	return(false);
}


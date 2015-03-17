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

#ifndef _QFREEDESKTOPMIME_H_
#define _QFREEDESKTOPMIME_H_

#include <QDomElement>
#include <QFile>

class QFreeDesktopMime : public QObject {
	Q_OBJECT

	public:
		QFreeDesktopMime (QObject *parent = 0);
		~QFreeDesktopMime();

		QString fromFileName (const QString& fileName);

		QString fromFile (const QString& fileName);
		QString fromFile (QFile *file);

		// Methods Information
		QString genericIconName (const QString& mimeType);
		QString expandedAcronym (const QString& mimeType);		
		QString description (const QString& mimeType);
		QString subClassOf (const QString& mimeType);
		QString acronym (const QString& mimeType);
		QString alias (const QString& mimeType);

		QString genericIconName (void) const;
		QString expandedAcronym (void) const;
		QString description (void) const;
		QString subClassOf (void) const;
		QString mimeType (void) const;
		QString acronym (void) const;
		QString alias (void) const;		

	protected:
		bool checkMatch (const QDomElement& matchNode, QFile *file);
		bool checkMagic (const QDomElement& magicNode, QFile *file);
		bool checkMagic (const QDomNodeList& matchList, QFile *file);
		bool checkSubMagic (const QDomNodeList& matchList, QFile *file);

		bool getMimeNode (const QString& mimeType);

	private:
		bool checkMagicString (const QString offset, const QByteArray value, const QByteArray mask, QFile *file);

	private:
		class Private;
		Private *d;
};

#endif // !_QFREEDESKTOPMIME_H_


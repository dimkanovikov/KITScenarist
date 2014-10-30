/*
	Copyright (C)  2010  Brad Hards <bradh@frogmouth.net>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PictDestination.h"

#include "rtfreader.h"

namespace RtfReader
{
	PictDestination::PictDestination( Reader *reader, AbstractRtfOutput *output, const QString &name ) :
	  Destination( reader, output, name )
	{
	}

	PictDestination::~PictDestination()
	{
	}

	void PictDestination::handleControlWord( const QString &controlWord, bool hasValue, const int value )
	{
	if (! hasValue) {
		qDebug() << "expected control word to have valid value, but it does not";
	}

	if ( controlWord == "jpegblip" ) {
		// handle this later
	} else if ( controlWord == "wmetafile" ) {
		qDebug() << "todo: get WMF data";
	} else if ( controlWord == "picw" ) {
		qDebug() << "pict width: " << value;
		m_imageFormat.setWidth( value );
	} else if ( controlWord == "pich" ) {
		qDebug() << "pict height: " << value;
		m_imageFormat.setHeight( value );
	} else if ( controlWord == "picscalex" ) {
		qDebug() << "X scale: " << value;
	} else if ( controlWord == "picscaley" ) {
		qDebug() << "Y scale: " << value;
	} else if ( controlWord == "piccropl" ) {
		qDebug() << "Left crop:" << value;
	} else if ( controlWord == "piccropr" ) {
		qDebug() << "Right crop:" << value;
	} else if ( controlWord == "piccropt" ) {
		qDebug() << "Top crop:" << value;
	} else if ( controlWord == "piccropb" ) {
		qDebug() << "Bottom crop:" << value;
	} else if ( controlWord == "pichgoal" ) {
		qDebug() << "Goal Height:" << value;
	} else if ( controlWord == "picwgoal" ) {
		qDebug() << "Goal Width:" << value;
	} else {
		qDebug() << "unexpected control word in pict:" << controlWord;
	}
	}

	void PictDestination::handlePlainText( const QString &plainText )
	{
		// FIXME: dimkanovikov
	m_pictHexData += plainText.toUtf8();//.toAscii();
	}

	void PictDestination::aboutToEndDestination()
	{
	QImage image = QImage::fromData( QByteArray::fromHex( m_pictHexData ) );
	m_output->createImage(image, m_imageFormat);
	}
}

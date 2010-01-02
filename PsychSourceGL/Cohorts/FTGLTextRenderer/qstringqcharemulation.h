/*
 * qstringqcharemulation: A very minimalistic bare-bones reimplementation
 * of the QRgb, QChar and QString classes of Nokia's (former Trolltech's)
 * QT-4 toolkit.
 *
 * This only reimplements an API compatible, ABI incompatible small
 * subset of QString/QChars functionality -- just the tiny amount needed
 * to interface libptbdrawtext_ftgl.cpp with OGLFT.cpp for passing of
 * Unicode text strings and for OGLFT.cpp's internal unicode processing
 * needs.
 *
 * As OGLFT.cpp is almost unmodified, you can switch at compile/link/build
 * time between using this minimalistic implementation, or QT-4's full
 * implementation by passing different compiler options as explained in the
 * build instructions at the top of libptbdrawtext_ftgl.cpp source code.
 *
 *
 * qstringqcharemulation is copyright (c) 2010 by Mario Kleiner.
 * It is licensed to you under the LGPL license as follows:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdlib.h>

#define qRed(x) (x).rgbcolor[0]
#define qGreen(x) (x).rgbcolor[1]
#define qBlue(x) (x).rgbcolor[2]
#define qAlpha(x) (x).rgbcolor[3]

class QRgb
{
	public:
		unsigned int rgbcolor[4];
};

class QChar
{
	public:
		QChar();
		QChar(unsigned int value);
		unsigned int unicode() const;
		
	private:
		unsigned int unicodeval;
};

class QString
{
	public:
		QString();
		QString(const QString& s);
		QString(const QChar *unicode, int size);
		QString& operator = (const QString& s);
		~QString();
		int length() const;
		const QChar at(int i) const;
		
	private:
		unsigned int*	data;
		int				dlength;
};

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

#include "qstringqcharemulation.h"

QChar::QChar()
{
	// Nothing to do, except init to unicode zero:
	unicodeval = 0;
	return;
}

QChar::QChar(unsigned int value)
{
	// Assign given unicode value to internal representation:
	unicodeval = value;
	return;
}

unsigned int QChar::unicode(void) const
{
	return(unicodeval);
}

QString::QString()
{
	data = NULL;
	dlength = 0;
	return;
}

QString::QString(const QString& s)
{
	dlength = s.dlength;
	if (dlength > 0) {
		data = new unsigned int[dlength];
		for (int i = 0; i < dlength; i++) data[i] = s.data[i];
	}
	else {
		data = NULL;
	}

	return;	
}

QString& QString::operator = (const QString& s)
{
	if (&s != this) {
		if (data) delete[] data;
		data = NULL;
		dlength = 0;
		
		dlength = s.dlength;
		if (dlength > 0) {
			data = new unsigned int[dlength];
			for (int i = 0; i < dlength; i++) data[i] = s.data[i];
		}
		else {
			data = NULL;
		}
	}

	return(*this);
}

QString::QString(const QChar *unicode, int size)
{
	dlength = size;
	data = new unsigned int[dlength];	
	for (int i = 0; i < dlength; i++) data[i] = ((QChar*) unicode)[i].unicode();
	return;
}

QString::~QString()
{
	if (data) delete[] data;
	data = NULL;
	dlength = 0;
	
	return;
}

int QString::length(void) const
{
	return(dlength);
}

const QChar QString::at(int i) const
{
	return( QChar(data[i]) );
}

/*
* Copyright (C) 2016 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#define _CRT_SECURE_NO_WARNINGS

#include "trace.h"
#include <crtdbg.h>
#include <stdio.h>
#include <stdarg.h>

void TRACE( const char* format, ... )
{
	char szBuf[1024];

	va_list args;
	va_start( args, format );
	_vsnprintf( szBuf, sizeof( szBuf ), format, args );
	va_end( args );

	_RPT0( _CRT_WARN, szBuf );
}

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

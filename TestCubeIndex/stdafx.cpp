// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// TestCubeIndex.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
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

///////////////////////////////////
// 
// debugproc.cpp
// 
// Author: HekSeon
///////////////////////////////////
#include <stdio.h>
#include "debugproc.h"
#include "renderer.h"

///////////////////////////////////
// makro area
///////////////////////////////////

///////////////////////////////////
// protota area
///////////////////////////////////

///////////////////////////////////
// global area
///////////////////////////////////
char		g_aStrDebug[1024] = { "\0" };


///////////////////////////////////
// debug area
///////////////////////////////////
void DrawDebugProc(void)
{
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	DebugTextOut(g_aStrDebug, 0, 0);

	// èÓïÒÉNÉäÉA
	memset(g_aStrDebug, 0, sizeof g_aStrDebug);
}

///////////////////////////////////
// debug area
///////////////////////////////////
void PrintDebugProc(char* fmt, ...)
{
#if 0
	long* pParam;
	static char aBuf[256];

	pParam = (long*)&fmt;
	sprintf(aBuf, fmt, pParam[1], pParam[2], pParam[3], pParam[4],
		pParam[5], pParam[6], pParam[7], pParam[8],
		pParam[9], pParam[10], pParam[11], pParam[12]);
#else
	va_list list;		
	char* pCur;
	char aBuf[256] = { "\0" };
	char aWk[32];

	va_start(list, fmt);

	pCur = fmt;
	for (; *pCur; ++pCur)
	{
		if (*pCur != '%')
		{
			sprintf_s(aWk, "%c", *pCur);
		}
		else
		{
			pCur++;

			switch (*pCur)
			{
			case 'd':
				sprintf_s(aWk, "%d", va_arg(list, int));
				break;

			case 'f':
				sprintf_s(aWk, "%.2f", va_arg(list, double));
				break;

			case 's':
				sprintf_s(aWk, "%s", va_arg(list, char*));
				break;

			case 'c':
				sprintf_s(aWk, "%c", va_arg(list, char));
				break;

			default:
				sprintf_s(aWk, "%c", *pCur);
				break;
			}
		}
		strcat_s(aBuf, aWk);
	}

	va_end(list);

	if ((strlen(g_aStrDebug) + strlen(aBuf)) < ((sizeof g_aStrDebug) - 1))
	{
		strcat_s(g_aStrDebug, aBuf);
	}
#endif
}


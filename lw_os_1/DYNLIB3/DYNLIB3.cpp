#include <Windows.h>
#include <wchar.h>
#include <stdio.h>
#include "DYNLIB3.h"

#ifndef MOUSEWHEEL_ROUTING_FOCUS 
#define MOUSEWHEEL_ROUTING_FOCUS		0
#endif // !MOUSEWHEEL_ROUTING_FOCUS 

#ifndef MOUSEWHEEL_ROUTING_HYBRID 
#define MOUSEWHEEL_ROUTING_HYBRID		1
#endif // !MOUSEWHEEL_ROUTING_HYBRID 

#ifndef MOUSEWHEEL_ROUTING_MOUSE_POS
#define MOUSEWHEEL_ROUTING_MOUSE_POS	2
#endif // MOUSEWHEEL_ROUTING_MOUSE_POS 

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD dwReason, LPVOID LpvResearved)
{
	return TRUE;
}
DYNLIB3_API BOOL PrintSYSparamInfo(LPCWSTR lpDispalyNam, UINT uiAction)
{
	BOOL bReturn = FALSE;
	switch (uiAction)
	{
	case SPI_GETMOUSEWHEELROUTING:
		{
			DWORD value;
			bReturn = SystemParametersInfo(uiAction, 0, &value, 0);
			if (FALSE != bReturn)
			{
				switch (value)
				{
				case MOUSEWHEEL_ROUTING_FOCUS:
					/*èíôîðìàöèÿ ââîäèìàÿ êîëåñîì ìûøì îòïðàâëÿåòñÿ â ïðèëîæåíèå ñ ôîêóñîì*/
					wprintf(L"%s:MOUSEWHEEL_ROUTING_FOCUS\n", lpDispalyNam);
					break;
				case MOUSEWHEEL_ROUTING_HYBRID:
					/* èíôîðìàöèÿ ââåäåííàÿ êîë¸ñèêîì ìûøè îòïðàâëÿåòñÿâ ïðèëîæåíèå
					ðàáî÷åãî ñòîëà ñ ôîêóñîì èëè ïðèëîæåíèÿ Window Store ïîä êóðñîðîì ìûøè*/
					wprintf(L"%s:MOUSEWHEEL_ROUTING_HYBRID \n", lpDispalyNam);
					break;
				case MOUSEWHEEL_ROUTING_MOUSE_POS:
					wprintf(L"%s:MOUSEWHEEL_ROUTING_MOUSE_POS \n", lpDispalyNam);
					break;
				default:
					wprintf(L"%s:%u \n", lpDispalyNam,value);
					break;
				}
			}
		}
		break;
	case SPI_GETWORKAREA:
	{
		RECT rcWorkArea;
		bReturn = SystemParametersInfo(uiAction, 0, &rcWorkArea, 0);
		if (FALSE != bReturn) 
		{
			wprintf(L"%s:%d x %d \n", lpDispalyNam, (rcWorkArea.right - rcWorkArea.left), (rcWorkArea.bottom - rcWorkArea.top));
		}
	}
	break;
	}
	return bReturn;
}

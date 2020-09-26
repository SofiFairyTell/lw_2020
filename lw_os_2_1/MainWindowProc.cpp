////
////#include "lw_os_2_1.h"
////LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
////{
////	HDC hdc;             // индекс контекста устройства
////	PAINTSTRUCT ps;      // структура для рисования
////
////	switch (msg)
////	{
////	case WM_PAINT:
////	{
////		hdc = BeginPaint(hwnd, &ps);
////		TextOut(hdc, 10, 20, L"WM_PAINT", 8);
////		EndPaint(hwnd, &ps);
////		return 0;
////	}
////
////	case WM_LBUTTONDOWN:
////	{
////		hdc = GetDC(hwnd);
////		TextOut(hdc, 10, 40, L"WM_LBUTTONDOWN", 14);
////		ReleaseDC(hwnd, hdc);
////		return 0;
////	}
////
////	case WM_DESTROY:
////	{
////		PostQuitMessage(0);
////		return 0;
////	}
////	}
////	return DefWindowProc(hwnd, msg, wParam, lParam);
////}
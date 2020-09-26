//// =====================================
//// Функция Register
//// Выполняет регистрацию классов окна,
//// используемых приложением
//// =====================================
//#include "lw_os_2_1.h"
//
//int Register(HINSTANCE hInstance)
//{
//	ATOM aWndClass; // атом для кода возврата
//   
//	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) }; // структура для регистрации класса окна
//	memset(&wcex, 0, sizeof(wcex));
//	// Регистрируем класс окна для главного	окна
//
//	wcex.style = CS_HREDRAW | CS_VREDRAW; //для перерисовки окна при изменении ширины/высоты. Через сообщение WM_Paint
//	wcex.lpfnWndProc = MainWindowProc; //  указатель на процедуру окна вызываемую функцией CallWindowProc.
//	wcex.hInstance = hInstance; //дескриптор содержащий оконную процедуру класса
//	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);//имя в ресурсах 
//	wcex.lpszClassName = TEXT("MainWindowClass"); // имя класса 
//	//aWndClass = RegisterClass(&wcex);
//	//if (aWndClass == 0) return FALSE;
//	if (0 == RegisterClassEx(&wcex))
//		return -1;
//}
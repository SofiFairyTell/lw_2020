//
//#define STRICT   
///*
//Rich Edit example
//Coded by: weijixian
//*/
//#include <windows.h>   
//#include "rchedit.h" //this is my header   
//void CenterWindow(HWND);
//
//HWND hWnd, richedit;
//HINSTANCE relib;
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
//{
//	HDC             hdc;
//	PAINTSTRUCT     ps;
//	static HMENU    hmenu;
//	EDITSTREAM      editstream;
//	RECT            clientrect; //just for cosmetic purposes   
//	int             height = 0;
//	char            fname[MAX_PATH] = { "" };
//	fstream         fileio;
//	string          clipboard("");      //for clipboard functions   
//	HANDLE          globalmem = NULL;   //for clipboard functions   
//	LPSTR           stringpntr = NULL;  //for clipboard functions   
//	HANDLE          cliphandle;         //for paste   
//	string          cliptext("");       //for paste   
//	NMHDR           *notify;            //for drag/drop (see wm_notify)   
//	ENDROPFILES     *dropfiles;         //for drag/drop (see wm_notify)   
//	HANDLE          hdrop;
//	char            dropped[MAX_PATH] = { "" };
//
//	switch (iMsg)
//	{
//	case WM_CREATE:
//		hmenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(MENU));
//		SetMenu(hWnd, hmenu);
//		DrawMenuBar(hWnd);
//		//To make up for titlebar and menu's affects on the height   
//		//we can use the client's height only for richedit window   
//		GetClientRect(hWnd, &clientrect);
//		height = clientrect.bottom - clientrect.top;
//
//		relib = LoadLibrary("riched32.dll");    //load the dll don't forget this   
//												//and don't forget to free it (see wm_destroy)   
//		if (relib == NULL)
//			MessageBox(NULL, "Failed to load riched32.dll!", "Error", MB_ICONEXCLAMATION);
//		//might as well abort the program here but ill let you look at an empty window..   
//		//why wouldn't you have riched32.dll anyway!? :o\ ...   
//
//		richedit = CreateWindowEx(NULL, RICHEDIT_CLASS, "Open file or drag/drop into window",
//			WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_HSCROLL | WS_VSCROLL,
//			0, 0, 390, height, hWnd, NULL, GetModuleHandle(NULL), NULL);
//
//		//a richedit left as normal is stripped down   
//		//we have to enable things such as drag/drop capabilities   
//		SendMessage(richedit, EM_SETEVENTMASK, 0, (LPARAM)(DWORD)ENM_DROPFILES);
//		DragAcceptFiles(richedit, true); //allows the drag/drop   
//		break;
//	case WM_SIZE:
//		SetWindowPos(richedit, 0, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOMOVE | SWP_NOZORDER);
//		break;
//	case WM_COMMAND:
//		switch (LOWORD(wParam))
//		{
//		case FILE_OPEN:
//			//here we get a filename and open the file to stream   
//			//the data in, this is actually very easy   
//			//refer to the callback to follow what's happening   
//			if (GetFileName(fname, MAX_PATH, hWnd)) //get filename to open   
//			{
//				fileio.open(fname, ios::in); //open as you normally would   
//				if (fileio.fail())       //checks   
//				{
//					fileio.close();
//					MessageBox(hWnd, "Error opening file", "Error", MB_OK);
//					break;
//				}
//				SetStreamType(StreamType_File); //set stream type   
//				//setting the stream type is OUR way of knowing if it's   
//				//a buffer from a string or if it's a file object   
//				//this isn't window's code, this is our makeshift way   
//				editstream.pfnCallback = EditStreamCallback;    //tell it the callback function   
//				editstream.dwCookie = (unsigned long)&fileio;   //pass the file object through cookie   
//				SendMessage(richedit, EM_STREAMIN, SF_TEXT, (LPARAM)&editstream); //tell it to start stream in   
//				fileio.close(); //close file   
//			}
//			break;
//		case FILE_EXIT:
//			SendMessage(hWnd, WM_CLOSE, 0, 0);
//			break;
//		case OPTIONS_COPY:
//			if (OpenClipboard(hWnd) == false)
//			{
//				MessageBox(hWnd, "Another program is using the clipboard, please wait until that program finishes.", "Error", MB_OK | MB_ICONEXCLAMATION);
//				return false;
//			}
//			EmptyClipboard();
//			editstream.pfnCallback = EditStreamCallbackRead;
//			editstream.dwCookie = (unsigned long)&clipboard;
//			SendMessage(richedit, EM_STREAMOUT, SF_TEXT, (LPARAM)&editstream);
//			globalmem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, clipboard.size() + 1);
//			stringpntr = (LPSTR)GlobalLock(globalmem);
//			strcpy(stringpntr, clipboard.data());
//			GlobalUnlock(globalmem);
//			SetClipboardData(CF_TEXT, globalmem);
//			CloseClipboard();
//			break;
//		case OPTIONS_PASTE:
//			if (OpenClipboard(hWnd) == false)
//			{
//				MessageBox(hWnd, "Another program is using the clipboard, please wait until that program finishes.", "Error", MB_OK | MB_ICONEXCLAMATION);
//				return false;
//			}
//			cliphandle = GetClipboardData(CF_TEXT);
//			if (cliphandle == 0)//no text   
//			{
//				CloseClipboard();
//				return false;
//			}
//
//			stringpntr = (LPSTR)GlobalLock(cliphandle);
//			cliptext = stringpntr;
//			GlobalUnlock(cliphandle);
//			CloseClipboard();
//
//			SetStreamType(StreamType_Buffer);   //set stream as buffer   
//			editstream.pfnCallback = EditStreamCallback;    //set callback   
//			editstream.dwCookie = (unsigned long)&cliptext; //pass address of buffer   
//			SendMessage(richedit, EM_STREAMIN, SF_TEXT, (LPARAM)&editstream);  //stream   
//			break;
//		case OPTIONS_CLEAR:
//			SendMessage(richedit, WM_SETTEXT, 0, (LPARAM)(LPCSTR)"");  //easy, old way   
//			//this would be a good time to mention that the reason we use STREAM_IN/OUT   
//			//is because the normal method of WM_SETTEXT has limitations   
//			//a normal edit control has a limit on the amount of text it can hold   
//			//but the limit a rich edit has depends on your system memory   
//			break;
//		case ABOUT_ABOUT:
//			MessageBox(hWnd, "RichText control example, WIN32 API\nhttp://www.PlanetCpp.com", "About", MB_OK);
//			break;
//		};
//		break;
//	case WM_NOTIFY:
//		notify = (NMHDR*)lParam;
//		if (notify->hwndFrom == richedit)
//		{
//			switch (notify->code)
//			{
//			case EN_DROPFILES:
//				dropfiles = (ENDROPFILES FAR *)lParam;
//				hdrop = dropfiles->hDrop;
//				DragQueryFile((HDROP)hdrop, 0, dropped, MAX_PATH);//only first one will count   
//				DragFinish((HDROP)hdrop);
//				for (int l = 0; l < strlen(dropped); l++)
//					if (isupper(dropped[l]))
//						dropped[l] = tolower(dropped[l]);
//				if (strstr(dropped, ".txt") != NULL || strstr(dropped, ".dat") != NULL
//					|| strstr(dropped, ".log") != NULL)
//				{
//					//same as open   
//					fileio.open(dropped, ios::in);
//					if (fileio.fail()) {
//						fileio.close();
//						MessageBox(hWnd, "Error opening file", "Error", MB_OK);
//						break;
//					}
//					SetStreamType(StreamType_File);
//					editstream.pfnCallback = EditStreamCallback;    //tell it the callback function   
//					editstream.dwCookie = (unsigned long)&fileio;   //pass the file object through cookie   
//					SendMessage(richedit, EM_STREAMIN, SF_TEXT, (LPARAM)&editstream); //tell it to start stream in   
//					fileio.close(); //close file   
//				}
//				else
//					MessageBox(hWnd, "Only *.txt ,*.dat, and *.log files accepted.", "Error", MB_OK);
//			};
//		}
//		break;
//	case WM_PAINT:
//		hdc = BeginPaint(hWnd, &ps);
//		EndPaint(hWnd, &ps);
//		break;
//	case WM_CLOSE:
//		SendMessage(richedit, WM_CLOSE, 0, 0); //close richedit control   
//		if (relib != NULL)
//			FreeLibrary(relib);                 //tell DLL it's free to go   
//		break;
//	case WM_DESTROY:
//		PostQuitMessage(0);                 //i'm outta here   
//		break;
//	}
//	return DefWindowProc(hWnd, iMsg, wParam, lParam);
//}
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//	PSTR szCmdLine, int iCmdShow)
//{
//
//	const char *ClassName = "PCPP - RichEdit";
//	WNDCLASSEX wclass;          //here we set up a variable for our window class   
//	MSG msg;                    //this will hold the window messages later on   
//	//Next we set up the properties of the window class   
//	wclass.cbSize = sizeof(wclass);         //the size   
//	wclass.style = CS_HREDRAW | CS_VREDRAW; //style   
//	wclass.lpfnWndProc = WndProc;           //this tells windows the function to send messages to   
//	wclass.cbClsExtra = 0;
//	wclass.cbWndExtra = 0;
//	wclass.hInstance = hInstance;           //the instance of your program   
//	wclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);  //which icon to use   
//	wclass.hCursor = LoadCursor(NULL, IDC_ARROW);    //the cursor to use   
//	wclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);    //background color   
//	wclass.lpszMenuName = NULL;             //set the menu   
//	wclass.lpszClassName = ClassName;       //set classname   
//	wclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);    //set small icon   
//
//	RegisterClassEx(&wclass);       //this registers your window with windows   
//
//	hWnd = CreateWindow(ClassName, "PlanetCpp - RichEdit example",
//		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 400, 300, NULL, NULL, hInstance, NULL);
//	CenterWindow(hWnd);
//
//	while (GetMessage(&msg, NULL, 0, 0))//getmessage loops until a message is on the queue   
//	{                               //then it returns focus,peekmessage does the same except returns focus no matter what   
//		TranslateMessage(&msg);     //translate the message into its char equivelent   
//		DispatchMessage(&msg);      //dispatch to the window   
//	}
//	return msg.wParam;
//}
//////////////////////////////////////////////   
//void CenterWindow(HWND cwin)
//{
//	int width = 0, height = 0;
//	RECT workarea, winrect;
//	GetWindowRect(cwin, &winrect);
//	width = winrect.right - winrect.left;
//	height = winrect.bottom - winrect.top;
//	SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);
//	SetWindowPos(cwin, 0, (workarea.right / 2) - (width / 2), (workarea.bottom / 2) - (height / 2),
//		0, 0, SWP_NOSIZE | SWP_NOZORDER);
//}
#include "stdafx.h"
#include "DelvieryService.h"

#define MAX_LOADSTRING 100

using namespace std;
class Order {
public:
	char name[30];
	char address[30];
	int weight;
	bool priority;
};

list<Order> orders;

// Global Variables:
HWND mainWnd;									// main window instace
HMENU hmenu;									// menu instance
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
char adminPassword[MAX_LOADSTRING];				// password
int priceMultiplier;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Customer(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Service(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Login(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DELVIERYSERVICE, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_PASSWORD, adminPassword, MAX_LOADSTRING);
	char priceMultiplierString[MAX_LOADSTRING];
	LoadString(hInstance, IDC_PRICE, priceMultiplierString, MAX_LOADSTRING);
	priceMultiplier = atoi(priceMultiplierString);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DELVIERYSERVICE));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DELVIERYSERVICE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DELVIERYSERVICE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 341, 390, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	mainWnd = hWnd;
	hmenu = GetMenu(hWnd);
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case BTN_SERVICE_ID:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SERVICE), hWnd, Service);
			break;
		case BTN_CUSTOMER_ID:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CUSTOMER), hWnd, Customer);
			break;
		case ID_FILE_LOGIN:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_LOGIN), hWnd, Login);
			break;
		case IDM_FILE_SAVE:
		{
			HANDLE hCreateFile = CreateFile(_T("save.csv"),               // file to open
				GENERIC_WRITE,          // open for writing
				0,       // share for writing
				NULL,                  // default security
									   //  CREATE_NEW,         // existing file only
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, // normal file
				NULL);

			DWORD dwBytesWritten = 0;
			char orderList[20000];
			memset(orderList, 0, sizeof orderList);
			strcat_s(orderList, 255, "Name;Address;Weight;Priority\r\n");
			for (list<Order>::iterator it = orders.begin(); it != orders.end(); it++) {
				char cBuffer[256], weight[20];
				memset(cBuffer, 0, sizeof cBuffer);
				strcat_s(cBuffer, 30, it->name);
				strcat_s(cBuffer, 30, ";");
				strcat_s(cBuffer, 30, it->address);
				strcat_s(cBuffer, 30, ";");
				sprintf_s(weight, "%d", it->weight);
				strcat_s(cBuffer, 30, weight);
				if (it->priority) {
					strcat_s(cBuffer, 30, ";1");
				}
				else {
					strcat_s(cBuffer, 30, ";0");
				}
				strcat_s(cBuffer, 30, "\r\n");
				strcat_s(orderList, 255, cBuffer);
			}
			WriteFile(hCreateFile, orderList, strlen(orderList), &dwBytesWritten, NULL);
		}
		break;
		case IDM_FILE_LOAD:
		{
			HANDLE hOpenFile = CreateFile(_T("save.csv"),               // file to open
				GENERIC_READ,          // open for writing
				0,       // share for writing
				NULL,                  // default security
									   //  CREATE_NEW,         // existing file only
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, // normal file
				NULL);
			char orderList[20000];
			memset(orderList, 0, sizeof orderList);
			DWORD bytes_read = 0;
			bool flag = ReadFile(hOpenFile, &orderList, 20000, &bytes_read, NULL);
			CloseHandle(hOpenFile);

			char *listNextSeparator = NULL;
			char *rowNextSeparator = NULL;
			char *line = strtok_s(_strdup(orderList), "\r\n", &listNextSeparator);
			line = strtok_s(NULL, "\r\n", &listNextSeparator);//ship header
			while (line) {
				char *orderRow = strtok_s(_strdup(line), ";", &rowNextSeparator);
				Order order;
				int i = 0;
				char weight[30];
				while (orderRow) {
					switch (i)
					{
					case 0:
						strcpy_s(order.name, orderRow);
						break;
					case 1:
						strcpy_s(order.address, orderRow);
						break;
					case 2:
						strcpy_s(weight, orderRow);
						order.weight = atoi(weight);
						break;
					case 3:
						order.priority = (strcmp(orderRow, "1") == 0);
						break;
					} 
					i++;
					orderRow = strtok_s(NULL, ";", &rowNextSeparator);
				}
				orders.push_back(order);
				line = strtok_s(NULL, "\r\n", &listNextSeparator);
			}
			orders;
		}
		break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		CreateWindowW(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Service",      // Button text 
			WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			60,         // x position 
			130,         // y position 
			210,        // Button width
			20,        // Button height
			hWnd,     // Parent window
			(HMENU)BTN_SERVICE_ID,       // No menu.
			NULL,
			NULL);      // Pointer not needed.

		CreateWindowW(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Customer",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			60,         // x position 
			100,         // y position 
			210,        // Button width
			20,        // Button height
			hWnd,     // Parent window
			(HMENU)BTN_CUSTOMER_ID,       // No menu.
			NULL,
			NULL);      // Pointer not needed.

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK Customer(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	static HBITMAP lowBmp, topBmp;
	switch (message)
	{
	case WM_INITDIALOG:
		lowBmp = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_LOW));
		topBmp = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TOP));
		SendMessage(GetDlgItem(hDlg, IDC_PICTURE), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)lowBmp);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDC_ORDER:
		{
			char name[MAX_LOADSTRING], address[MAX_LOADSTRING], weight[MAX_LOADSTRING];
			GetWindowTextA(GetDlgItem(hDlg, IDC_NAME), name, MAX_LOADSTRING);
			GetWindowTextA(GetDlgItem(hDlg, IDC_ADDRESS), address, MAX_LOADSTRING);
			GetWindowTextA(GetDlgItem(hDlg, IDC_WEIGHT), weight, MAX_LOADSTRING);

			if (strcmp(name, "") == 0 || strcmp(address, "") == 0 || strcmp(weight, "") == 0) {
				int val = MessageBoxW(hDlg, L"You did not enter anything.", NULL, MB_RETRYCANCEL);
				switch (val)
				{
				case IDCANCEL:
					EndDialog(hDlg, LOWORD(wParam));
					break;
				case IDRETRY:
					return 0;
				}
			}
			Order order;
			strcpy_s(order.name, name);
			strcpy_s(order.address, address);
			order.weight = atoi(weight);
			order.priority = IsDlgButtonChecked(hDlg, IDC_PRIORITY);
			orders.push_back(order);

			//calculate price
			double price = calculatePrice(order.weight, order.priority);
			WCHAR buff[500];
			swprintf_s(buff, L"The price is %.2f", price);
			MessageBoxW(hDlg, buff, NULL, MB_OK);

			//Reset fields 
			CheckDlgButton(hDlg, IDC_PRIORITY, BST_UNCHECKED);
			SetWindowText(GetDlgItem(hDlg, IDC_NAME), TEXT(""));
			SetWindowText(GetDlgItem(hDlg, IDC_ADDRESS), TEXT(""));
			SetWindowText(GetDlgItem(hDlg, IDC_WEIGHT), TEXT(""));
			break;
		}
		case IDC_CLOSE:
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		}
		case IDC_PRIORITY:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (IsDlgButtonChecked(hDlg, IDC_PRIORITY)) {
					SendMessage(GetDlgItem(hDlg, IDC_PICTURE), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)topBmp);
				}
				else {
					SendMessage(GetDlgItem(hDlg, IDC_PICTURE), STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)lowBmp);
				}
				break;
			}
		}
		break;
		}
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Service(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		HWND hwndList = GetDlgItem(hDlg, IDC_SERVICE_LIST);
		int position = 0;
		for (list<Order>::iterator it = orders.begin(); it != orders.end(); it++) {
			char *cBuffer = new char[256];
			strcpy_s(cBuffer, 256, it->name);

			int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)it->name);
			SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM)position);
			delete[] cBuffer;

			position++;

		}
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDC_SERVICE_CLOSE:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Login(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
		break;
	case WM_COMMAND:
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDC_LOGIN_OK:
		{
			char password[MAX_LOADSTRING];
			GetWindowTextA(GetDlgItem(hDlg, IDC_LOGIN_PASSWORD), password, MAX_LOADSTRING);
			if (strcmp(password, "") == 0 || strcmp(password, adminPassword) != 0) {
				int val = MessageBoxW(hDlg, L"You did not enter anything.", NULL, MB_RETRYCANCEL);
				switch (val)
				{
				case IDCANCEL:
					EndDialog(hDlg, LOWORD(wParam));
					break;
				case IDRETRY:
					return 0;
				}
			}
			else {
				ShowWindow(GetDlgItem(mainWnd, BTN_SERVICE_ID), SW_SHOW);
				UpdateWindow(mainWnd);
				EnableMenuItem(hmenu, ID_FILE_LOGIN, MF_DISABLED);
				EnableMenuItem(hmenu, IDM_FILE_SAVE, MF_ENABLED);
				EnableMenuItem(hmenu, IDM_FILE_LOAD, MF_ENABLED);
				EndDialog(hDlg, LOWORD(wParam));
			}
			break;
		}
		case IDC_LOGIN_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

double calculatePrice(int weight, bool priority) {
	double price = weight * priceMultiplier;
	if (priority) {
		price = price * 1.5;
	}
	return price;
}
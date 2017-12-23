#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <CommCtrl.h>

#pragma comment( lib, "comctl32.lib" )

static TCHAR szWindowClass[] = _T("win32app");

static TCHAR szTitle[] = _T("Win32 App - Fedor Kalugin");

HINSTANCE hInst;
HWND hSpeedTrack;
HWND hSpeedLabel;
HWND hPolyPrimary;
HWND hPolySecondary;

const int screenH = 1080;
const int screenW = 1920;

const int polysize = 300;
POINT poly[4] = { {0,0}, { polysize/4, polysize}, { polysize*3/4,polysize}, {polysize,0 } };

long speedInc = 0;
const int polyX = 500;
int polyY = 300;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateControls(HWND hwnd);
void UpdateLabel();

void CreateControls(HWND hwnd) {

	HWND hLeftLabel = CreateWindowW(L"Static", L"-50",
		WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, (HMENU)1, NULL, NULL);

	HWND hRightLabel = CreateWindowW(L"Static", L"50",
		WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, (HMENU)2, NULL, NULL);

	hSpeedLabel = CreateWindowW(L"Static", L"0", WS_CHILD | WS_VISIBLE,
		270, 20, 30, 30, hwnd, (HMENU)3, NULL, NULL);

	INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	hSpeedTrack = CreateWindowW(TRACKBAR_CLASSW, L"Trackbar Control",
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
		40, 20, 170, 30, hwnd, (HMENU)3, NULL, NULL);

	SendMessageW(hSpeedTrack, TBM_SETRANGE, TRUE, MAKELONG(-50, 50));
	SendMessageW(hSpeedTrack, TBM_SETPAGESIZE, 0, 10);
	SendMessageW(hSpeedTrack, TBM_SETTICFREQ, 10, 0);
	SendMessageW(hSpeedTrack, TBM_SETPOS, FALSE, 0);
	SendMessageW(hSpeedTrack, TBM_SETBUDDY, TRUE, (LPARAM)hLeftLabel);
	SendMessageW(hSpeedTrack, TBM_SETBUDDY, FALSE, (LPARAM)hRightLabel);
}

void UpdateLabel() {
	LRESULT pos = SendMessageW(hSpeedTrack, TBM_GETPOS, 0, 0);
	wchar_t buf[4];
	wsprintfW(buf, L"%ld", pos);
	speedInc = pos;
	SetWindowTextW(hSpeedLabel, buf);
}


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	hInst = hInstance; 

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		350, 150,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 App"),
			NULL);

		return 1;
	}

	HWND hWndHidden = CreateWindow(szWindowClass, NULL, NULL, NULL, NULL, 500, 500,
		NULL,NULL, hInstance,NULL);

	hPolyPrimary = CreateWindowW(szWindowClass, szTitle, WS_POPUP,
		polyX, polyY, polysize, polysize, hWndHidden, nullptr, hInstance, nullptr);

	HRGN hRgn = CreatePolygonRgn(poly, 4, WINDING);
	SetWindowRgn(hPolyPrimary, hRgn, TRUE);
	SetTimer(hPolyPrimary, 1, 10, NULL);

	ShowWindow(hPolyPrimary, nCmdShow);
	UpdateWindow(hPolyPrimary);

	hPolySecondary = CreateWindowW(szWindowClass, szTitle, WS_POPUP,
		polyX, polyY, polysize, polysize, hWndHidden, nullptr, hInstance, nullptr);
	HRGN hRgn2 = CreatePolygonRgn(poly, 4, WINDING);
	SetWindowRgn(hPolySecondary, hRgn2, TRUE);
	SetTimer(hPolySecondary, 1, 10, NULL);

	ShowWindow(hPolySecondary, nCmdShow);
	UpdateWindow(hPolySecondary);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CreateControls(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

void paint(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC 		hdc;

	hdc = BeginPaint(hWnd, &ps);

	EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
		paint(hWnd);
		break;
	case WM_HSCROLL:
		UpdateLabel();
		break;
	case WM_TIMER:
	{
		if (hWnd == hPolyPrimary) {
			polyY += speedInc;
			if (polyY > screenH)
				polyY = 0;
			if (polyY < -polysize)
				polyY = screenH - polysize;
			MoveWindow(hWnd, polyX, polyY, polysize, polysize, TRUE);
		}
		if (hWnd == hPolySecondary) {
			int y_sec;
			bool isOnTop = polyY > -polysize && polyY < 0;
			bool isOnBot = polyY > screenH - polysize && polyY <= screenH;
			if (isOnTop || isOnBot) {
				if (isOnTop)
					y_sec = polyY + screenH;
				else
					y_sec = polyY - screenH;
				MoveWindow(hPolySecondary, polyX, y_sec, polysize, polysize, TRUE);
			}
			else {
				MoveWindow(hPolySecondary, polyX, screenH, polysize, polysize, TRUE);
			}
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
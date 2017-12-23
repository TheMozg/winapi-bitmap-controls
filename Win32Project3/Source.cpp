#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "bitmap.h"

static TCHAR szWindowClass[] = _T("win32app");

static TCHAR szTitle[] = _T("Win32 App - Fedor Kalugin");

static const BOOL drawFast = TRUE;

HINSTANCE hInst;

HBITMAP hBitmap;
HBITMAP hBitmapOut;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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

	wcex.hbrBackground = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
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
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1000, 1000,
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

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

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
	BITMAP 		bitmap;
	HDC 		hdcMemBitmap;
	HDC			hdcMemOut;
	HGDIOBJ 	oldBitmap;
	HGDIOBJ		oldOutBitmap;

	hdc = BeginPaint(hWnd, &ps);

	GetObject(hBitmap, sizeof(bitmap), &bitmap);

	hdcMemBitmap = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMemBitmap, hBitmap);

	hdcMemOut = CreateCompatibleDC(hdc);
	hBitmapOut = CreateCompatibleBitmap(hdc, bitmap.bmWidth, bitmap.bmHeight);
	oldOutBitmap = SelectObject(hdcMemOut, hBitmapOut);
	RECT rect{ 0, 0, bitmap.bmWidth, bitmap.bmHeight };
	FillRect(hdcMemOut, &rect, CreateSolidBrush(RGB(0x00, 0x00, 0xF0)));

	if (drawFast)
	{
		// create intermediate memory dc as a mask without blue channel
		HDC hdcMemTmp = CreateCompatibleDC(hdc);
		HBITMAP hTmpBitmap = CreateCompatibleBitmap(hdc, bitmap.bmWidth, bitmap.bmHeight);
		HGDIOBJ oldTmpBitmap = SelectObject(hdcMemTmp, hTmpBitmap);
		RECT rect{ 0, 0, bitmap.bmWidth, bitmap.bmHeight };
		FillRect(hdcMemTmp, &rect, CreateSolidBrush(RGB(0xFF, 0xFF, 0x00)));

		// transfer bitmap removing blue channel
		BitBlt(hdcMemTmp, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMemBitmap, 0, 0, SRCAND);
		// transfer bitmap again, this time set blue channel to 0xF0
		BitBlt(hdcMemOut, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMemTmp, 0, 0, SRCPAINT);
		
		SelectObject(hdcMemTmp, oldTmpBitmap);
		DeleteDC(hdcMemTmp);
	}
	else
	{
		for (size_t x = 0; x < bitmap.bmWidth; x++)
		{
			for (size_t y = 0; y < bitmap.bmHeight; y++)
			{
				COLORREF pixel = GetPixel(hdcMemBitmap, x, y);
				COLORREF newpixel = RGB(GetRValue(pixel), GetGValue(pixel), 0xF0);
				SetPixel(hdcMemOut, x, y, newpixel);
			}
		}
	}

	BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMemOut, 0, 0, SRCCOPY);

	SelectObject(hdcMemBitmap, oldBitmap);
	DeleteDC(hdcMemBitmap);

	SelectObject(hdcMemOut, oldOutBitmap);
	DeleteDC(hdcMemOut);

	EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		hBitmap = (HBITMAP)LoadImage(hInst, L"in.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	case WM_PAINT:
		paint(hWnd);
		break;
	case WM_DESTROY:
		PBITMAPINFO bmpinfo;
		bmpinfo = CreateBitmapInfoStruct(hWnd, hBitmapOut);
		CreateBMPFile(hWnd, L"out.bmp", bmpinfo, hBitmapOut, GetDC(hWnd));
		DeleteObject(hBitmap);
		DeleteObject(hBitmapOut);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
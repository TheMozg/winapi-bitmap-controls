#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "bitmap.h"
#include "resource.h"
static TCHAR szWindowClass[] = _T("win32app");

static TCHAR szTitle[] = _T("Win32 App - Fedor Kalugin");

HINSTANCE hInst;

HBITMAP hBitmap;
HBITMAP hBitmapOut;

BOOL orig = TRUE;

BYTE* lpPixels;

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
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);
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
		500, 500,
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
	HDC 		hdcMemBitmap;
	HDC			hdcMemOut;
	HGDIOBJ 	oldBitmap;
	HGDIOBJ		oldOutBitmap;
	BITMAPINFO	bitmapInfo;

	hdc = BeginPaint(hWnd, &ps);

	hdcMemBitmap = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMemBitmap, hBitmap);

	// Get the BITMAPINFO structure from the bitmap
	bitmapInfo = { 0 };
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	GetDIBits(hdc, hBitmap, 0, 0, NULL, &bitmapInfo, DIB_RGB_COLORS);

	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	// Call GetDIBits a second time, this time to get pixel data
	GetDIBits(hdc, hBitmap, 0, bitmapInfo.bmiHeader.biHeight, lpPixels, &bitmapInfo, DIB_RGB_COLORS);

	if (orig == FALSE)
	{
		BYTE* pCurrPixel = lpPixels;
		for (size_t y = 0; y < bitmapInfo.bmiHeader.biHeight; y++)
		{
			for (size_t x = 0; x < bitmapInfo.bmiHeader.biWidth; x++)
			{
				for (size_t color = 0; color < 3; color++)
				{
					if (pCurrPixel[color] < 128)
						pCurrPixel[color] *= 2;
					else
						pCurrPixel[color] = 255;
				}
				pCurrPixel += 4;
			}
		}
	}

	hdcMemOut = CreateCompatibleDC(hdc);
	hBitmapOut = CreateCompatibleBitmap(hdc, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight);
	oldOutBitmap = SelectObject(hdcMemOut, hBitmapOut);

	SetDIBits(hdc, hBitmapOut, 0, bitmapInfo.bmiHeader.biHeight, lpPixels, &bitmapInfo, DIB_RGB_COLORS);

	BitBlt(hdc, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, hdcMemOut, 0, 0, SRCCOPY);

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
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_SAVE:
			PBITMAPINFO bmpinfo;
			bmpinfo = CreateBitmapInfoStruct(hWnd, hBitmapOut);
			CreateBMPFile(hWnd, L"out.bmp", bmpinfo, hBitmapOut, GetDC(hWnd));
			break;
		case ID_PICTURE_TOGGLECONTRAST:
			if (orig == TRUE)
				orig = FALSE;
			else
				orig = TRUE;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_CREATE:
		hBitmap = (HBITMAP)LoadImage(hInst, L"in.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		PBITMAPINFO info;
		info = CreateBitmapInfoStruct(hWnd, hBitmap);
		lpPixels = new BYTE[info->bmiHeader.biSizeImage];
		break;
	case WM_PAINT:
		paint(hWnd);
		break;
	case WM_DESTROY:
		delete lpPixels;
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
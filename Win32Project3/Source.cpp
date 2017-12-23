#include <windows.h>
#include <tchar.h>

HINSTANCE hInst;
TCHAR *szClassName = _T("Win32 - Fedor Kalugin");

class Application {

public:

	void run() {
		MSG msg;

		while (GetMessage(&msg, NULL, NULL, NULL)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Application(HINSTANCE hInst, int nCmdShow) {

		RegClass(hInst);

		hWnd = CreateWindow(szClassName, szClassName, WS_OVERLAPPEDWINDOW,
			NULL, NULL, 400, 400, NULL, NULL, HINSTANCE(hInst), NULL);
		ShowWindow(hWnd, nCmdShow);

		UpdateWindow(hWnd);

	}

	~Application(void) {
		SendMessage(hWnd, WM_CLOSE, NULL, NULL);
	}

private:

	HWND hWnd;

	ATOM RegClass(HINSTANCE hInstance) {
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(wcex);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = szClassName;
		wcex.cbWndExtra = NULL;
		wcex.cbClsExtra = NULL;
		wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wcex.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.hInstance = hInst;

		if (!RegisterClassEx(&wcex)) return false;
	}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_CREATE:
		break;

	case WM_DESTROY: {
		PostQuitMessage(NULL);
		break;
	}

	case WM_KEYDOWN: {
		MyWindow a;
		a.run();
		break;
	}

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return TRUE;
}

	class MyWindow {
		HWND  _hWnd;
	public:
		MyWindow() {
			create_window();
		}

		void run() {
			message_loop();
		}

		~MyWindow(void) {
			SendMessage(_hWnd, WM_CLOSE, NULL, NULL);
		}

	private:
		void create_window() {
			_hWnd = CreateWindow(szClassName, szClassName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
				0, 100, 100, NULL, NULL, GetModuleHandle(NULL), NULL);

			ShowWindow(_hWnd, SW_SHOWNOACTIVATE);
			UpdateWindow(_hWnd);
		}

		void message_loop() {
			MSG msg;

			while (GetMessage(&msg, NULL, NULL, NULL)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	};
};

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {

	Application t(hInst, nCmdShow);
	t.run();
	return 0;
}
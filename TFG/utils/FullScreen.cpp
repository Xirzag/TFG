#include "FullScreen.h"

void FullScreen::set()
{
	glutFullScreen();
}

void FullScreen::set_on_window(int index, const char* windowName)
{
#ifdef _WIN32

	HWND hwnd = windowName == nullptr ? GetForegroundWindow() : FindWindow(NULL, windowName);

	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_MAXIMIZE);

	struct ScreenParameter {
		RECT screenRect;
		int screenIndex;
	}screenParameter{ RECT(), index};

	EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> int {
		MONITORINFOEX iMonitor;
		iMonitor.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &iMonitor);

		ScreenParameter& info = *(ScreenParameter*)dwData;

		if (info.screenIndex == 0) {
			info.screenRect = iMonitor.rcMonitor;
			return false;
		}
		else {
			info.screenIndex--;
			return true;
		}
	}, (LPARAM)&screenParameter);

	SetWindowPos(hwnd, HWND_NOTOPMOST,
		screenParameter.screenRect.left, screenParameter.screenRect.top,
		screenParameter.screenRect.right - screenParameter.screenRect.left,
		screenParameter.screenRect.bottom - screenParameter.screenRect.top,
		SWP_SHOWWINDOW);

#else
	#warn "FullScreen on a selected windows is not implemented. Use FullScreen::set"
	FullScreen::set();
#endif
}

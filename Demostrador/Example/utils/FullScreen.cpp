#include "FullScreen.h"

void FullScreen::set()
{
	glutFullScreen();
}

void FullScreen::set_on_screen(int index, const char* windowName)
{
#ifdef _WIN32

	HWND hwnd = windowName == nullptr ? GetForegroundWindow() : FindWindow(NULL, windowName);

	struct ScreenParameter {
		RECT screenRect;
		int screenIndex;
	}screenParameter{ RECT(), index};

	EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> int {
		MONITORINFOEX iMonitor;
		iMonitor.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &iMonitor);

		ScreenParameter& info = *(ScreenParameter*)dwData;
		info.screenRect = iMonitor.rcMonitor;

		if (info.screenIndex == 0) {
			return false;
		}
		else {
			info.screenIndex--;
			return true;
		}
	}, (LPARAM)&screenParameter);

	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_MAXIMIZE);
	SetWindowPos(hwnd, HWND_NOTOPMOST,
		screenParameter.screenRect.left, screenParameter.screenRect.top,
		screenParameter.screenRect.right - screenParameter.screenRect.left,
		screenParameter.screenRect.bottom - screenParameter.screenRect.top,
		SWP_SHOWWINDOW);

#else
	#warn "FullScreen on a selected screen is not implemented for this OS. Use FullScreen::set"
	FullScreen::set();
#endif
}

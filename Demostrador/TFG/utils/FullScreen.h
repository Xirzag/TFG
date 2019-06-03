#pragma once
#include <GL\freeglut.h>
#include <iostream>

namespace FullScreen
{
	//! puts in fullscreen mode the glut window 
	void set();
	//! puts in fullscreen mode the glut window in a specified window
	void set_on_screen(int index /*! index of the window */,
		const char* windowName = nullptr /*! name of the window. If not specify, search the active window */
	);
};


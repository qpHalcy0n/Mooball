#include "windowsvc.h"
#include <stdio.h>

#ifndef WIN32
#include <X11/cursorfont.h>
#endif

bool	g_fullscreen   = false;
bool	quit_requested = false;
bool	g_bMouseHidden = true;
bool    g_bRunning     = true;

int		g_screenWidth  = 640;
int		g_screenHeight = 480;

#ifdef WIN32
	static HGLRC		g_render_context;
	static HDC			g_device_context;
	static HWND			g_window_handle;
	static HINSTANCE	g_window_instance = NULL;
#else
	typedef struct
	{
		Display *display;
		Window window;
		GLXContext context;
		XSetWindowAttributes attrib;
		bool fullscreen;
		XF86VidModeModeInfo vidMode; //desktop res and shit
		int x, y; //where the window will be when opened at load
		unsigned int width, height, depth;
		int screen; // the window ID in X
		char *title;
	} _Window;

	static _Window win;
	static XEvent event;
	//attributes for signle buffered display
	static int attribSgl[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 4,
		GLX_GREEN_SIZE, 4,
		GLX_BLUE_SIZE, 4,
		GLX_DEPTH_SIZE, 16, None
	};

	//attributes for double buffered display
	static int attribDbl[] = {
		GLX_RGBA, GLX_DOUBLEBUFFER,
		GLX_RED_SIZE, 4,
		GLX_GREEN_SIZE, 4,
		GLX_BLUE_SIZE, 4,
		GLX_DEPTH_SIZE, 16, None
	};
#endif


#ifdef WIN32
static LRESULT CALLBACK WndProc(HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
	int cx, cy;

	switch(uMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_KEYDOWN:
			ActivateKey(wParam);
			break;

		case WM_KEYUP:
			DeactivateKey(wParam);
			break;

		case WM_LBUTTONDOWN:
			cx = GET_X_LPARAM(lParam);
			cy = GET_Y_LPARAM(lParam);
			ActivateMouseButton(true, false, cx, cy);
			break;

		case WM_RBUTTONDOWN:
			cx = GET_X_LPARAM(lParam);
			cy = GET_Y_LPARAM(lParam);
			ActivateMouseButton(false, true, cx, cy);
			break;

		case WM_LBUTTONUP:
			cx = GET_X_LPARAM(lParam);
			cy = GET_Y_LPARAM(lParam);
			DeactivateMouseButton(true, false, cx, cy);
			break;

		case WM_RBUTTONUP:
			cx = GET_X_LPARAM(lParam);
			cy = GET_Y_LPARAM(lParam);
			DeactivateMouseButton(false, true, cx, cy);
			break;
		case WM_SIZE:
			cx = GET_X_LPARAM(lParam);
			cy = GET_Y_LPARAM(lParam);
			g_screenWidth = cx;
			g_screenHeight = cy;
			break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



static bool CreateAppWindowWin(char* winTitle, int winWidth, int winHeight, int bpp, bool fullscreen)
{
	GLuint pixFormat;
	WNDCLASS	window_class;
	DWORD		extended_style;
	DWORD		style;

	RECT		windowRect;
	windowRect.left = (long)0;
	windowRect.right = (long)winWidth;
	windowRect.top = (long)0;
	windowRect.bottom = (long)winHeight;

	bool useFullscreen = g_fullscreen;
	g_window_instance = GetModuleHandle(NULL);
	window_class.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc	= (WNDPROC)WndProc;
	window_class.cbClsExtra		= 0;
	window_class.cbWndExtra		= 0;
	window_class.hInstance		= g_window_instance;
	window_class.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	window_class.hCursor		= LoadCursor(NULL, IDC_ARROW);
	window_class.hbrBackground	= NULL;
	window_class.lpszMenuName	= NULL;
	window_class.lpszClassName	= "Mooball";

	if(!RegisterClass(&window_class))
		return false;


	if(useFullscreen)
	{
		DEVMODE		devmode_screen_settings;
		memset(&devmode_screen_settings, 0, sizeof(DEVMODE));

		devmode_screen_settings.dmSize = sizeof(devmode_screen_settings);
		devmode_screen_settings.dmPelsWidth			= winWidth;
		devmode_screen_settings.dmPelsHeight		= winHeight;
		devmode_screen_settings.dmBitsPerPel		= bpp;
		devmode_screen_settings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if(ChangeDisplaySettings(&devmode_screen_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			useFullscreen = false;
	}

	if(useFullscreen)
	{
		extended_style = WS_EX_APPWINDOW;
		style = WS_POPUP;
		ShowCursor(FALSE);
	}

	else
	{
		extended_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		style = WS_OVERLAPPEDWINDOW;
	}

	g_fullscreen = useFullscreen;
	AdjustWindowRectEx(&windowRect, style, FALSE, extended_style);

	if(!(g_window_handle = CreateWindowEx(extended_style, "Mooball", winTitle, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | style, 0, 0,
										  windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL,
										  NULL, g_window_instance, NULL)))
		return false;

	// Pixel format descriptor is required for getting a valid pixel format to create the
	// GL render context which represents the API layer //
	static PIXELFORMATDESCRIPTOR pix_format_desc =
	{
		sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 
		bpp, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	// Create a device context //
	if(!(g_device_context = GetDC(g_window_handle)))
		return false;

	// Get a valid pixel format //
	if(!(pixFormat = ChoosePixelFormat(g_device_context, &pix_format_desc)))
		return false;

	// Make this new pixel format current //
	if(!SetPixelFormat(g_device_context, pixFormat, &pix_format_desc))
		return false;

	// Make a new render context from the device //
	if(!(g_render_context = wglCreateContext(g_device_context)))
		return false;

	// Register it with wgl as the current render device //
	if(!wglMakeCurrent(g_device_context, g_render_context))
		return false;

	// Show the window and make it the topmost //
	ShowWindow(g_window_handle, SW_SHOW);
	SetForegroundWindow(g_window_handle);
	SetFocus(g_window_handle);

	// Set default viewport size //
	glViewport(0, 0, winWidth, winHeight);

	// Hide mouse if its requested //
	//ShowCursor(g_bMouseHidden ? FALSE : TRUE);
	SetCursorVisibility(g_bMouseHidden ? false : true);
	return true;
}

static bool QueryQuitMsgWin()
{
	MSG msg;

	// Peek the event queue and remove the event if it's found //
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// Return true if a quit message has been posted //
		if(msg.message == WM_QUIT)
			return true;

		// Redispatch the message //
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			return false;
		}
	}

	return false;
}

static void DestroyAppWindowWin()
{
	if(g_fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	if(g_render_context)
	{
		if(!wglMakeCurrent(NULL, NULL))
		{
			MessageBox(NULL, "Something bad happened", "Kthx", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if(!wglDeleteContext(g_render_context))
		{
			MessageBox(NULL, "Something worse happened", "Kthxbai", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		g_render_context = NULL;
	}

	if(g_device_context && !ReleaseDC(g_window_handle, g_device_context))
	{
		MessageBox(NULL, "Something unimaginable happened", "Kthxbaimgmt", MB_OK | MB_ICONEXCLAMATION);
		g_device_context = NULL;
		return;
	}

	if(g_window_handle && !DestroyWindow(g_window_handle))
	{
		MessageBox(NULL, "The Unthinkable happened", "Kbai", MB_OK | MB_ICONEXCLAMATION);
		g_window_handle = NULL;
		return;
	}

	if(!UnregisterClass("Mooball", g_window_instance))
	{
		MessageBox(NULL, "You Fail", "Jackass", MB_OK | MB_ICONEXCLAMATION);
		g_window_instance = NULL;
		return;
	}
}

static void FlipBuffersWin()
{
	SwapBuffers(g_device_context);
}

static void SetMousePositionWin(int x, int y)
{
	POINT cp;
	cp.x = x;
	cp.y = y;

	ClientToScreen(g_window_handle, &cp);
	SetCursorPos(cp.x, cp.y);
	SetMousePos(x, y);
}

static s_window_placement GetWindowLocationWin()
{
	WINDOWPLACEMENT wp;
	GetWindowPlacement(g_window_handle, &wp);

	if((wp.rcNormalPosition.right - wp.rcNormalPosition.left) > g_screenWidth)
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + g_screenWidth;

	if((wp.rcNormalPosition.bottom - wp.rcNormalPosition.top) > g_screenHeight)
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + g_screenHeight;

	s_window_placement ret;
	ret.left = wp.rcNormalPosition.left;
	ret.right = wp.rcNormalPosition.right;
	ret.bottom = wp.rcNormalPosition.bottom;
	ret.top = wp.rcNormalPosition.top;

	return ret;
}

static void SetCursorVisibilityWin(int toggle)
{
	if(toggle)
	{
		ShowCursor(TRUE);
	}
	else
	{
		ShowCursor(FALSE);
	}
};

void UpdateSceneWin()
{
	// Update the camera's look direction based on the mouse
	POINT cp;
	GetCursorPos(&cp);

	//	Mouse position is given relative to the window's client area.
	ScreenToClient(g_window_handle, &cp);
	MoveCursor(cp.x, cp.y);
}

#else

static bool CreateAppWindowX(char* title, int width, int height, int bpp, bool fullscreen)
{
	XVisualInfo *VisInfo;
	Colormap colourMap;
	int h, w; //display width height
	int glxMajor, glxMinor; //glx version
	int vidMajor, vidMinor; //video mode version
	XF86VidModeModeInfo **modes;

	int NumModes;
	Atom wmDelete;
	Window winDummy;
	unsigned int borderDummy;

	win.fullscreen = fullscreen;
	win.title = title;
	int currMode = 0;

	win.display = XOpenDisplay(0);
	win.screen = DefaultScreen(win.display);
	XF86VidModeQueryVersion(win.display, &vidMajor, &vidMinor);
	printf("XF86vm version %d.%d\n", vidMajor, vidMinor);

	XF86VidModeGetAllModeLines(win.display, win.screen, &NumModes, &modes);

	win.vidMode = *modes[0];

	//find the mode which suits the the res asked for
	int i;
	for(i = 0; i < NumModes; i++)
	{
		if((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
		{
			currMode = i;
			break;
		}
	}

	VisInfo = glXChooseVisual(win.display, win.screen, attribDbl);

	if(VisInfo == NULL)
	{
		VisInfo = glXChooseVisual(win.display, win.screen, attribSgl);
		printf("Can only use Single buffering\n");
	}
	else
	{
		printf("Successfully initialized Double buffering\n");
	}

	glXQueryVersion(win.display, &glxMajor, &glxMinor);

	printf("GLX version %d.%d\n", glxMajor, glxMinor);

	win.context = glXCreateContext(win.display, VisInfo, 0, GL_TRUE);
	colourMap = XCreateColormap(win.display, RootWindow(win.display, VisInfo->screen), VisInfo->visual, AllocNone);

	win.attrib.colormap = colourMap;
	win.attrib.border_pixel = 0;

	if(win.fullscreen)
	{
		XF86VidModeSwitchToMode(win.display, win.screen, modes[currMode]);
		XF86VidModeSetViewPort(win.display, win.screen, 0, 0);
		w = modes[currMode]->hdisplay;
		h = modes[currMode]->vdisplay;
		printf("Resolution %dx%d\n", w, h);
		XFree(modes);
		win.attrib.override_redirect = True;
		win.attrib.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask | KeyReleaseMask | PointerMotionMask;
		win.window = XCreateWindow(win.display, RootWindow(win.display, VisInfo->screen),
					0, 0, w, h, 0, VisInfo->depth, InputOutput, VisInfo->visual,
					CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
					&win.attrib);
		XWarpPointer(win.display, None, win.window, 0, 0, 0, 0, 0, 0);
		XMapRaised(win.display, win.window);
		XGrabKeyboard(win.display, win.window, True, GrabModeAsync, GrabModeAsync, CurrentTime);
		XGrabPointer(win.display, win.window, True, ButtonPressMask,
				GrabModeAsync, GrabModeAsync, win.window, None, CurrentTime);
	}
	else
	{
		win.attrib.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask | KeyReleaseMask | PointerMotionMask;
		win.window = XCreateWindow(win.display, RootWindow(win.display, VisInfo->screen),
					0, 0, width, height, 0, VisInfo->depth, InputOutput, VisInfo->visual,
					CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
					&win.attrib);
		wmDelete = XInternAtom(win.display, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(win.display, win.window, &wmDelete, 1);
		XSetStandardProperties(win.display, win.window, title,
					title, None, NULL, 0, NULL);
		XMapRaised(win.display, win.window);
	}

	glXMakeCurrent(win.display, win.window, win.context);

	XGetGeometry(win.display, win.window, &winDummy, &win.x, &win.y,
			&win.width, &win.height, &borderDummy, &win.depth);

	return true;
};

static bool DestroyAppWindowX()
{
	if(win.context)
	{
		if(!glXMakeCurrent(win.display, None, NULL))
		{
			printf("could not free OpenGL context\n");
		}
		glXDestroyContext(win.display, win.context);
		win.context = NULL;
	}
	//change desktop res back if fullscreen
	if(win.fullscreen)
	{
		XF86VidModeSwitchToMode(win.display, win.screen, &win.vidMode);
		XF86VidModeSetViewPort(win.display, win.screen, 0, 0);
	}
	XCloseDisplay(win.display);

	return true;
};

static bool QueryQuitMsgX()
{
	return quit_requested;
};

static void FlipBuffersX()
{
	glXSwapBuffers(win.display, win.window);
};

void UpdateSceneUnix()
{


	while(XPending(win.display) > 0)
	{
		XNextEvent(win.display, &event);
		switch(event.type)
		{
			case DestroyNotify:
				quit_requested = true;
				break;

			case KeyPress:
				ActivateKey(XLookupKeysym(&event.xkey, 0));
                break;
			case KeyRelease:
				DeactivateKey(XLookupKeysym(&event.xkey, 0));
                break;
			case ButtonPress:
				//mouse buttons in X are left (1) middle (2) right (3)
				if(event.xbutton.button == 1)
					ActivateMouseButton(true, false, event.xbutton.x, event.xbutton.y);
				else if(event.xbutton.button == 3)
					ActivateMouseButton(false, true, event.xbutton.x, event.xbutton.y);
                break;
			case ButtonRelease:
				if(event.xbutton.button == 1)
					DeactivateMouseButton(true, false, event.xbutton.x, event.xbutton.y);
				else if(event.xbutton.button == 3)
					DeactivateMouseButton(false, true, event.xbutton.x, event.xbutton.y);
                break;

			// The reporting in *nix for the mouse happens in several different spaces
			// In windows it happens globally. Therefore we have to adjust the mouse
			// to a global coordinate frame for the mouse movement to work
			case MotionNotify:
				MoveCursor(win.x + event.xmotion.x, win.y + event.xmotion.y);
                break;
			case ClientMessage:
				if (*XGetAtomName(win.display, event.xclient.message_type) == *"WM_PROTOCOLS")
				{
					quit_requested = true;
				}
				break;
		}
	}
};

static void SetMousePositionX(int x, int y)
{
	Window tmp;
	unsigned int crap;
	XGetGeometry(win.display, win.window, &tmp, &win.x, &win.y, &win.width, &win.height, &crap, &win.depth);

	// Specifying a src window means you have to specify a rect within which the cursor must reside for the RELATIVE
	// update. By only specifying a dest window we can just set the cursor relative to the window origin.
	XWarpPointer(win.display, None, win.window, 0,0,0,0, win.width / 2, win.height / 2);
	SetMousePos(x, y);
//	XWarpPointer(win.display, win.window, win.window, win.x, win.y, win.width, win.height, x, y);
};

static s_window_placement GetWindowLocationX()
{
	Window winDummy;
	unsigned int borderDummy;
	s_window_placement ret;
	XGetGeometry(win.display, win.window, &winDummy, &win.x, &win.y,
			&win.width, &win.height, &borderDummy, &win.depth);
	ret.bottom = win.y + win.height;
	ret.top = win.y;
	ret.left = win.x;
	ret.right = win.x + win.width;
	return ret;
};

static void SetCursorVisibilityX(int toggle)
{
	if(toggle)
	{
		Cursor cursor;
		cursor=XCreateFontCursor(win.display,XC_left_ptr);
		XDefineCursor(win.display, win.window, cursor);
		XFreeCursor(win.display, cursor);
	}
	else
	{
		Cursor invisibleCursor;
		Pixmap bitmapNoData;
		XColor black;
		static char noData[] = { 0,0,0,0,0,0,0,0 };
		black.red = black.green = black.blue = 0;

		bitmapNoData = XCreateBitmapFromData(win.display, win.window, noData, 8, 8);
		invisibleCursor = XCreatePixmapCursor(win.display, bitmapNoData, bitmapNoData,
							&black, &black, 0, 0);
		XDefineCursor(win.display,win.window, invisibleCursor);
		XFreeCursor(win.display, invisibleCursor);
	}
};

#endif

bool CreateAppWindow(char* winTitle, int winWidth, int winHeight, int bpp, bool fullscreen)
{
	g_screenWidth = quickINI::Instance()->getValueAsInt("window_width");
	g_screenHeight = quickINI::Instance()->getValueAsInt("window_height");
	if(g_screenWidth <= 0)
		g_screenWidth = 800;
	if(g_screenHeight <= 0)
		g_screenHeight = 600;

	#ifdef WIN32
		return CreateAppWindowWin(winTitle, g_screenWidth, g_screenHeight, bpp, fullscreen);
	#else
		return CreateAppWindowX(winTitle, g_screenWidth, g_screenHeight, bpp, fullscreen);
	#endif
}

void DestroyAppWindow()
{
	#ifdef WIN32
		DestroyAppWindowWin();
	#else
		DestroyAppWindowX();
	#endif
}


bool QueryQuitMsg()
{
	#ifdef WIN32
		return QueryQuitMsgWin();
	#else
		return QueryQuitMsgX();
	#endif
}

void SendQuitMsg()
{
	#ifdef WIN32
		PostQuitMessage(0);
	#else
		quit_requested = true;
	#endif
}


void FlipBuffers()
{
	#ifdef WIN32
		FlipBuffersWin();
	#else
		FlipBuffersX();
	#endif
}


void SetMousePosition(int x, int y)
{
	#ifdef WIN32
		SetMousePositionWin(x, y);
	#else
		SetMousePositionX(x, y);
	#endif
}

s_window_placement GetWindowLocation()
{
	#ifdef WIN32
		return GetWindowLocationWin();
	#else
		return GetWindowLocationX();
	#endif
}

void SetCursorVisibility(int toggle)
{
	if( toggle )
		g_bMouseHidden = false;
	else
		g_bMouseHidden = true;

	#ifdef WIN32
		SetCursorVisibilityWin(toggle);
	#else
		SetCursorVisibilityX(toggle);
	#endif
}

bool IsCursorHidden()
{
	return g_bMouseHidden;
}


int GetScreenWidth()
{
	return g_screenWidth;
}

int GetScreenHeight()
{
	return g_screenHeight;
}

void GetScreenDims(int& width, int& height)
{
	width = g_screenWidth;
	height = g_screenHeight;
}

void UpdateScene()
{
	#ifdef WIN32
		UpdateSceneWin();
	#else
		UpdateSceneUnix();
	#endif
}


bool IsProgramRunning()
{
	return g_bRunning;
}

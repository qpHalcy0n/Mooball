#ifndef __WINDOWSVC_H_
#define __WINDOWSVC_H_


#include "extensions.h"
#include "input.h"
#include "quickINI.h"

#ifndef WIN32
	#include <X11/Xlib.h>
#endif


typedef struct
{
	int		left;
	int		right;
	int		bottom;
	int		top;
} s_window_placement;



/*
	CreateAppWindow

	Creates the application window with GL render context.
	Returns true on success, false on failure. Application can not proceed if CreateAppWindow fails

	"winTitle" - window title (titlebar)
	"winWidth" - window width (pixels)
	"winHeight" - window height (pixels)
	"bpp" - bits per pixel on the front plane
	"fullscreen" - toggle fullscreen on or off
*/
bool CreateAppWindow(char* winTitle, int winWidth, int winHeight, int bpp, bool fullscreen);


/*
	DestroyAppWindow

	Destroys the application window created with CreateAppWindow
*/
void DestroyAppWindow();


/*
	QueryQuitMsg

	Queries that a quit message has been posted.
	Returns true if it is, if not returns false and redispatches the message to the OS.
*/
bool QueryQuitMsg();


/*
	SendQuitMsg

	Sends a quit message to the system requesting the application terminate
*/
void SendQuitMsg();


/*
	FlipBuffers

	Swaps the back buffer with the primary surface
*/
void FlipBuffers();


/*
	UpdateScene

	Updates all window events for all platforms
*/
void UpdateScene();



/*
	IsProgramRunning

	Query whether a quit message has been posted or not
*/
bool IsProgramRunning();



/*
	SetMousePosition

	Reset mouse position to relative position specified by x, y
*/
void SetMousePosition(int x, int y);


/*
	GetWindowLocation

	Obtain the window's location as a quadruple (left, right, top, bottom)

	"wpl" - array of at least 4 ints
*/
s_window_placement GetWindowLocation();


/*
	SetCursorVisibility

	Show/hide the cursor in the window (1 == show, 0 == hide)
*/
void SetCursorVisibility(int toggle);


/*
	IsCursorHidden

	Query whether the cursor is hidden or not
*/
bool IsCursorHidden();

/*
	GetScreenWidth/Height

	Obtain the screen's internal renderable width & height
*/
int GetScreenWidth();
int GetScreenHeight();
void GetScreenDims(int& width, int& height);


#endif

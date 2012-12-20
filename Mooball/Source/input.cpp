#include "input.h"


static bool		g_bActiveKeys[256];
static bool		g_bMouseClicks[2];
static int		g_curMouseX;
static int		g_curMouseY;

void InitKeys()
{
	memset(g_bActiveKeys, 0, sizeof(bool) * 256);
	memset(g_bMouseClicks, 0, sizeof(bool) * 2);
	
	g_curMouseX = g_curMouseY = 0;
}


void ActivateKey(unsigned int keyVal)
{
	g_bActiveKeys[(unsigned char)keyVal] = true;
}

void DeactivateKey(unsigned int keyVal)
{
	g_bActiveKeys[(unsigned char)keyVal] = false;
}


bool IsKeyDown(unsigned int keyVal)
{
	return g_bActiveKeys[(unsigned char)keyVal];
}

void ActivateMouseButton(bool lButton, bool rButton, int cursorX, int cursorY)
{
	if(lButton)
		g_bMouseClicks[MOUSE_BUTTON_LEFT] = true;
	if(rButton)
		g_bMouseClicks[MOUSE_BUTTON_RIGHT] = true;
	
	g_curMouseX = cursorX;
	g_curMouseY = cursorY;
}

void DeactivateMouseButton(bool lButton, bool rButton, int cursorX, int cursorY)
{
	if(lButton)
		g_bMouseClicks[MOUSE_BUTTON_LEFT] = false;
	if(rButton)
		g_bMouseClicks[MOUSE_BUTTON_RIGHT] = false;
	
	g_curMouseX = cursorX;
	g_curMouseY = cursorY;	
}

void MoveCursor(int newCursorX, int newCursorY)
{
	g_curMouseX = newCursorX;
	g_curMouseY = newCursorY;
}

bool IsMouseButtonPressed(int button)
{
	return g_bMouseClicks[button];
}

int GetMouseX()
{
	return g_curMouseX;
}

int GetMouseY()
{
	return g_curMouseY;
}

void GetMousePosition(int& x, int& y)
{
	x = g_curMouseX;
	y = g_curMouseY;
}

void SetMousePos(int x, int y)
{
	g_curMouseX = x;
	g_curMouseY = y;
}


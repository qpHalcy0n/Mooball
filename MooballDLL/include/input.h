#ifndef __INPUT_H_
#define __INPUT_H_

#include <string.h>


#define MOUSE_BUTTON_LEFT		0
#define MOUSE_BUTTON_RIGHT		1


void InitKeys();

void ActivateKey(unsigned int keyVal);
void DeactivateKey(unsigned int keyVal);

bool IsKeyDown(unsigned int keyVal);

void ActivateMouseButton(bool lButton, bool rButton, int cursorX, int cursorY);
void DeactivateMouseButton(bool lButton, bool rButton, int cursorX, int cursorY);
void MoveCursor(int newCursorX, int newCursorY);

bool IsMouseButtonPressed(int button);
int GetMouseX();
int GetMouseY();
void GetMousePosition(int& x, int& y);
void SetMousePos(int x, int y);


#endif
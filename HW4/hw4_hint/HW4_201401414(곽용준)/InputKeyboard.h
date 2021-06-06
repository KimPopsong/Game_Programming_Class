#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class InputKeyboard
{
private:
	IDirectInput8* di;
	IDirectInputDevice8* diKeyboard;
	char keyBuffer[256];

public:
	InputKeyboard(HWND hWnd);
	~InputKeyboard();
	bool update();
	bool keyPressed(int key);

private:
	void release();
	bool init(HWND hWnd);
};

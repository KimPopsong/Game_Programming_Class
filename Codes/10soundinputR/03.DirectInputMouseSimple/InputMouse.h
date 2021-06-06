#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define DIMOUSE_LEFTBUTTON		0
#define DIMOUSE_RIGHTBUTTON		1
#define DIMOUSE_MIDDLEBUTTON	2
#define DIMOUSE_4BUTTON			3
#define DIMOUSE_5BUTTON			4
#define DIMOUSE_6BUTTON			5
#define DIMOUSE_7BUTTON			6
#define DIMOUSE_8BUTTON			7

class InputMouse
{
private:
	HWND hWnd; //WIN32
	DWORD screenWidth; //copy. 스크린 크기.
	DWORD screenHeight;

	IDirectInput8* di;
	IDirectInputDevice8* diMouse;
	DIMOUSESTATE2 mouseState; //마우스 상태.

	// 마우스 성능.
	DWORD mouseNumAxes; //마우스 축의 개수.
	DWORD mouseNumButtons; //마우스 버튼의 개수.

	float mouseSensitivity; // 마우스 민감도.

public:
	InputMouse(HWND _hWnd);
	~InputMouse();
	bool update();
	bool mouseButtonDown(int Button);

	// 커서의 상대 변이.
	float getRelativeX() { return (float)mouseState.lX * mouseSensitivity; }
	float getRelativeY() { return (float)mouseState.lY * mouseSensitivity; }
	float getRelativeZ() { return (float)mouseState.lZ * mouseSensitivity; }

	bool setScreenSize(DWORD _screenWidth, DWORD _screenHeight);

	// 커서의 절대위치 관련.
	void setCursorPos(POINT* pos);
	void getCursorPos(POINT* pos);
	void getString(char str[]);

private:
	bool init(HWND _hWnd);
	void release();

};

#include "InputMouse.h"
#include <stdio.h> //sprintf_s

InputMouse::InputMouse(HWND _hWnd)
{
	di = NULL;
	diMouse = NULL;

	hWnd = _hWnd;

	init(hWnd);
}

InputMouse::~InputMouse()
{
	release();
}

void InputMouse::release()
{
	if (diMouse != NULL) {
		diMouse->Unacquire();
		diMouse->Release();
		diMouse = NULL;
	}
	if (di != NULL) {
		di->Release();
		di = NULL;
	}
}

bool InputMouse::setScreenSize(DWORD _screenWidth, DWORD _screenHeight)
{
	screenWidth = _screenWidth;
	screenHeight = _screenHeight;
	return true;
}

bool InputMouse::init(HWND hWnd)
{
	// DirectInput 객체를 생성함.

	if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di, NULL))) {
		::MessageBox(NULL, "DirectInput8Create() failed!", "InitDirectInput()", MB_OK);
		return false;
	}

	// 마우스 장치를 초기화함.

	if (FAILED(di->CreateDevice(GUID_SysMouse, &diMouse, NULL))) {
		::MessageBox(NULL, "CreateDevice() failed!", "InitMouse()", MB_OK);
		return false;
	}

	if (FAILED(diMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) {
		::MessageBox(NULL, "SetCooperativeLevel() failed!", "InitMouse()", MB_OK);
		return false;
	}

	if (FAILED(diMouse->SetDataFormat(&c_dfDIMouse2))) {
		::MessageBox(NULL, "SetDataFormat() failed!", "InitMouse()", MB_OK);
		return false;
	}

	if (FAILED(diMouse->Acquire())) {
		::MessageBox(NULL, "Acquire() failed!", "InitMouse()", MB_OK);
		return false;
	}

	DIDEVCAPS mouseCapabilities;
	mouseCapabilities.dwSize = sizeof(mouseCapabilities);
	diMouse->GetCapabilities(&mouseCapabilities);
	if (!(mouseCapabilities.dwFlags & DIDC_ATTACHED)) {
		::MessageBox(NULL, "Mouse is currently not attached!", "InitMouse()", MB_OK);
		return false;
	}

	mouseNumAxes = mouseCapabilities.dwAxes;
	mouseNumButtons = mouseCapabilities.dwButtons;

	mouseSensitivity = 1.0f;

	return true;
}

bool InputMouse::update()
{
	// 마우스 장치를 갱신함.
	if (diMouse->GetDeviceState( sizeof(mouseState), (void*)&mouseState) == DIERR_INPUTLOST) {
		diMouse->Acquire();
	}

	return true;
}

bool InputMouse::mouseButtonDown(int Button)
{
	// 마우스 버턴이 눌러졌는지를 확인함.
	if (mouseState.rgbButtons[Button] & 0x80) {
		return true;
	} else {
		return false;
	}
}

void InputMouse::setCursorPos(POINT* pos)
{
	::ClientToScreen(hWnd, pos); //클라이언트 영역에서의 좌표값을 스크린 전체 영역에서의 좌표값으로 바꾼다.
	::SetCursorPos(pos->x, pos->y); 
}

void InputMouse::getCursorPos(POINT* pos)
{
	::GetCursorPos(pos);
	::ScreenToClient(hWnd, pos); //스크린 좌표값을 클라이언트 좌표값으로 바꾼다.
}

void InputMouse::getString(char str[])
{
	POINT pos;
	getCursorPos(&pos);
	if (pos.x < 0 || pos.x >= (long)screenWidth || pos.y < 0 || pos.y >= (long)screenHeight)
		return;
	sprintf_s(str, 800, "[%d, %d]", pos.x, pos.y);
}

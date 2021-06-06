#include "InputKeyboard.h"

InputKeyboard::InputKeyboard(HWND hWnd)
{
	di = NULL;
	diKeyboard = NULL;

	init(hWnd);
}

InputKeyboard::~InputKeyboard()
{
	release();
}

void InputKeyboard::release()
{
	if (diKeyboard != NULL) {
		diKeyboard->Unacquire();
		diKeyboard->Release();
		diKeyboard = NULL;
	}
	if (di != NULL) {
		di->Release();
		di = NULL;
	}
}

bool InputKeyboard::init(HWND hWnd)
{
	// DirectInput 객체를 생성함.

	if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di, NULL))) {
		::MessageBoxA(NULL, "DirectInput8Create() failed!", "ERROR", MB_OK);
		return false;
	}

	// 키보드 장치를 초기화함.

	if (FAILED(di->CreateDevice(GUID_SysKeyboard, &diKeyboard, NULL))) {
		::MessageBoxA(NULL, "CreateDevice() failed!", "ERROR", MB_OK);
		return false;
	}

	if (FAILED(diKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) {
		::MessageBoxA(NULL, "SetCooperativeLevel() failed!", "ERROR", MB_OK);
		return false;
	}

	if (FAILED(diKeyboard->SetDataFormat(&c_dfDIKeyboard))) {
		::MessageBoxA(NULL, "SetDataFormat() failed!", "ERROR", MB_OK);
		return false;
	}

	if (FAILED(diKeyboard->Acquire())) {
		::MessageBoxA(NULL, "Acquire() failed!", "ERROR", MB_OK);
		return false;
	}

	return true;
}

bool InputKeyboard::update()
{
	//키보드 장치의 상태를 갱신함.
	if (FAILED(diKeyboard->GetDeviceState(sizeof(keyBuffer), (void*)&keyBuffer))) {
		::MessageBoxA(NULL, "GetDeviceState() failed!", "ERROR", MB_OK);
		return false;
	}

	return true;
}

bool InputKeyboard::keyPressed(int key)
{
	// 키가 눌러졌는지를 확인함.
	if (keyBuffer[key] & 0x80) {
		return true;
	}
	else {
		return false;
	}
}

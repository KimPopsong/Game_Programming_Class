#pragma once
#include <d2d1.h>
#include <dwrite.h>
#include <Windows.h>

// �ڿ� ���� ��ȯ ��ũ��.
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_FREE(p) { if (p) { free(p); (p)=NULL; } }

namespace d2d
{
	// WIN32 ���� �Լ�.
	HWND initWin32(HINSTANCE hInstance, int width, int height);
	bool releaseWin32(HINSTANCE hInstance);
	int doMessageLoop(bool (*renderFunction)(float timeDelta));

	// d2d ���� �Լ�
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources(HWND,D2D1_SIZE_U);
	void DiscardDeviceResources();
	void DiscardDeviceIndependentResources();
}

// ����� ���ǿ� WIN32 �Լ�.
#if defined(_DEBUG) && defined(WIN32) && !defined(_AFX) && !defined(_AFXDLL) && !defined(TRACE)
#define TRACE	TRACE_WIN32
void TRACE_WIN32(LPCTSTR lpszFormat, ...);
#define TRACE_WIN32_REQUIRE_IMPL
#endif

//@Author: Jong-Seung Park.


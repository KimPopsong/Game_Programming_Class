#pragma once

// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
// DX Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

// 자원 안전 반환 매크로.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp {
	public:
		DemoApp();
		~DemoApp();
		HRESULT Initialize(HINSTANCE hInstance);
		void RunMessageLoop();
	private:
		HRESULT CreateDeviceIndependentResources();
		HRESULT CreateDeviceResources();
		void DiscardDeviceResources();
		HRESULT OnRender();
		void OnResize(UINT width, UINT height);
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		HWND m_hwnd;
		ID2D1Factory* m_pD2dFactory;
		ID2D1HwndRenderTarget* m_pRenderTarget;
		ID2D1SolidColorBrush* m_pCharColl;
		ID2D1SolidColorBrush* m_pCharImage;
};

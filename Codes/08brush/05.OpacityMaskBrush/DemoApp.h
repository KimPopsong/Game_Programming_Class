#pragma once

// Windows Header Files:
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

// 현재 모듈의 시작주소 얻기.
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp
{
public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize();
	void RunMessageLoop();

private:
	HRESULT CreateDeviceIndependentResources();
	void DiscardDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HRESULT LoadBitmapFromResource(ID2D1RenderTarget* pRT, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, __deref_out ID2D1Bitmap** ppBitmap);

private:
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2DFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pBlackBrush;

	// Rectangle Geometry
	ID2D1RectangleGeometry* m_pRectGeo;

	// Bitmaps
	ID2D1Bitmap* m_pLinearFadeFlowersBitmap;
	ID2D1Bitmap* m_pRadialFadeFlowersBitmap;

	// Gradients
	ID2D1LinearGradientBrush* m_pLinearGradientBrush;
	ID2D1RadialGradientBrush* m_pRadialGradientBrush;

	// Bitmap brushes
	ID2D1BitmapBrush* m_pLinearFadeFlowersBitmapBrush;
	ID2D1BitmapBrush* m_pRadialFadeFlowersBitmapBrush;

	// WIC
	IWICImagingFactory* m_pWICFactory;

	// DWrite
	IDWriteFactory* m_pDirectWriteFactory;
};

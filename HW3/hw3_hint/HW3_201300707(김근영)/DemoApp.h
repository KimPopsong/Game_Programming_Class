#pragma once
#pragma comment(lib,"dwmapi.lib") //Dwmapi.lib�� ����.
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <list>
#include <wchar.h>
#include <math.h>
// DX Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dwmapi.h>
#include "Animation.h"
//
#include "Fly.h"
// �ڿ� ���� ��ȯ ��ũ��.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// ���� ����� �����ּ� ���.
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
	HRESULT CreateDeviceIndependentResources();
	

private:
	HRESULT CreateDeviceResources();
	HRESULT OnRender();
	void DiscardDeviceResources();
	void FlyMove(std::list <Fly*>::iterator iter);
	void GoldBug();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT LoadBitmapFromResource(ID2D1RenderTarget*, IWICImagingFactory*, PCWSTR, PCWSTR, UINT, UINT, ID2D1Bitmap**);

private:
	HWND m_hwnd;
	ID2D1Factory* m_pD2DFactory;
	IDWriteTextFormat* m_pTextFormat;
	IDWriteFactory* m_pDWriteFactory;
	ID2D1HwndRenderTarget* m_pRT;
	ID2D1PathGeometry* m_pPathGeometry;
	ID2D1PathGeometry* m_pObjectGeometry;
	ID2D1PathGeometry* m_FlyGeometry[5];
	ID2D1RectangleGeometry* m_pRectGeo;
	/*Brush*/
	ID2D1SolidColorBrush* m_pPathBrush;
	ID2D1SolidColorBrush* m_pGoldBrush;
	ID2D1BitmapBrush* m_pBitmapBrush;
	ID2D1BitmapBrush* m_pEffectBrush;
	ID2D1LinearGradientBrush* m_pLinearGradientBrush;
	ID2D1RadialGradientBrush* m_pRadialGradientBrush;
	
	ID2D1Bitmap* m_flyBitmap;
	ID2D1Bitmap* m_rainbowBitmap;
	ID2D1Bitmap* m_backgroundBitmap;
	ID2D1Bitmap* m_GoldBugMask;
	ID2D1Bitmap* m_EffectBitmap;
	IWICImagingFactory* m_pWICFactory;
	AnimationLinear<float> m_Animation;
	DWM_TIMING_INFO m_DwmTimingInfo;
	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;

};



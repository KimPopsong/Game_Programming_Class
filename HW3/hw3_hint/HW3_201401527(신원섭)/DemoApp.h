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
#include <Dwmapi.h>

#include "Animation.h"
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
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//HRESULT CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, __deref_out ID2D1BitmapBrush** ppBitmapBrush);

	HRESULT LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, __deref_out ID2D1Bitmap** ppBitmap);
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, __deref_out ID2D1Bitmap** ppBitmap);

private:
	HWND m_hwnd;
	ID2D1Factory* m_pD2DFactory;
	IWICImagingFactory* m_pWICFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1Bitmap* m_pBitmap;
	ID2D1Bitmap* m_pCrowBitmap;
	ID2D1Bitmap* m_pCrowBitmap2;
	ID2D1Bitmap* m_pTreeBitmap;
	ID2D1Bitmap* m_pEnemyBitmap;
	ID2D1Bitmap* m_pHeroBitmap[10];
	ID2D1Bitmap* m_pCrowMotionBitmap[6];


	ID2D1StrokeStyle *m_pStrokeStyleDash;
	//브러쉬
	ID2D1SolidColorBrush* m_pSceneBrush;
	ID2D1SolidColorBrush* m_pStarBrush;
	ID2D1RadialGradientBrush* m_pRadialGradientBrush;
	ID2D1LinearGradientBrush* m_pLinearStarGradientBrush;

	//비트맵
	ID2D1BitmapBrush *m_pCrow_Nomal_BitmapBrush;
	ID2D1BitmapBrush *m_pCrow_Nomal_Bitmap2Brush;
	ID2D1BitmapBrush *m_pTree_Nomal_BitmapBrush;
	ID2D1BitmapBrush *m_pEnemy_Nomal_BitmapBrush;
	ID2D1BitmapBrush *m_pHero_Nomal_BitmapBrush[10];
	ID2D1BitmapBrush *m_pCrowMotion_Nomal_BitmapBrush[6];


	//경로기하
	ID2D1PathGeometry *m_pCrow_PathGeometry; //까마귀
	ID2D1PathGeometry *m_pMoonGeometry; //달
	ID2D1PathGeometry *m_pStarGeometry;
	ID2D1PathGeometry *m_pStarFallGeometry;
	ID2D1PathGeometry *m_pStarFallFireGeometry;



	// Animation
	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;
	CrowAnimation<float> m_Animation;





};

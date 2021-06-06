#pragma once

// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wchar.h>
#include <math.h>
#include <random>
#include <time.h>
// DX Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include "Animation.h"

#include <xaudio2.h>
#include <strsafe.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <conio.h>
#include "FiniteStateMachine.h"
#include "Boss.h"
//#include "Bullet.h"
#include "SoundManager.h"

// 자원 안전 반환 매크로.
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_FREE(p) { if (p) { free(p); (p)=NULL; } }

namespace d3d
{
	// WIN32 관련 함수.
	HWND initWin32(HINSTANCE hInstance, int width, int height);
	bool releaseWin32(HINSTANCE hInstance);
	int doMessageLoop(bool(*renderFunction)(float timeDelta));
}

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp
{
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
	void showInfo();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HRESULT LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap);
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap);


private:
	HWND m_hwnd;
	ID2D1Factory* m_pD2DFactory;
	IWICImagingFactory* m_pWICFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1Bitmap *m_pBackGround;
	ID2D1Bitmap *m_pBackGround2;
	ID2D1Bitmap *m_pBackGround2_mask;
	ID2D1Bitmap *m_pMain_Bitmap1;
	ID2D1Bitmap *m_pMain_Bitmap2;
	ID2D1Bitmap *m_pMain_Bitmap3;
	ID2D1Bitmap *m_pBoss_Bitmap;
	ID2D1Bitmap *m_pMainBullet_Bitmap;
	ID2D1Bitmap *m_pEnemyBullet1_Bitmap;
	ID2D1Bitmap *m_pEnemyBullet2_Bitmap;
	ID2D1Bitmap *m_pClear_Bitmap;
	ID2D1Bitmap *m_pGameOver_Bitmap;

	ID2D1BitmapBrush *m_pBackGround2Brush;
	ID2D1BitmapBrush *m_pBackGround2_maskBrush;
	ID2D1BitmapBrush *m_pMain_Bitmap1_Brush;
	ID2D1BitmapBrush *m_pMain_Bitmap2_Brush;
	ID2D1BitmapBrush *m_pMain_Bitmap3_Brush;
	ID2D1BitmapBrush *m_pBoss_Bitmap_Brush;
	ID2D1BitmapBrush *m_pMainBullet_Bitmap_Brush;
	ID2D1BitmapBrush *m_pEnemyBullet_Bitmap1_Brush;
	ID2D1BitmapBrush *m_pEnemyBullet_Bitmap2_Brush;
	ID2D1BitmapBrush *m_pClear_Bitmap_Brush;
	ID2D1BitmapBrush *m_pGameOver_Bitmap_Brush;

	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	IDWriteTextFormat* m_pTextFormat2;
	ID2D1SolidColorBrush* m_pWhiteBrush;
	ID2D1SolidColorBrush* m_pYellowBrush;
	ID2D1SolidColorBrush* m_pRedBrush;

	ID2D1PathGeometry *m_pBoss_PathGeometry;
	LinearAnimation<float> m_Animation;

	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;

	BOOL Init(HWND hWnd);
	
};
struct Bullet
{
	BOOL Visible;
	D2D1_RECT_F Source;
	D2D_POINT_2F Position;
	D2D_POINT_2F Point;
	float radius;
	float t;
};
struct enemy_Boss
{
	BOOL Visible;
	int hp = 10000;
	D2D1_RECT_F Source;
	D2D_POINT_2F Position;

};


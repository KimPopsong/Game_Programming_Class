#pragma once

// Windows Header Files:
#include <windows.h>
// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
// DX Header Files:
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dwmapi.h>
// Animation
#include "Animation.h"
#include "SoundManager.h"
#include "Character.h"

// 자원 안전 반환 매크로.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// 현재 모듈의 시작주소 얻기.
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}
#else
#define Assert(b)
#endif
#endif

// DemoApp class declaration
class DemoApp
{
public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize(HINSTANCE hInstance);  // Register the window class and call methods for instantiating drawing resources
	void RunMessageLoop();  // Process and dispatch messages

private:
	HRESULT CreateDeviceIndependentResources();  // Initialize device-independent resources.
	HRESULT CreateDeviceResources();  // Initialize device-dependent resources.
	void DiscardDeviceResources();  // Release device-dependent resource.
	HRESULT OnRender();   // Draw content.
  void drawKettle();
	//HRESULT LoadResourceBitmap(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, ID2D1Bitmap** ppBitmap);
	HRESULT LoadResourceBitmap(
		ID2D1RenderTarget *pRenderTarget,
		IWICImagingFactory *pIWICFactory,
		PCWSTR resourceName,
		PCWSTR resourceType,
		UINT destinationWidth,
		UINT destinationHeight,
		ID2D1Bitmap **ppBitmap
	);
	void soundOn(DemoApp* pDemoApp, int i, bool trueorfalse);
	BOOL SetVolume(LPDIRECTSOUNDBUFFER lpDSBuffer, LONG lVolume);

	void OnResize(UINT width, UINT height);  // Resize the render target.

	BOOL addSound(HWND hWnd);
	void WriteActionInfo();			// Write Action Info (mouse cursor position, rectangle rotation & resize, etc...)

	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	);  // The windows procedure.

	BOOL SetPan(LPDIRECTSOUNDBUFFER lpDSBuffer, LONG IPan);

private:
	HWND m_hwnd;

	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;

	// Color Brush
	ID2D1SolidColorBrush* m_pSceneBrush;
	ID2D1SolidColorBrush* fly_path_red_Brush;
	ID2D1SolidColorBrush* m_pRedBrush;
	ID2D1SolidColorBrush* m_pTextBrush;
	ID2D1StrokeStyle *m_pStrokeStyleDash;

	// DWrite
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	IDWriteTextFormat* m_score_TextFormat;

	// WIC and Bitmaps
	IWICImagingFactory* m_pWICFactory;
	ID2D1Bitmap *m_pBackgroundBitmap;
	ID2D1Bitmap *ramenTypeBitmap[4];
	ID2D1Bitmap *gasrangeBitmap;
	ID2D1Bitmap *firegasrangeBitmap;

	ID2D1Bitmap *nambi_Bitmap;
	ID2D1Bitmap *notcover_nambi_Bitmap;
	ID2D1Bitmap *soupe_nambi_Bitmap;
	ID2D1Bitmap *water_nambi_Bitmap;
	ID2D1Bitmap *ramen_nambi_Bitmap;
	ID2D1Bitmap *burned_nambi_Bitmap;
	ID2D1Bitmap *riped_ramen_nambi_Bitmap;

	ID2D1Bitmap *sonnim_Bitmap;
	ID2D1Bitmap *malpungsun_Bitmap;
	ID2D1Bitmap *fly_Bitmap;
	ID2D1Bitmap *skullBitmap;
	ID2D1Bitmap *mouseJailBitmap;

	// Bitmap brushes
	ID2D1BitmapBrush *m_pBackgroundBitmapBrush;
	ID2D1BitmapBrush *ramenTypeBrush[4];
	ID2D1RadialGradientBrush *ramenTypeOpacityBrush[4];
	ID2D1BitmapBrush *gasrangeBitmapBrush;
	ID2D1BitmapBrush *firegasrangeBitmapBrush;
	ID2D1BitmapBrush *skullBitmapBrush;

	ID2D1BitmapBrush *nambi_BitmapBrush;
	ID2D1BitmapBrush *notcover_nambi_BitmapBrush;
	ID2D1BitmapBrush *soupe_nambi_BitmapBrush;
	ID2D1BitmapBrush *water_nambi_BitmapBrush;
	ID2D1BitmapBrush *ramen_nambi_BitmapBrush;
	ID2D1BitmapBrush *burned_nambi_BitmapBrush;
	ID2D1BitmapBrush *riped_ramen_nambi_BitmapBrush;

	ID2D1BitmapBrush *sonnim_BitmapBrush;
	ID2D1BitmapBrush *malpungsun_BitmapBrush;
	ID2D1BitmapBrush *fly_BitmapBrush;

	ID2D1BitmapBrush *mouseJailBitmapBrush;

	// Geometry
	ID2D1PathGeometry *fly_PathGeometry;
	ID2D1PathGeometry *sonnim_PathGeometry;
	ID2D1PathGeometry *kettleGeometry;

	ID2D1RectangleGeometry* m_pRectGeo;

	// ai Geometry
	ID2D1PathGeometry* pViewRange[5];
	ID2D1PathGeometry* pAttackRange[5];
	
	// ai
	void setRandomSkullPosition();
	void drawAICharacter(bool isAiShow);
	
	// Animation
	LinearAnimation<float> m_Animation;
	LinearAnimation<float> m_Animation2;

	DWM_TIMING_INFO fly_DwmTimingInfo;
	DWM_TIMING_INFO m_Character1_DwmTimingInfo;

	// sound 변수
	LPDIRECTSOUNDBUFFER g_lpDSBG[2] = { NULL };
	CSoundManager* soundManager;
	std::vector<Character*> character;
	Character* myCharacter;



};

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER              // Allow use of features specific to Windows XP or later.
#define WINVER 0x0501       // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501 // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS      // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE           // Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600    // Change this to the appropriate value to target other versions of IE.
#endif

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <string>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dwmapi.h>

#include "person.h"
#include "Animation.h"
/******************************************************************
*                                                                 *
*  Macros                                                         *
*                                                                 *
******************************************************************/

template<class Interface>
inline void
SafeRelease(
	Interface **ppInterfaceToRelease
)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

/******************************************************************
*                                                                 *
*  DemoApp                                                        *
*                                                                 *
******************************************************************/
class DemoApp
{
public:
	DemoApp();
	~DemoApp();

	// Register the window class and call methods for instantiating drawing resources
	HRESULT Initialize();

	// Process and dispatch messages
	void RunMessageLoop();

private:

	// Initialize device-independent resources
	HRESULT CreateDeviceIndependentResources();

	// Initialize device-dependent resources
	HRESULT CreateDeviceResources();

	HRESULT LoadResourceBitmap(
		ID2D1RenderTarget *pRT,
		IWICImagingFactory *pIWICFactory,
		PCWSTR resourceName,
		PCWSTR resourceType,
		ID2D1Bitmap **ppBitmap
	);

	// Release device-dependent resources
	void DiscardDeviceResources();

	// Draw content
	HRESULT OnRender();

	// Resize the render target
	void OnResize(
		UINT width,
		UINT height
	);

	// The windows procedure
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	);

private:
	HWND m_hwnd;
	ID2D1Factory *m_pD2DFactory;
	ID2D1HwndRenderTarget *m_pRT;
	ID2D1StrokeStyle *m_pStrokeStyleDash;

	//brush
	ID2D1BitmapBrush *m_pSkyBitmapBrush;
	ID2D1BitmapBrush *m_pGroundBitmapBrush;
	ID2D1SolidColorBrush *m_pTextBrush;
	ID2D1SolidColorBrush *m_pBlackBrush;
	ID2D1SolidColorBrush *m_pPathBrush;
	ID2D1BitmapBrush *m_pCharacterBitmapBrush1;
	ID2D1BitmapBrush *m_pCharacterBitmapBrush2;
	ID2D1BitmapBrush *m_pMissileBitmapBrush;
	ID2D1BitmapBrush *m_pExplosionBitmapBrush;

	//bitmap
	IWICImagingFactory *m_pWICFactory;
	ID2D1Bitmap *m_pCharacterBitmap1;
	ID2D1Bitmap *m_pCharacterBitmap2;
	ID2D1Bitmap *m_pSkyBitmap;
	ID2D1Bitmap *m_pMissileBitmap;
	ID2D1Bitmap *m_pGroundBitmap;
	ID2D1Bitmap *m_pExplosionBitmap;

	//pathgeometry
	ID2D1PathGeometry *m_pPathGeometry;
	ID2D1RectangleGeometry *m_pMissileGeometry;
	//animation
	EaseInOutExponentialAnimation<float> m_Animation;

	//DWrite
	IDWriteFactory *m_pDWriteFactory;
	IDWriteTextFormat *m_pTextFormat;

	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;
};
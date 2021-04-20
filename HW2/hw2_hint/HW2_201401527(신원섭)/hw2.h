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
#include "resource.h"

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <stack>
#include <string> // 문자열
#include <iostream>
#include <locale.h>     
#include <vector>
#include <fstream>
#include <windows.h>  // 이 헤더파일은 Win32 API 관련 구조와 함수 선언들을 포함함.
#include <Windowsx.h> // SetDlgMsgResult... window message crackers
#include <functional> //greater
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <cstring>
#include <time.h>

/******************************************************************
*                                                                 *
*  Macros                                                         *
*                                                                 *
******************************************************************/

using namespace std;

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

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
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

    HRESULT CreateGridPatternBrush(
        ID2D1RenderTarget *pRenderTarget,
        ID2D1BitmapBrush **ppBitmapBrush
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

    // Translate
    void MyDrawRect();
	void write();

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
    ID2D1HwndRenderTarget *m_pRenderTarget;
    ID2D1StrokeStyle *m_pStrokeStyleDash;

	ID2D1SolidColorBrush *m_pgrayBrush;
    ID2D1SolidColorBrush *m_pOriginalShapeBrush;
    ID2D1SolidColorBrush *m_pFillBrush;
	ID2D1SolidColorBrush *m_pFillBrush2;
	ID2D1SolidColorBrush *m_pFillBrush11;
	ID2D1SolidColorBrush *m_pFillBrush12;
	ID2D1SolidColorBrush *m_pFillBrush13;
	ID2D1SolidColorBrush *m_pFillBrush14;
	ID2D1SolidColorBrush *m_pFillBrush15;
	ID2D1SolidColorBrush *m_pFillBrush16;
	ID2D1SolidColorBrush *m_pFillBrush17;
	ID2D1SolidColorBrush *m_pFillBrush18;
	ID2D1SolidColorBrush *m_pFillBrush19;
	ID2D1SolidColorBrush *m_pFillBrush20;
	ID2D1SolidColorBrush *m_pTextBrush;
    ID2D1SolidColorBrush *m_pTextBrush2;
    ID2D1BitmapBrush *m_pGridPatternBitmapBrush;

    //DWrite
    IDWriteFactory *m_pDWriteFactory;
    IDWriteTextFormat *m_pTextFormat;
};
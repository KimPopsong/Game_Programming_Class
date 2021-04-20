#pragma once

// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
// DX Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

// 자원 안전 반환 매크로.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// 디버그 메시지를 표시하기 
#if defined(_DEBUG)&&defined(WIN32)&&!defined(_AFX)&&!defined(_AFXDLL)
#define TRACE	TRACE_WIN32
#pragma warning(disable: 4996)
void TRACE_WIN32(LPCTSTR lpszFormat, ...) {
  TCHAR lpszBuffer[0x160]; //buffer size
  va_list fmtList;
  va_start(fmtList, lpszFormat);
  _vstprintf(lpszBuffer, lpszFormat, fmtList);
  va_end(fmtList);
  ::OutputDebugString(lpszBuffer);
}
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
  HRESULT CreateDeviceIndependentResources();  // Initialize device-independent resources
  HRESULT CreateDeviceResources();  // Initialize device-dependent resources
  HRESULT CreateGridPatternBrush(ID2D1RenderTarget *pRenderTarget, ID2D1BitmapBrush **ppBitmapBrush);
  void DiscardDeviceResources();  // Release device-dependent resources
  HRESULT OnRender();  // Draw content
  void OnResize(UINT width, UINT height);  // Resize the render target

  void DrawSmallRectangle();	// Small rectangle
  void DrawVirtualRectangle();	// Virtual rectangle
  void DrawMovingRectangle();	// Moving rectangle
  void DrawRealRectangle();		// Real rectangle
  void WriteActionInfo();			// Write Action Info (mouse cursor position, rectangle rotation & resize, etc...)

  void MakeStudent();			// Make student struct
  void WriteStudentInfo();			// Write Student Info (grade, name) 

  // ClientRect(그리기 영역의 위치와 크기)로부터 i(순서)에 따라 box의 위치와
  // 크기를 결정한다
  void GetRect(D2D1_RECT_F& boxRect, int i) const;

  // x, y 좌표가 rect 영역 안에 위치하는지 확인한다
  bool IsIn(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) const;

  const D2D1_RECT_F& GetSmallRect() const;
  const D2D1_RECT_F& GetVirtualRect() const;
  const D2D1_RECT_F& GetTopRect() const;
  
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);  // The windows procedure

  HWND m_hwnd;
  ID2D1Factory* m_pD2DFactory;
  ID2D1HwndRenderTarget* m_pRenderTarget;

  ID2D1StrokeStyle* m_pStrokeStyleDash;

  ID2D1SolidColorBrush* m_pBlackShapeBrush;
  ID2D1SolidColorBrush* m_pDarkBlueShapeBrush;

  ID2D1SolidColorBrush* m_pFillLightGrayBrush;
  ID2D1SolidColorBrush* m_pFillGrayBrush;
  ID2D1SolidColorBrush* m_pFilllRainbowBrush[10];

  ID2D1SolidColorBrush* m_pTextBrush;
  ID2D1BitmapBrush* m_pGridPatternBitmapBrush;

  //DWrite
  IDWriteFactory* m_pDWriteFactory;
  IDWriteTextFormat* m_pTextFormat;

  D2D1_RECT_F m_smallRect;
};

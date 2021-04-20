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
#include <iostream>
#include <vector>
#include <time.h>
#include <random>

// 자원 안전 반환 매크로.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// DemoApp class declaration

typedef struct {
	int grade;		//성적
	std::string name;	//이름
} Student;

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
	HRESULT CreateGridPatternBrush(ID2D1RenderTarget *pRenderTarget, ID2D1BitmapBrush **ppBitmapBrush);
	void DiscardDeviceResources();  
	HRESULT OnRender();  
	void OnResize(UINT width, UINT height);
	void stackBox();
	void dragBox();
	void showBoxInfo();
	void StudentInfo();
	void predictBox();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);  // The windows procedure.

private:
	HWND m_hwnd;
	ID2D1Factory* m_pD2DFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1StrokeStyle* m_pStrokeStyleDash;

	ID2D1SolidColorBrush* m_pOriginalShapeBrush;
	ID2D1SolidColorBrush* m_pTransformedShapeBrush;
	ID2D1SolidColorBrush* m_pTransformedShapeBrush1;
	ID2D1SolidColorBrush* m_pTransformedShapeBrush2;

	ID2D1SolidColorBrush* m_pFillBrush;
	ID2D1SolidColorBrush* m_pFillBrush1;
	ID2D1SolidColorBrush* m_pFillBrush2;
	ID2D1SolidColorBrush* m_pTextBrush;
	ID2D1BitmapBrush* m_pGridPatternBitmapBrush;

	ID2D1BitmapBrush* m_bitmapBrush;

	// DirectWrite
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
};

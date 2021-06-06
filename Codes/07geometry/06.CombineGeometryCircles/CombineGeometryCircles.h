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
	HRESULT CreateDeviceResources();
	HRESULT CreateGeometryResources();
	HRESULT CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, ID2D1BitmapBrush** ppBitmapBrush);
	void DiscardDeviceResources();
	void RenderCombinedGeometry();
	void UseGeometryFunctions();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hwnd;

	ID2D1Factory* m_pD2DFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pOutlineBrush;
	ID2D1SolidColorBrush* m_pTextBrush;
	ID2D1SolidColorBrush* m_pShapeFillBrush;
	ID2D1BitmapBrush* m_pGridPatternBitmapBrush;

	ID2D1EllipseGeometry* m_pCircleGeometry1;
	ID2D1EllipseGeometry* m_pCircleGeometry2;

	ID2D1PathGeometry* m_pPathGeometryUnion;
	ID2D1PathGeometry* m_pPathGeometryIntersect;
	ID2D1PathGeometry* m_pPathGeometryXOR;
	ID2D1PathGeometry* m_pPathGeometryExclude;

	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;

	ID2D1StrokeStyle* m_pStrokeStyle;
};

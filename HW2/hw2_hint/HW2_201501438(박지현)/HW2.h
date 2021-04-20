#pragma once
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
// DX Header Files:
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <string>
// �ڿ� ���� ��ȯ ��ũ��.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

using namespace std;

class student
{
public:
	string name;
	int score;
	student(string name, int score) {
		this->name = name;
		this->score = score;
	}
};

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
	HRESULT CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, ID2D1BitmapBrush** ppBitmapBrush);
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);

	/*void DrawAndRotateARectangle();
	void DrawAndScaleARectangle();*/
	void drawInit();
	void drawTempInsertingRectangle();
	void drawTempdeletingRectangle();
	//void drawRectangle();
	//void DrawAndSkewARectangle();
	//void DrawAndTranslateARectangle();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hwnd;
	ID2D1Factory* m_pD2DFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1StrokeStyle* m_pStrokeStyleDash;

	ID2D1SolidColorBrush* m_pOriginalShapeBrush;
	ID2D1SolidColorBrush* m_boundaryBrush;
	ID2D1SolidColorBrush* m_RectangleBrush;
	ID2D1SolidColorBrush* m_pFillBrush;
	ID2D1SolidColorBrush* m_pTextBrush;
	ID2D1BitmapBrush* m_pGridPatternBitmapBrush;

	//DWrite
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
};

string randumsung[10] = {
	"��", "��", "��", "��", "��", "��", "��", "��", "��", "��"
};

string randname[24] = {
	"��","��","��","��","��","��","��","��","ö","��","��","��","��","��","��",
	"ȣ","��","��","��","��","��","��","��","��"
};
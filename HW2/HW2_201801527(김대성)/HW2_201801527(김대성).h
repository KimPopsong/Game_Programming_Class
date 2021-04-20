#pragma once

#include "resource.h"

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <vector>  // For vector
#include <algorithm>  // sort 함수
#include <time.h>  // srand

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

typedef struct {
	WCHAR name[4];
	int score;
}Student;  // 학생의 정보

typedef struct {
	int left;
	int right;
	int top;
	int bottom;
}VirtualBox;

BOOL makeBoxFlag = false;
BOOL delBoxFlag = false;
BOOL mouseClick = false;
FLOAT info_s, info_r;
D2D_POINT_2F mousePosition;
int studentNumber;
Student student;

const WCHAR lastName[] = L"김나박이";
const WCHAR firstName[] = L"대성수연정민수한종혁석준식";

std::vector<Student> studentVector;
VirtualBox topBox, bottomBox, studentBox;

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

class DemoApp
{
private:  // 변수
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;

	ID2D1SolidColorBrush* m_pTransformedShapeBrush;
	ID2D1SolidColorBrush* m_pFillBrush[10];
	ID2D1SolidColorBrush* m_pTextBrush;
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;

public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize(HINSTANCE hInstance); // 윈도우 클래스를 등록, 그리기 자원들을 생성하는 함수들을 호출
	void RunMessageLoop(); // 메시지를 처리

private:
	HRESULT CreateDeviceIndependentResources(); // 장치 독립 자원들을 초기화
	HRESULT CreateDeviceResources(); // 장치 의존 자원들을 초기화
	void DiscardDeviceResources(); // 장치 의존 자원들을 반납
	HRESULT OnRender(); // 내용을 그리기
	void OnResize(UINT width, UINT height); // 렌더타겟을 resize
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);  // 윈도우 프로시져
	D2D1_RECT_F MakeChasingRect(D2D_POINT_2F mousePos, FLOAT h);
};
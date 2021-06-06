#include "d2dHelper.h"

ID2D1Factory* pFactory;
ID2D1HwndRenderTarget* pRenderTarget;
ID2D1SolidColorBrush* pRedBrush;
ID2D1SolidColorBrush* pBlackBrush;
ID2D1SolidColorBrush* pGreenBrush;
ID2D1SolidColorBrush* pBlueBrush;
ID2D1SolidColorBrush* pSolidColorBrush = NULL; //할당하지 않음
ID2D1PathGeometry* pCharacter;
IDWriteFactory* pWriteFactory;
IDWriteTextFormat* pTextFormat;

ID2D1PathGeometry* pEnemy[5];
ID2D1PathGeometry* pViewRange[5];
ID2D1PathGeometry* pAttackRange[5];

HWND d2d::initWin32(HINSTANCE hInstance, int width, int height)
{
	HWND hWnd; //반환할 윈도우 핸들.
	WNDCLASSEX wc; //윈도우 클래스 객체.
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	// 윈도우 클래스 속성을 초기화함.
	wc.cbSize = sizeof(WNDCLASSEX); //WNDCLASSEX 구조체의 크기.
	wc.style = CS_HREDRAW | CS_VREDRAW; //윈도우 스타일.
	wc.lpfnWndProc = (WNDPROC)WndProc; //윈도우 프로시져 주소.
	wc.cbClsExtra = 0; //클래스 추가 정보.
	wc.cbWndExtra = 0; //윈도우 추가 정보.
	wc.hInstance = hInstance; //응용 인스턴스의 핸들.
	wc.hIcon = ::LoadIcon(NULL, IDI_APPLICATION); //윈도우 아이콘.
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW); //마우스 커서.
	wc.hbrBackground = NULL; //배경 색.
	wc.lpszMenuName = NULL; //메뉴.
	wc.lpszClassName = L"DemoApp"; //윈도우 클래스 이름.
	wc.hIconSm = ::LoadIcon(NULL, IDI_APPLICATION); //최소화된 윈도우 아이콘.

	// 윈도우 클래스를 등록함.
	if (! ::RegisterClassEx(&wc)) {
		::MessageBox(NULL, L"RegisterClassEx() - Failed", L"ERROR", NULL);
		return false;
	}

	// 창의 크기를 조절함.
	RECT screenrect;
	DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	::SetRect(&screenrect, 0, 0, width, height);
	::AdjustWindowRect(&screenrect, dwWindowStyle, false);
	
	// 새 창을 생성함.
	hWnd = ::CreateWindowEx(NULL, //확장된 스타일.
		L"DemoApp", //윈도우 클래스.
		L"DemoApp", //윈도우 이름.
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, //윈도우 스타일.
		CW_USEDEFAULT, CW_USEDEFAULT, //X,Y좌표.
		screenrect.right - screenrect.left, screenrect.bottom - screenrect.top, //윈도우 크기.
		NULL, //부모 윈도우의 핸들.
		NULL, //메뉴.
		hInstance, //응용 인스턴스의 핸들.
		NULL); //윈도우의 포인터.
	if (! hWnd) {
		::MessageBox(NULL, L"CreateWindowEx() - Failed", L"ERROR", NULL);
		return 0;
	}

	::ShowWindow(hWnd, SW_SHOW);
	::UpdateWindow(hWnd);

	return hWnd;
}

bool d2d::releaseWin32(HINSTANCE hInstance)
{
	// 윈도우 클래스를 unregister한다.
	::UnregisterClass(L"DemoApp", hInstance);
	return true;
}

int d2d::doMessageLoop(bool (*renderFunction)(float timeDelta))
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)::timeGetTime(); //winmm.lib

	while (msg.message != WM_QUIT) {
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else {
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;

			if (! renderFunction(timeDelta)) 
			{
				return 0;
			}

			lastTime = currTime;
		}
	}
	return (int)(msg.wParam);
}

HRESULT d2d::CreateDeviceIndependentResources()
{
	HRESULT hr;
	ID2D1GeometrySink* pSink = NULL;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,&pFactory);
	if(SUCCEEDED(hr))
	{
		hr = pFactory->CreatePathGeometry(&pCharacter);
	}

	if(SUCCEEDED(hr))
	{
		hr = pCharacter->Open(&pSink);
	}
	if(SUCCEEDED(hr))
	{
		D2D1_POINT_2F StartPos = D2D1::Point2F(-10,0);
		pSink->BeginFigure(StartPos,D2D1_FIGURE_BEGIN_FILLED);

		pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(10,0),D2D1::SizeF(10,10),0,D2D1_SWEEP_DIRECTION_CLOCKWISE,D2D1_ARC_SIZE_SMALL));
		pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(-10,0),D2D1::SizeF(10,10),0,D2D1_SWEEP_DIRECTION_CLOCKWISE,D2D1_ARC_SIZE_SMALL));

		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		hr = pSink->Close();
	}
	SAFE_RELEASE(pSink);

	if(SUCCEEDED(hr))
	{
		for(int i = 0; i < 5; i++)
		{
			hr = pFactory->CreatePathGeometry(&pEnemy[i]);
			hr = pFactory->CreatePathGeometry(&pViewRange[i]);
			hr = pFactory->CreatePathGeometry(&pAttackRange[i]);
			//Enemy
			if(SUCCEEDED(hr))
			{
				hr = pEnemy[i]->Open(&pSink);
			}
			if(SUCCEEDED(hr))
			{
				D2D1_POINT_2F StartPos = D2D1::Point2F(-10,0);
				pSink->BeginFigure(StartPos,D2D1_FIGURE_BEGIN_FILLED);

				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(10,0),D2D1::SizeF(10,10),0,D2D1_SWEEP_DIRECTION_CLOCKWISE,D2D1_ARC_SIZE_SMALL));
				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(-10,0),D2D1::SizeF(10,10),0,D2D1_SWEEP_DIRECTION_CLOCKWISE,D2D1_ARC_SIZE_SMALL));

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSink->Close();
			}
			SAFE_RELEASE(pSink);
			//ViewRange
			if(SUCCEEDED(hr))
			{
				hr = pViewRange[i]->Open(&pSink);
			}
			if(SUCCEEDED(hr))
			{
				D2D1_POINT_2F StartPos = D2D1::Point2F(-90,0);
				pSink->BeginFigure(StartPos,D2D1_FIGURE_BEGIN_FILLED);

				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(90,0),D2D1::SizeF(90,90),0,D2D1_SWEEP_DIRECTION_CLOCKWISE,D2D1_ARC_SIZE_SMALL));
				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(-90,0),D2D1::SizeF(90,90),0,D2D1_SWEEP_DIRECTION_CLOCKWISE,D2D1_ARC_SIZE_SMALL));

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSink->Close();
			}
			SAFE_RELEASE(pSink);
			//AttackRange
			if(SUCCEEDED(hr))
			{
				hr = pAttackRange[i]->Open(&pSink);
			}
			if(SUCCEEDED(hr))
			{
				D2D1_POINT_2F StartPos = D2D1::Point2F(-30,0);
				pSink->BeginFigure(StartPos,D2D1_FIGURE_BEGIN_FILLED);

				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(30,0),D2D1::SizeF(30,30),0,D2D1_SWEEP_DIRECTION_CLOCKWISE,D2D1_ARC_SIZE_SMALL));
				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(-30,0),D2D1::SizeF(30,30),0,D2D1_SWEEP_DIRECTION_CLOCKWISE,D2D1_ARC_SIZE_SMALL));

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSink->Close();
			}
			SAFE_RELEASE(pSink);
		}
	}
	
	if(SUCCEEDED(hr))
	{
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(pWriteFactory),(IUnknown**)&pWriteFactory);
	}
	if(SUCCEEDED(hr))
	{
		pWriteFactory->CreateTextFormat(L"궁서",0,DWRITE_FONT_WEIGHT_REGULAR,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,11,L"ko-KR",&pTextFormat);
	}
	return hr;
}

void d2d::DiscardDeviceIndependentResources()
{
	SAFE_RELEASE(pFactory);
	SAFE_RELEASE(pCharacter);
	for (int i = 0; i < 5; i++){
		SAFE_RELEASE(pEnemy[i]);
		SAFE_RELEASE(pViewRange[i]);
		SAFE_RELEASE(pAttackRange[i]);
	}
	SAFE_RELEASE(pWriteFactory);
	SAFE_RELEASE(pTextFormat);
}

HRESULT d2d::CreateDeviceResources(HWND hWnd, D2D1_SIZE_U size)
{
	HRESULT hr = S_OK;
	if(!pRenderTarget)
	{
		hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),D2D1::HwndRenderTargetProperties(hWnd,size),&pRenderTarget);
		if(SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red),&pRedBrush);
		}
		if(SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue),&pBlueBrush);
		}
		if(SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),&pBlackBrush);
		}	
		if(SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGreen),&pGreenBrush);
		}
	}
	return hr;
}

void d2d::DiscardDeviceResources()
{
	SAFE_RELEASE(pRedBrush);
	SAFE_RELEASE(pBlueBrush);
	SAFE_RELEASE(pBlackBrush);
	SAFE_RELEASE(pGreenBrush);
	//주의: pSolidColorBrush는 반납하지 말것!
	SAFE_RELEASE(pRenderTarget);
}

#include <stdio.h> //_vstprintf_s
#include <tchar.h> //_vstprintf_s

#if defined(TRACE_WIN32_REQUIRE_IMPL)
void TRACE_WIN32(LPCTSTR lpszFormat, ...)
{
	TCHAR lpszBuffer[0x160]; //버퍼 크기.
	va_list fmtList;
	va_start(fmtList, lpszFormat);
	_vstprintf_s(lpszBuffer, lpszFormat, fmtList);
	va_end(fmtList);
	::OutputDebugString(lpszBuffer);
}
#undef TRACE_WIN32_REQUIRE_IMPL
#endif

//@Author: Jong-Seung Park.


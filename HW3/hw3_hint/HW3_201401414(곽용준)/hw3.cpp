// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "hw3.h"
#define PI 3.1415926535897
/*현재 마우스 확인 포인트*/
D2D_POINT_2F current_p;
D2D_POINT_2F click_p;
//플레이어 위치
D2D_POINT_2F p1_location;
D2D_POINT_2F p2_location;
//미사일 발사 포인트
D2D_POINT_2F BezierEnd;
D2D_POINT_2F MissileEnd;
D2D_POINT_2F point1;
D2D_POINT_2F point2;

/*발사 각, 힘, 발사 시간*/
FLOAT Angle = 0.0f;
FLOAT Power = 0.0f;
FLOAT Time = 0.0f;
DWORD dwStart = 0;

D2D1_SIZE_F rtSize;
UINT winWidth;
UINT winHeight;

FLOAT x_rate = 0.0f;
FLOAT y_rate = 0.0f;


//person 클래스 구현
Person::Person(int health) {
	this->health = health;
}

void Person::damaged(int damage) {
	this->health -= damage;
}

//메서드 구현
//라디안 값 변환 메서드
FLOAT getRadian(FLOAT num) {
	return num * (PI / 180);
}

// 화면보기/끄기와 시작/정지 flag
WCHAR input_str;
bool isShow = false;
bool isStarted = true;
bool end = false;

//미사일 발사 flag
bool isShoot = false;


int WINAPI WinMain(
	HINSTANCE /* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */
)
{
	// Ignoring the return value because we want to continue running even in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;

			if (SUCCEEDED(app.Initialize()))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}

	return 0;
}

/******************************************************************
*                                                                 *
*  DemoApp::DemoApp constructor                                   *
*                                                                 *
*  Initialize member data                                         *
*                                                                 *
******************************************************************/

DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pD2DFactory(NULL),
	m_pRT(NULL),
	m_pPathGeometry(NULL),
	m_pMissileGeometry(NULL),
	m_pTextBrush(NULL),
	m_pBlackBrush(NULL),
	m_pPathBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL),
	m_pWICFactory(NULL),
	m_pSkyBitmap(NULL),
	m_pCharacterBitmap1(NULL),
	m_pCharacterBitmap2(NULL),
	m_pMissileBitmap(NULL),
	m_pGroundBitmap(NULL),
	m_pExplosionBitmap(NULL),
	m_pSkyBitmapBrush(NULL),
	m_pCharacterBitmapBrush1(NULL),
	m_pCharacterBitmapBrush2(NULL),
	m_pMissileBitmapBrush(NULL),
	m_pGroundBitmapBrush(NULL),
	m_pExplosionBitmapBrush(NULL),
	m_Animation(0,0,0)
{
}

/******************************************************************
*                                                                 *
*  DemoApp::~DemoApp destructor                                   *
*                                                                 *
*  Release resources.                                             *
*                                                                 *
******************************************************************/

DemoApp::~DemoApp()
{
	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pRT);
	SafeRelease(&m_pPathGeometry);
	SafeRelease(&m_pMissileGeometry);
	
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pTextFormat);
	SafeRelease(&m_pWICFactory);

	SafeRelease(&m_pSkyBitmap);
	SafeRelease(&m_pGroundBitmap);
	SafeRelease(&m_pMissileBitmap);
	SafeRelease(&m_pCharacterBitmap1);
	SafeRelease(&m_pCharacterBitmap2);
	SafeRelease(&m_pExplosionBitmap);

	SafeRelease(&m_pTextBrush);
	SafeRelease(&m_pBlackBrush);
	SafeRelease(&m_pPathBrush);
	SafeRelease(&m_pSkyBitmapBrush);
	SafeRelease(&m_pGroundBitmapBrush);
	SafeRelease(&m_pCharacterBitmapBrush1);
	SafeRelease(&m_pCharacterBitmapBrush2);
	SafeRelease(&m_pMissileBitmapBrush);
	SafeRelease(&m_pExplosionBitmapBrush);
}

/******************************************************************
*                                                                 *
*  DemoApp::Initialize                                            *
*                                                                 *
*  Create application window and device-independent resources     *
*                                                                 *
******************************************************************/

HRESULT DemoApp::Initialize()
{
	HRESULT hr;

	//register window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = DemoApp::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"HW3_201401414(곽용준)";

	RegisterClassEx(&wcex);

	p1_location.x = 90;
	p1_location.y = 490;
	
	p2_location.x = 790;
	p2_location.y = 460;

	point1.x = 150;
	point1.y = 50;
	point2.x = 650;
	point2.y = 50;

	BezierEnd.x = p2_location.x;
	BezierEnd.y = p2_location.y + 150;

	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		// Create the application window.
		m_hwnd = CreateWindow(
			L"HW3_201401414(곽용준)",
			L"HW3_201401414(곽용준)",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			1000,
			700,
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
		);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			float length = 0;
			hr = m_pPathGeometry->ComputeLength(
				NULL, //no transform
				&length
			);
			if (SUCCEEDED(hr))
			{
				m_Animation.SetStart(0);        //start at beginning of path
				m_Animation.SetEnd(length);     //length at end of path
				m_Animation.SetDuration(10.0f);  //seconds

				ShowWindow(m_hwnd, SW_SHOWNORMAL);

				UpdateWindow(m_hwnd);
			}
		}
		QueryPerformanceFrequency(&m_nFrequency);
		QueryPerformanceCounter(&m_nPrevTime);
	}
	return hr;
}



/******************************************************************
*                                                                 *
*  DemoApp::CreateDeviceIndependentResources                      *
*                                                                 *
*  This method is used to create resources which are not bound    *
*  to any device. Their lifetime effectively extends for the      *
*  duration of the app.                                           *
*                                                                 *
******************************************************************/

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr;
	ID2D1GeometrySink *pSink = NULL;



	// Create a WIC factory.
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pWICFactory)
	);
	// Create a Direct2D factory.
	if (SUCCEEDED(hr))
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr))
	{
		// Create a shared DirectWrite factory
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
		);
	}

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",     // The font family name.
			NULL,           // The font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			13.0f,
			L"en-us",
			&m_pTextFormat
		);
	}

	/* 미사일 경로 기하 */
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pPathGeometry->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(p1_location, D2D1_FIGURE_BEGIN_FILLED);
		pSink->AddBezier(D2D1::BezierSegment(point1, point2, BezierEnd));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}

	

	return hr;
}

/******************************************************************
*                                                                 *
*  DemoApp::CreateDeviceResources                                 *
*                                                                 *
*  This method creates resources which are bound to a particular  *
*  D3D device. It's all centralized here, in case the resources   *
*  need to be recreated in case of D3D device loss (eg. display   *
*  change, remoting, removal of video card, etc).                 *
*                                                                 *
******************************************************************/

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;
	HRSRC imageResHandle = NULL;

	if (!m_pRT)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
		);

		// Create a Direct2D render target
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRT
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pBlackBrush
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pTextBrush
			);
		}

		/* 배경 및 물체의 비트맵생성 */
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"Sky",
				L"Image",
				&m_pSkyBitmap
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"PackMan",
				L"Image",
				&m_pCharacterBitmap1
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"Penguin",
				L"Image",
				&m_pCharacterBitmap2
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"Ground",
				L"Image",
				&m_pGroundBitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"Missile",
				L"Image",
				&m_pMissileBitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"Explosion",
				L"Image",
				&m_pExplosionBitmap
			);
		}

		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
			);

			hr = m_pRT->CreateBitmapBrush(
				m_pSkyBitmap,
				propertiesXClampYClamp,
				&m_pSkyBitmapBrush
			);

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateBitmapBrush(
					m_pCharacterBitmap1,
					propertiesXClampYClamp,
					&m_pCharacterBitmapBrush1
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateBitmapBrush(
					m_pCharacterBitmap2,
					propertiesXClampYClamp,
					&m_pCharacterBitmapBrush2
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateBitmapBrush(
					m_pGroundBitmap,
					propertiesXClampYClamp,
					&m_pGroundBitmapBrush
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateBitmapBrush(
					m_pMissileBitmap,
					propertiesXClampYClamp,
					&m_pMissileBitmapBrush
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateBitmapBrush(
					m_pExplosionBitmap,
					propertiesXClampYClamp,
					&m_pExplosionBitmapBrush
				);
			}
		}
	}
	return hr;
}

/******************************************************************
*                                                                 *
*  DemoApp::DiscardDeviceResources                                *
*                                                                 *
*  Discard device-specific resources which need to be recreated   *
*  when a D3D device is lost                                      *
*                                                                 *
******************************************************************/

void DemoApp::DiscardDeviceResources()
{
	SafeRelease(&m_pRT);
	SafeRelease(&m_pBlackBrush);
	SafeRelease(&m_pTextBrush);

	SafeRelease(&m_pSkyBitmap);
	SafeRelease(&m_pCharacterBitmap1);
	SafeRelease(&m_pCharacterBitmap2);
	SafeRelease(&m_pGroundBitmap);
	SafeRelease(&m_pMissileBitmap);
	SafeRelease(&m_pExplosionBitmap);

	SafeRelease(&m_pSkyBitmapBrush);
	SafeRelease(&m_pCharacterBitmapBrush1);
	SafeRelease(&m_pCharacterBitmapBrush2);
	SafeRelease(&m_pGroundBitmapBrush);
	SafeRelease(&m_pMissileBitmapBrush);
	SafeRelease(&m_pExplosionBitmapBrush);
}

/******************************************************************
*                                                                 *
*  DemoApp::RunMessageLoop                                        *
*                                                                 *
*  Main window message loop                                       *
*                                                                 *
******************************************************************/

void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/******************************************************************
*                                                                 *
*  DemoApp::OnRender                                              *
*                                                                 *
*  Called whenever the application needs to display the client    *
*  window. This method draws a single frame of animated content   *
*                                                                 *
*  Note that this function will automatically discard device-     *
*  specific resources if the D3D device disappears during         *
*  function invocation, and will recreate the resources the next  *
*  time it's invoked.                                             *
*                                                                 *
******************************************************************/

HRESULT DemoApp::OnRender()
{
	HRESULT hr;

	hr = CreateDeviceResources();
	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F point, point2, point3, point4;
		D2D1_POINT_2F tangent, tangent2, tangent3, tangent4;

		D2D1_MATRIX_3X2_F triangleMatrix;
		D2D1_SIZE_F rtSize = m_pRT->GetSize();
		float minWidthHeightScale = min(rtSize.width, rtSize.height) / 512;

		D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(
			minWidthHeightScale,
			minWidthHeightScale
		);

		D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(
			rtSize.width / 16 - 150,
			rtSize.height / 16 - 150
		);

		// Prepare to draw.
		m_pRT->BeginDraw();

		// Reset to identity transform D2D1::Matrix3x2F::Scale(0.5, 0.5)
		m_pRT->SetTransform(D2D1::Matrix3x2F::Scale(1, 1));
		m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		/* 비트맵배경그리기 */
		D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);


		//뒷 배경 그리기 (MIRROR,WARP설정)
		m_pSkyBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
		m_pSkyBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);
		m_pRT->FillRectangle(&rcBrushRect, m_pSkyBitmapBrush);

		//바닥 (땅)  그리기(CLAMP,WRAP설정)
		rcBrushRect = D2D1::RectF(0, rtSize.height / 2 + 280, rtSize.width, rtSize.height);
		m_pGroundBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
		m_pGroundBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);
		m_pRT->FillRectangle(&rcBrushRect, m_pGroundBitmapBrush);

		//1p 플레이어 그리기
		D2D1_MATRIX_3X2_F policetran = D2D1::Matrix3x2F::Translation(p1_location.x, p1_location.y);
		m_pRT->SetTransform(policetran);
		D2D1_SIZE_F characterSize = m_pCharacterBitmap1->GetSize();
		D2D1_RECT_F rcBrushRect2 = D2D1::RectF(0, 0, characterSize.width, characterSize.height);

		m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pRT->FillOpacityMask(
			m_pCharacterBitmap1,
			m_pCharacterBitmapBrush1,
			D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
			&rcBrushRect2,
			NULL
		);
		m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		//2p 플레이어 그리기
		D2D1_MATRIX_3X2_F Enemytran = D2D1::Matrix3x2F::Translation(p2_location.x, p2_location.y);
		m_pRT->SetTransform(Enemytran);
		D2D1_SIZE_F enemySize = m_pCharacterBitmap2->GetSize();
		D2D1_RECT_F rcBrushRect3 = D2D1::RectF(0, 0, enemySize.width, enemySize.height);

		m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pRT->FillOpacityMask(
			m_pCharacterBitmap2,
			m_pCharacterBitmapBrush2,
			D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
			&rcBrushRect3,
			NULL
		);
		m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());
		WCHAR szText[100];
		swprintf_s(szText, L"클릭x좌표:%f\n클릭y좌표:%f\nPower:%f\nAngle:%f\n",
			click_p.x, click_p.y, Power,Angle);
		m_pTextBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1));
		/* 텍스트 출력 */
		m_pRT->DrawText(
			szText,
			wcslen(szText),
			m_pTextFormat,
			D2D1::RectF(20.0f, rtSize.height - 120, 200.0f, rtSize.height),
			m_pTextBrush
		);

		//미사일 궤적 그리기
		if (isShow) {
			Angle = atan2(p1_location.y - click_p.y, click_p.x - p1_location.x) * 180 / PI;
			FLOAT middle = 0.0f;
			Time = Power * sin(getRadian(Angle));
			MissileEnd.x = (Power * cos(getRadian(Angle)) * Time) + p1_location.x;
			MissileEnd.y = rtSize.height / 2 + 230;

			if (Angle == 0) {
				y_rate = 0;
			}
			else {
				y_rate = Angle / 45;
			}
			x_rate = MissileEnd.x / p2_location.x;

			D2D1::Matrix3x2F missileScale = D2D1::Matrix3x2F::Scale(D2D1::Size(x_rate, y_rate), p1_location);
			m_pRT->SetTransform(missileScale);
			m_pRT->DrawGeometry(m_pPathGeometry, m_pBlackBrush);
			static float anim_time = 0.0f;
			static float elapsedTime = 0.0;
			float length = m_Animation.GetValue(anim_time);

			m_pPathGeometry->ComputePointAtLength(length, NULL, &point, &tangent);

			//미사일 크기 구하기
			D2D1_SIZE_F	missileSize = m_pMissileBitmap->GetSize();
			D2D1_RECT_F rcMissileRect = D2D1::RectF(0, 0, missileSize.width, missileSize.height);
			// 미사일의 방향을 조절하여 이동 동선을 따라가는 방향이 되도록 함.
			triangleMatrix = D2D1::Matrix3x2F(
				tangent.x,tangent.y,
				-tangent.y,tangent.x,
				point.x, point.y);
			
			//m_pRT->SetTransform(triangleMatrix * scale * translation);
			m_pRT->SetTransform(triangleMatrix * missileScale);
			
			// 미사일 그리기
			m_pRT->FillRectangle(rcMissileRect, m_pMissileBitmapBrush);
			if (isShoot) {
				anim_time = 0;
				isShoot = false;
			}
				
			// 애니메이션의 끝에 도달하면 폭발이미지 생성
			if (anim_time >= m_Animation.GetDuration())
			{
				D2D1_SIZE_F	explosionSize = m_pExplosionBitmap->GetSize();
				D2D1_RECT_F rcExplosionRect = D2D1::RectF(0, 0, explosionSize.width, explosionSize.height);
				D2D1_MATRIX_3X2_F explosiontran = D2D1::Matrix3x2F::Translation(MissileEnd.x - explosionSize.width/2,MissileEnd.y);
				m_pRT->SetTransform(explosiontran);

				m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
				m_pRT->FillOpacityMask(
					m_pExplosionBitmap,
					m_pExplosionBitmapBrush,
					D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
					&rcExplosionRect,
					NULL
				);
				m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
				
				
			}
			else
			{
				LARGE_INTEGER CurrentTime;
				QueryPerformanceCounter(&CurrentTime);

				float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
				m_nPrevTime = CurrentTime;

				anim_time += elapsedTime;
			}
		}
			
		// Commit the drawing operations.
		hr = m_pRT->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}
	}

	return hr;
}

/******************************************************************
*                                                                 *
*  DemoApp::OnResize                                              *
*                                                                 *
*  If the application receives a WM_SIZE message, this method     *
*  resize the render target appropriately.                        *
*                                                                 *
******************************************************************/

void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRT)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;

		// Note: This method can fail, but it's okay to ignore the
		// error here -- it will be repeated on the next call to
		// EndDraw.
		m_pRT->Resize(size);
	}
}

/******************************************************************
*                                                                 *
*  DemoApp::WndProc                                               *
*                                                                 *
*  Window message handler                                         *
*                                                                 *
******************************************************************/

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pDemoApp)
		);

		result = 1;
	}
	else
	{
		DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
			)));

		bool wasHandled = false;

		if (pDemoApp)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pDemoApp->OnResize(width, height);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_PAINT:
			{
				pDemoApp->OnRender();

				// Do not call ValidateRect so that the window will be redrawn.
			} 
			result = 0;
			wasHandled = true;
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			result = 1;
			wasHandled = true;
			break;

			case WM_LBUTTONDOWN:
				isShow = false;
				click_p.x = LOWORD(lParam);
				click_p.y = HIWORD(lParam);
				dwStart = GetTickCount();

				result = 0;
				wasHandled = true;
				break;
			case WM_LBUTTONUP:
				isShow = true;
				isShoot = true;
				Power = GetTickCount() - dwStart;
				if (Power < 0) {
					Power = 0;
				}
				else if (Power > 2000) {
					Power = 2000;
				}
				Power = Power / 20;

				result = 0;
				wasHandled = true;
				break;
			//키보드 제어
			case WM_KEYDOWN:
			{
				input_str = (TCHAR)wParam;

				if (isStarted == true) {
					if (input_str == VK_RIGHT) {
						if (p1_location.x < 870)
							p1_location.x = p1_location.x + 30;
					}
					if (input_str == VK_LEFT) {
						if (p1_location.x > 0)
							p1_location.x = p1_location.x - 30;
					}
				}
				
				if (input_str == VK_SPACE)
					isStarted = !isStarted;
			}
			result = 0;
			wasHandled = true;
			break;

			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}


HRESULT DemoApp::LoadResourceBitmap(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	PCWSTR resourceName,
	PCWSTR resourceType,
	ID2D1Bitmap **ppBitmap
)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource.
	imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

	hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
		);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}

	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pStream);
	SafeRelease(&pConverter);

	return hr;
}
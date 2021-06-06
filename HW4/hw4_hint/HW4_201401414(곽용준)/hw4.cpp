// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "hw3.h"
#include "InputKeyBoard.h"
#include "Boss.h"

#define radian			57.295779513082323
#define PI 3.1415926535897
//발사체를 위한 플레이어 위치 저장
D2D_POINT_2F attackPoint;
D2D_POINT_2F startPoint;
/*현재 마우스 확인 포인트*/
D2D_POINT_2F click_p;
//플레이어 위치
D2D_POINT_2F p1_location;
D2D_POINT_2F p2_location;

//미사일 발사 포인트
D2D_POINT_2F MissileEnd;
D2D_POINT_2F Missile;

/*발사 각, 힘, 발사 시간*/
FLOAT Angle = 0.0f;
FLOAT Power = 0.0f;
DWORD dwStart = 0;


D2D1_SIZE_F rtSize;
UINT winWidth;
UINT winHeight;

FLOAT x_rate = 0.0f;
FLOAT y_rate = 0.0f;

float gravity = 9.8;
float missiletime = 0;
float fallingtime = 0;
float jump_time = 0;
float dt = 0.1f; //시간 간격(timestep)
float volume = 29; //볼륨
//act
bool act1 =	true;
bool act2 = false;

//플레이어 상태
bool character_jump = false;
int walkcount = 0;

std::vector<Boss*> enemyBoss;

//person 클래스 구현
Person::Person(int health) {
	this->health = health;
}
//플레이어 생성
Person p1;

//키보드 DirectInput  
InputKeyboard* inputKeyboard = NULL;
DWORD key = 0;
HWND hWnd = NULL;
CSoundManager* soundManager = NULL;
//메서드 구현
//라디안 값 변환 메서드
FLOAT getRadian(FLOAT num) {
	return num * (PI / 180);
}

// 화면보기/끄기와 시작/정지 flag
WCHAR input_str;
bool isShow = false;
bool isStarted = true;
bool isStop = false;

//flag
bool Shoot = false;
bool Clear = false;
bool Explosion = false;
bool Over = false;

//보스 발사 flag
bool fire = true;


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
	m_pGrayBrush(NULL),
	m_pPathBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL),
	m_pWICFactory(NULL),
	m_pSkyBitmap(NULL),
	m_pNightSkyBitmap(NULL),
	m_pCharacterBitmap1(NULL),
	m_pCharacterBitmap2(NULL),
	m_pBossBitmap(NULL),
	m_pMissileBitmap(NULL),
	m_pBulletBitmap(NULL),
	m_pGroundBitmap(NULL),
	m_pExplosionBitmap(NULL),
	m_pGameOverBitmap(NULL),
	m_pHeartBitmap(NULL),
	m_pSkyBitmapBrush(NULL),
	m_pNightSkyBitmapBrush(NULL),
	m_pCharacterBitmapBrush1(NULL),
	m_pCharacterBitmapBrush2(NULL),
	m_pBossBitmapBrush(NULL),
	m_pMissileBitmapBrush(NULL),
	m_pBulletBitmapBrush(NULL),
	m_pGroundBitmapBrush(NULL),
	m_pExplosionBitmapBrush(NULL),
	m_pGameOverBitmapBrush(NULL),
	m_pHeartBitmapBrush(NULL),
	m_Animation(0, 0, 0)
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
	SafeRelease(&m_pNightSkyBitmap);
	SafeRelease(&m_pGroundBitmap);
	SafeRelease(&m_pMissileBitmap);
	SafeRelease(&m_pBulletBitmap);
	SafeRelease(&m_pCharacterBitmap1);
	SafeRelease(&m_pCharacterBitmap2);
	SafeRelease(&m_pBossBitmap);
	SafeRelease(&m_pExplosionBitmap);
	SafeRelease(&m_pGameOverBitmap);
	SafeRelease(&m_pGameClearBitmap);
	SafeRelease(&m_pHeartBitmap);

	SafeRelease(&m_pTextBrush);
	SafeRelease(&m_pBlackBrush);
	SafeRelease(&m_pGrayBrush);
	SafeRelease(&m_pPathBrush);
	SafeRelease(&m_pSkyBitmapBrush);
	SafeRelease(&m_pNightSkyBitmapBrush);
	SafeRelease(&m_pGroundBitmapBrush);
	SafeRelease(&m_pCharacterBitmapBrush1);
	SafeRelease(&m_pCharacterBitmapBrush2);
	SafeRelease(&m_pBossBitmapBrush);
	SafeRelease(&m_pMissileBitmapBrush);
	SafeRelease(&m_pBulletBitmapBrush);
	SafeRelease(&m_pExplosionBitmapBrush);
	SafeRelease(&m_pGameOverBitmapBrush);
	SafeRelease(&m_pGameClearBitmapBrush);
	SafeRelease(&m_pHeartBitmapBrush);
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
	wcex.lpszClassName = L"HW4_201401414(곽용준)";

	RegisterClassEx(&wcex);

	p1_location.x = 90;
	p1_location.y = 490;

	p2_location.x = 790;
	p2_location.y = 460;

	soundManager = new CSoundManager; //사운드매니저 객체생성

									  //사운드를 0부터 차례대로 추가시켜줌 
	//사운드 매니저
	if (!soundManager->add(const_cast<WCHAR*>(L"./sounds/Bgm1.wav") , 0))
		return FALSE;			//act1 배경음
	if (!soundManager->add(const_cast<WCHAR*>(L"./sounds/Bgm2.wav"), 1))
		return FALSE;			//act2 배경음
	if (!soundManager->add(const_cast<WCHAR*>(L"./sounds/launch.wav"), 2))
		return FALSE;			//미사일 발사시 효과음
	if (!soundManager->add(const_cast<WCHAR*>(L"./sounds/Explosion.wav"), 3))
		return FALSE;			//미사일 폭발 효과음
	if (!soundManager->add(const_cast<WCHAR*>(L"./sounds/bullet.wav"), 4))
		return FALSE;			//보스 총알 효과음
	if (!soundManager->add(const_cast<WCHAR*>(L"./sounds/clear.wav"), 5))
		return FALSE;			//레벨 클리어 효과음
	if (!soundManager->add(const_cast<WCHAR*>(L"./sounds/fail.wav"), 6))
		return FALSE;			//게임 실패 효과음

	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		// Create the application window.
		m_hwnd = CreateWindow(
			L"HW4_201401414(곽용준)",
			L"HW4_201401414(곽용준)",
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
			inputKeyboard = new InputKeyboard(m_hwnd);
		
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
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
				D2D1::ColorF(D2D1::ColorF::Gray),
				&m_pGrayBrush
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Red),
				&m_pTextBrush
			);
		}

		/* 배경 및 물체의 비트맵생성 */
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"NightSky",
				L"Image",
				&m_pNightSkyBitmap
			);
		}

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
				L"Boss",
				L"Image",
				&m_pBossBitmap
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
				L"Bullet",
				L"Image",
				&m_pBulletBitmap
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
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"GameOver",
				L"Image",
				&m_pGameOverBitmap
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"GameClear",
				L"Image",
				&m_pGameClearBitmap
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRT,
				m_pWICFactory,
				L"Heart",
				L"Image",
				&m_pHeartBitmap
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
					m_pNightSkyBitmap,
					propertiesXClampYClamp,
					&m_pNightSkyBitmapBrush
				);
			}

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
					m_pBossBitmap,
					propertiesXClampYClamp,
					&m_pBossBitmapBrush
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
					m_pBulletBitmap,
					propertiesXClampYClamp,
					&m_pBulletBitmapBrush
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

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateBitmapBrush(
					m_pGameOverBitmap,
					propertiesXClampYClamp,
					&m_pGameOverBitmapBrush
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateBitmapBrush(
					m_pGameClearBitmap,
					propertiesXClampYClamp,
					&m_pGameClearBitmapBrush
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateBitmapBrush(
					m_pHeartBitmap,
					propertiesXClampYClamp,
					&m_pHeartBitmapBrush
				);
			}
		}
		
		for (int i = 0; i < 1; i++) {
			DWORD enemyStateTransitions[][3] = {
				{ Boss::STATE_STAND, Boss::EVENT_FINDTARGET, Boss::STATE_FOLLOW },
				{ Boss::STATE_STAND, Boss::EVENT_BEATTACKED, Boss::STATE_ATTACK },
				{ Boss::STATE_STAND, Boss::EVENT_DUBIOUS, Boss::STATE_MOVE },
				{ Boss::STATE_MOVE, Boss::EVENT_FINDTARGET, Boss::STATE_FOLLOW },
				{ Boss::STATE_MOVE, Boss::EVENT_STOPWALK, Boss::STATE_STAND },
				{ Boss::STATE_MOVE, Boss::EVENT_BEATTACKED, Boss::STATE_ATTACK },
				{ Boss::STATE_ATTACK, Boss::EVENT_FINDTARGET, Boss::STATE_FOLLOW },
				{ Boss::STATE_ATTACK, Boss::EVENT_HEALTHDRAINED, Boss::STATE_RUNAWAY },
				{ Boss::STATE_ATTACK, Boss::EVENT_OUTOFATTACK, Boss::STATE_FOLLOW },
				{ Boss::STATE_FOLLOW, Boss::EVENT_WITHINATTACK, Boss::STATE_ATTACK },
				{ Boss::STATE_FOLLOW, Boss::EVENT_LOSTTARGET, Boss::STATE_STAND },
				{ Boss::STATE_FOLLOW, Boss::EVENT_HEALTHDRAINED,Boss::STATE_RUNAWAY},
				{ Boss::STATE_RUNAWAY, Boss::EVENT_LOSTTARGET, Boss::STATE_STAND },
			};

			Boss* enemy = new Boss(Boss::TYPE_AI, enemyStateTransitions, 13, (float)(rand() % 700), (float)(rand() % 500), 0);
			enemyBoss.push_back(enemy);

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
	SafeRelease(&m_pGrayBrush);
	SafeRelease(&m_pTextBrush);

	SafeRelease(&m_pSkyBitmap);
	SafeRelease(&m_pNightSkyBitmap);
	SafeRelease(&m_pCharacterBitmap1);
	SafeRelease(&m_pCharacterBitmap2);
	SafeRelease(&m_pBossBitmap);
	SafeRelease(&m_pGroundBitmap);
	SafeRelease(&m_pMissileBitmap);
	SafeRelease(&m_pBulletBitmap);
	SafeRelease(&m_pExplosionBitmap);
	SafeRelease(&m_pGameClearBitmap);
	SafeRelease(&m_pGameOverBitmap);

	SafeRelease(&m_pSkyBitmapBrush);
	SafeRelease(&m_pNightSkyBitmapBrush);
	SafeRelease(&m_pCharacterBitmapBrush1);
	SafeRelease(&m_pCharacterBitmapBrush2);
	SafeRelease(&m_pBossBitmapBrush);
	SafeRelease(&m_pGroundBitmapBrush);
	SafeRelease(&m_pMissileBitmapBrush);
	SafeRelease(&m_pBulletBitmapBrush);
	SafeRelease(&m_pExplosionBitmapBrush);
	SafeRelease(&m_pGameClearBitmapBrush);
	SafeRelease(&m_pGameOverBitmapBrush);
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
		keyrender();
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
		rtSize = m_pRT->GetSize();
		float minWidthHeightScale = min(rtSize.width, rtSize.height) / 512;

		D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(
			minWidthHeightScale,
			minWidthHeightScale
		);

		D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(
			rtSize.width / 16 - 150,
			rtSize.height / 16 - 150
		);
		soundManager->volume(volume);

		// Prepare to draw.
		m_pRT->BeginDraw();

		// Reset to identity transform D2D1::Matrix3x2F::Scale(0.5, 0.5)
		m_pRT->SetTransform(D2D1::Matrix3x2F::Scale(1, 1));
		m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		
		//액트에 따른 맵 배경 변환
		DrawMap();

		D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
		//땅그리기
		rcBrushRect = D2D1::RectF(0, rtSize.height / 2 + 280, rtSize.width, rtSize.height);
		m_pGroundBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
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
		DrawEnemy();

		//체력표시
		if (p1.getHealth() != 0) {
			D2D1_SIZE_F heartSize = m_pHeartBitmap->GetSize();
			for (int i = 0; i < p1.getHealth(); i++) {
				rcBrushRect = D2D1::RectF(0, 0, heartSize.width, heartSize.height);
				D2D1_MATRIX_3X2_F heartTran = D2D1::Matrix3x2F::Translation(50 + (heartSize.width*i) , 50);
				m_pRT->SetTransform(heartTran);
				m_pRT->FillRectangle(&rcBrushRect, m_pHeartBitmapBrush);
			}
		}
		else {
			//gameover
			D2D1_SIZE_F GameOverSize = m_pGameOverBitmap->GetSize();
			D2D1_RECT_F rcEndRect = D2D1::RectF(0, 0, GameOverSize.width, GameOverSize.height);
			m_pRT->SetTransform(D2D1::Matrix3x2F::Translation(rtSize.width / 2 - GameOverSize.width / 2, rtSize.height / 2 - GameOverSize.height / 2));

			m_pGameOverBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
			m_pGameOverBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);
			m_pRT->FillRectangle(&rcEndRect, m_pGameOverBitmapBrush);
			Over = true;
			if (!isStop) {
				soundManager->replay(6);
			}
		}
		
		m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());
		WCHAR szText[100];
		swprintf_s(szText, L"클릭x좌표:%f\n클릭y좌표:%f\nPower:%f\nAngle:%f\n",
			click_p.x, click_p.y, Power, Angle);
		m_pTextBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1));
		/* 텍스트 출력 */
		m_pRT->DrawText(
			szText,
			wcslen(szText),
			m_pTextFormat,
			D2D1::RectF(20.0f, rtSize.height - 120, 200.0f, rtSize.height),
			m_pTextBrush
		);

		
		if (isShow) {
			//발사한 위치 저장
			if (isStarted) {
				startPoint = p1_location;
				isStarted = false;
			}
			D2D1_SIZE_F	missileSize = m_pMissileBitmap->GetSize();
			D2D1_RECT_F rcMissileRect = D2D1::RectF(0, 0, missileSize.width, missileSize.height);
			
			//미사일 발사
			if (Shoot) {
				Angle = atan2(startPoint.y - click_p.y, click_p.x - startPoint.x) * 180 / PI;
				missileLaunch(Power, Angle);

				D2D1_MATRIX_3X2_F missile_tran = D2D1::Matrix3x2F::Translation(Missile.x, Missile.y);
				m_pRT->SetTransform(missile_tran);
				m_pRT->FillRectangle(rcMissileRect, m_pMissileBitmapBrush);
			}
			//미사일이 끝까지 운동시 폭발 이미지 생성
			if (Explosion) {
				D2D1_SIZE_F	explosionSize = m_pExplosionBitmap->GetSize();
				D2D1_RECT_F rcExplosionRect = D2D1::RectF(0, 0, explosionSize.width, explosionSize.height);
				D2D1_MATRIX_3X2_F explosiontran = D2D1::Matrix3x2F::Translation(MissileEnd.x - explosionSize.width / 2, MissileEnd.y);
				m_pRT->SetTransform(explosiontran);

				m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
				m_pRT->FillOpacityMask(
					m_pExplosionBitmap,
					m_pExplosionBitmapBrush,
					D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
					&rcExplosionRect,
					NULL
				);
				if (!isStop) {
					soundManager->replay(3);
				}
			}
		}

		if (character_jump) {
			CharacterJump(13);
		}

		if (Clear) {
			D2D1_SIZE_F GameClearSize = m_pGameClearBitmap->GetSize();
			D2D1_RECT_F rcEndRect = D2D1::RectF(0, 0, GameClearSize.width, GameClearSize.height);
			m_pRT->SetTransform(D2D1::Matrix3x2F::Translation(rtSize.width / 2 - GameClearSize.width / 2, rtSize.height / 2 - GameClearSize.height / 2));

			m_pGameClearBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
			m_pGameClearBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);
			m_pRT->FillRectangle(&rcEndRect, m_pGameClearBitmapBrush);
			if (!isStop) {
				soundManager->replay(5);
			}
		}

		// Commit the drawing operations.
		hr = m_pRT->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}

		/*음소거 기능*/
		if (isStop) {
			for (int i = 0; i < 7; i++)
				soundManager->stop(i);
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
		winWidth = width;
		winHeight = height;

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
				winWidth = width;
				winHeight = height;
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
				if (!Shoot) {
					isShow = false;
					Explosion = false;
					click_p.x = LOWORD(lParam);
					click_p.y = HIWORD(lParam);
					dwStart = GetTickCount();
				}
				
				result = 0;
				wasHandled = true;
				break;
			case WM_LBUTTONUP:
				if (!Shoot && !isShow && !Clear) {
					if (!isStop) {
						soundManager->replay(2);
					}
					isShow = true;
					Shoot = true;
					Power = GetTickCount() - dwStart;
					if (Power < 0) {
						Power = 0;
					}
					else if (Power > 2000) {
						Power = 2000;
					}
					Power = Power / 10;
				}
				if (Clear & act1) {
					Clear = false;
					act1 = false;
					act2 = true;
				}
				else if ((Clear & act2) || Over) {
					
					PostQuitMessage(0);
					
					result = 1;
					wasHandled = true;
					break;
				}
				result = 0;
				wasHandled = true;
				break;
				//키보드 제어
			/* 토글키(t)와 스페이스를 위한 설정들 */
			// 총, 토글키, 음소거, 음량조절은 동기식 처리로 받음 
			case WM_KEYDOWN:
			{
				input_str = (TCHAR)wParam;
				
				if (input_str == VK_OEM_PLUS || input_str == VK_ADD) {
					volume++;
					if (volume >= 40)
						volume = 40;
				}
				else if (input_str == VK_OEM_MINUS || input_str == VK_SUBTRACT) {
					volume--;
					if (volume <= 0)
						volume = 0;
				}
				else if (input_str == 't' || input_str == 'T')
					isShow = !isShow;
				else if (input_str == 's' || input_str == 'S')
					isStop = !isStop;

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
//케릭터 이동
bool DemoApp::keyrender()
{
	inputKeyboard->update();

	key = 0;
	if (inputKeyboard->keyPressed(DIK_RIGHT)) {
		if (p1_location.x < rtSize.width)
			p1_location.x = p1_location.x + 10;
		walkcount = 0;
	}
	if (inputKeyboard->keyPressed(DIK_LEFT)) {
		if (p1_location.x > 0)
			p1_location.x = p1_location.x - 10;
		walkcount = 0;
	}

	if (inputKeyboard->keyPressed(DIK_UP)) {
		if (!character_jump) {
			character_jump = true;
		}
	}

	if (key) {
		::InvalidateRect(hWnd, NULL, FALSE);
	}

	return true;
}
bool keyinit(HWND hWnd)
{
	inputKeyboard = new InputKeyboard(hWnd);

	return true;
}
void DemoApp::CharacterJump(float jump_v) {
	float tempTime;

	jump_time = jump_time + 0.1;
	tempTime = jump_time;
	/*중력 가속도 계산*/
	p1_location.y = p1_location.y - ((jump_v*tempTime) - (gravity*tempTime*tempTime / 2.0));


	/*땅 밑으로 떨어지지 않게*/
	if (p1_location.y > 490) {
		p1_location.y = 490;
		character_jump = false;
		jump_time = 0;
	}
}
void DemoApp::missileLaunch(float power, float angle) {
	Missile = startPoint;

	missiletime += 0.1;
	float temptime = missiletime;

	Missile.x += temptime * power * cos(getRadian(angle));
	Missile.y = Missile.y - (temptime * power * sin(getRadian(angle)) - (gravity * temptime * temptime) / 2.0);

	//바닥에 닿았을 경우
	if (Missile.y > winHeight - 100) {
		Missile.y = winHeight - 100;
		Shoot = false;
		Explosion = true;
		missiletime = 0;

		p1.damaged();
		isStarted = true;
	}
	//화면밖으로 나감
	if (Missile.x >= winWidth) {
		Shoot = false;
		missiletime = 0;
		p1.damaged();

		isStarted = true;
	}
	//p2와 충돌
	if (act1) {
		if ((Missile.x >= p2_location.x && Missile.x <= p2_location.x + 110) &&
			(Missile.y >= p2_location.y && Missile.y <= p2_location.y + 150)) {
			Shoot = false;
			Clear = true;
			Explosion = true;
			missiletime = 0;

			isStarted = true;
		}
	}
	else if (act2) {
		if ((Missile.x >= enemyBoss[0]->pos.x && Missile.x <= enemyBoss[0]->pos.x + 110) &&
			(Missile.y >= enemyBoss[0]->pos.y && Missile.y <= enemyBoss[0]->pos.y + 150)) {
			Shoot = false;
			Clear = true;
			Explosion = true;
			missiletime = 0;

			isStarted = true;
		}
	}
	MissileEnd = Missile;
}

void DemoApp::DrawMap() {

	D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
	if (act1) {
		m_pSkyBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
		m_pSkyBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);
		m_pRT->FillRectangle(&rcBrushRect, m_pSkyBitmapBrush);
	}
	else if (act2) {
		soundManager->stop(0);
		soundManager->play(1);
		m_pNightSkyBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
		m_pNightSkyBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);
		m_pRT->FillRectangle(&rcBrushRect, m_pNightSkyBitmapBrush);
	}
}

void DemoApp::DrawEnemy() {
	D2D1_MATRIX_3X2_F Enemytran = D2D1::Matrix3x2F::Translation(p2_location.x, p2_location.y);
	m_pRT->SetTransform(Enemytran);
	D2D1_SIZE_F enemySize;
	D2D1_RECT_F rcBrushRect3;
	if (act1) {
		enemySize = m_pCharacterBitmap2->GetSize();
		rcBrushRect3 = D2D1::RectF(0, 0, enemySize.width, enemySize.height);

		m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pRT->FillOpacityMask(
			m_pCharacterBitmap2,
			m_pCharacterBitmapBrush2,
			D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
			&rcBrushRect3,
			NULL
		);
		m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}
	else if (act2) {
		/*교수,해리포터(인공지능) 그리기*/
		enemyBoss[0]->setHeroPosition(p2_location.x, p2_location.y);
		enemyBoss[0]->update();
		enemyBoss[0]->a = (enemyBoss[0]->destPosition_y - enemyBoss[0]->pos.y) / (enemyBoss[0]->destPosition_x - enemyBoss[0]->pos.x);
		enemyBoss[0]->b = (enemyBoss[0]->destPosition_y - enemyBoss[0]->a*enemyBoss[0]->destPosition_x);

			/* 랜덤한 좌표생성 */
			if (enemyBoss[0]->random)
			{
				enemyBoss[0]->random = false;
				enemyBoss[0]->destPosition_x = (float)(rand() % (int)(rtSize.width) - 200);
				enemyBoss[0]->destPosition_y = (float)(rand() % (int)(rtSize.height) - 200);
				enemyBoss[0]->setDestPosition(enemyBoss[0]->destPosition_x, enemyBoss[0]->destPosition_y);
				enemyBoss[0]->chaser = true;
				fire = true;
			}
			/* 추격자 위치조정은 주인공 위치조정과 비슷함 */
			else if (enemyBoss[0]->chaser)
			{

				if (enemyBoss[0]->a<1 && enemyBoss[0]->a>-1)
				{
					if (enemyBoss[0]->pos.x < enemyBoss[0]->destPosition_x)
					{
						enemyBoss[0]->pos.x += enemyBoss[0]->velocity * 0.1; //현재의 velocity를 사용하여 위치 갱신
						enemyBoss[0]->pos.y = (enemyBoss[0]->a * enemyBoss[0]->pos.x + enemyBoss[0]->b);
						enemyBoss[0]->velocity += (enemyBoss[0]->force / enemyBoss[0]->mass) * 0.1;

						enemyBoss[0]->angle = (float)((atan(enemyBoss[0]->a) * radian) + 90);
						if (enemyBoss[0]->pos.x >= enemyBoss[0]->destPosition_x)
						{
							//enemyBoss[0]->random=true;
							enemyBoss[0]->chaser = false;
						}
					}
					else
					{
						enemyBoss[0]->pos.x -= enemyBoss[0]->velocity * dt; //현재의 velocity를 사용하여 위치 갱신
						enemyBoss[0]->pos.y = enemyBoss[0]->a * enemyBoss[0]->pos.x + enemyBoss[0]->b;
						enemyBoss[0]->velocity += (enemyBoss[0]->force / enemyBoss[0]->mass) * dt;

						enemyBoss[0]->angle = (float)((atan(enemyBoss[0]->a) * radian)) + 270;
						if (enemyBoss[0]->pos.x <= enemyBoss[0]->destPosition_x)
						{
							//enemyBoss[0]->random=true;
							enemyBoss[0]->chaser = false;
						}
					}
				}

				else
				{
					if (enemyBoss[0]->pos.y < enemyBoss[0]->destPosition_y)
					{
						enemyBoss[0]->pos.y += enemyBoss[0]->velocity * dt; //현재의 velocity를 사용하여 위치 갱신
						enemyBoss[0]->pos.x = (enemyBoss[0]->pos.y - enemyBoss[0]->b) / enemyBoss[0]->a;
						enemyBoss[0]->velocity += (enemyBoss[0]->force / enemyBoss[0]->mass) * dt;

						if (enemyBoss[0]->pos.x < enemyBoss[0]->destPosition_x)
							enemyBoss[0]->angle = (float)((atan(enemyBoss[0]->a) * radian)) + 90;
						else
							enemyBoss[0]->angle = (float)((atan(enemyBoss[0]->a) * radian)) + 270;

						if (enemyBoss[0]->pos.y >= enemyBoss[0]->destPosition_y)
						{
							//enemyBoss[0]->random=true;
							enemyBoss[0]->chaser = false;
						}
					}
					else
					{
						enemyBoss[0]->pos.y -= enemyBoss[0]->velocity * dt; //현재의 velocity를 사용하여 위치 갱신
						enemyBoss[0]->pos.x = (enemyBoss[0]->pos.y - enemyBoss[0]->b) / enemyBoss[0]->a;
						enemyBoss[0]->velocity += (enemyBoss[0]->force / enemyBoss[0]->mass) * dt;

						if (enemyBoss[0]->pos.x < enemyBoss[0]->destPosition_x)
							enemyBoss[0]->angle = (float)((atan(enemyBoss[0]->a) * radian)) + 90;
						else
							enemyBoss[0]->angle = (float)((atan(enemyBoss[0]->a) * radian)) + 270;

						if (enemyBoss[0]->pos.y <= enemyBoss[0]->destPosition_y)
						{
							//enemyBoss[0]->random=true;
							enemyBoss[0]->chaser = false;
						}
					}
				}
			}


			D2D1_MATRIX_3X2_F targetmove = D2D1::Matrix3x2F::Translation(enemyBoss[0]->pos.x , enemyBoss[0]->pos.y);
			m_pRT->SetTransform(targetmove);
			D2D1_SIZE_F targetsize = m_pBossBitmap->GetSize();
			D2D1_RECT_F targetRect = D2D1::RectF(0, 0, targetsize.width, targetsize.height);
			m_pRT->FillRectangle(&targetRect, m_pBossBitmapBrush);
			
			m_pTextBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red, 1));
			/* 텍스트 출력 */
			m_pRT->DrawText(
				enemyBoss[0]->get_state_str(),
				wcslen(enemyBoss[0]->get_state_str()),
				m_pTextFormat,
				D2D1::RectF(20, 130, 200.0f, 160),
				m_pTextBrush
			);

			/*공격*/
			//그때 당시 플레이어의 위치로 총알 발사
			if (fire) {
				fire = false;
				attackPoint = p1_location;
			}

			Attack();
			/*가시영역*/
			
				m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());
				m_pGrayBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1));
				m_pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(enemyBoss[0]->pos.x, enemyBoss[0]->pos.y), 250, 250), m_pGrayBrush);
				//250 범위 안에(isRunable) 있으면 RunAway
				m_pGrayBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue, 1));
				m_pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(enemyBoss[0]->pos.x, enemyBoss[0]->pos.y), 350, 350), m_pGrayBrush);
				//350 범위 안에 (isVisible) 있으면 Follow
				m_pGrayBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Yellow, 1));
				m_pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(enemyBoss[0]->pos.x, enemyBoss[0]->pos.y), 600, 600), m_pGrayBrush);
				//600 보다 벗어나면 (isAttack) 원거리 마법 공격
			
		}		//해리포터 그리기 끝
}

void DemoApp::Attack() {
	if (enemyBoss[0]->attack || enemyBoss[0]->attack_cooltime == true) {
		if (Bqueue.size() == 0) {
			for (int i = 0; i < 5; i++)
				CreateBullet();
		}
		
		std::queue <Bullet> tempbullet;
		Bullet temp;
		for (int i = Bqueue.size(); i > 0; i--) {
			temp = Bqueue.front();
			Bqueue.pop();
			DrawBullet(&temp);
			BulletCollision(&temp);
			//tempbook.push(temp);
			if (temp.y <= rtSize.height && temp.era == 0)
			{
				tempbullet.push(temp);
			}
		}
		
		Bqueue = tempbullet;

		for (int i = tempbullet.size(); i > 0; i--) {
			tempbullet.pop();
		}
		
		//초기화
		if (Bqueue.size() == 0) {
			enemyBoss[0]->attack_cooltime = false;
		}
		
	}
}

void DemoApp::CreateBullet() {
	Bullet tempBullet;
	tempBullet.x = enemyBoss[0]->pos.x;
	tempBullet.y = enemyBoss[0]->pos.y;
	tempBullet.speed = 50;
	tempBullet.era = 0;
	tempBullet.mass = 1;
	Bqueue.push(tempBullet);
}

void DemoApp::DrawBullet(struct Bullet *temp) {
	if (!isStop) {
		soundManager->replay(4);
	}

	D2D1_MATRIX_3X2_F bulletmove = D2D1::Matrix3x2F::Translation((*temp).x, (*temp).y);
	m_pRT->SetTransform(bulletmove);
	D2D1_SIZE_F bulletsize = m_pBulletBitmap->GetSize();
	D2D1_RECT_F bulletRect = D2D1::RectF(0, 0, bulletsize.width, bulletsize.height);
	m_pRT->FillRectangle(&bulletRect, m_pBulletBitmapBrush);
	
	if ((*temp).x > attackPoint.x) {
		(*temp).x = (*temp).x - (enemyBoss[0]->pos.x - attackPoint.x) / temp->speed;
	}
	else {
		(*temp).x = (*temp).x + (attackPoint.x - enemyBoss[0]->pos.x) / temp->speed;
	}

	if ((*temp).y > attackPoint.y) {
		(*temp).y = (*temp).y - (enemyBoss[0]->pos.y - attackPoint.y) / temp->speed;
	}
	else {
		(*temp).y = (*temp).y + (attackPoint.y - enemyBoss[0]->pos.y) / temp->speed;
	}
	
	
}

void DemoApp::BulletCollision(struct Bullet *temp) {
	if (p1_location.x - (*temp).x <= 40 && p1_location.y - (*temp).y <= 40 && p1_location.x - (*temp).x >= -30) {
		(*temp).era = 1;
		p1.setHealth(0);
	}
}
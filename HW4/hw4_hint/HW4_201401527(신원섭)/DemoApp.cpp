#include "DemoApp.h"
#include "Boss.h"
#define radian			57.295779513082323
float dt = 0.1f; //시간 간격(timestep)

//키보드 DirectInput  
InputKeyboard* inputKeyboard = NULL;
DWORD key = 0;
HWND hWnd = NULL;
//별 위치
//srand((unsigned int)time(NULL));
D2D1_POINT_2F starMove[STARCOUNT] = { 100, 100 };
//D2D1_POINT_2F starMove[STARCOUNT] = { 100,100,500,100,700,100 };

static float total_time = 0.0f;
float fall_time = 0;

//D2D1_POINT_2F bossMove = { 800, 225 };
D2D1_POINT_2F bossMove = { 800, 139 };
int jumpHeigh = 10;
bool end = false;
bool win = false;

bool isShow = false;

// 화면보기
WCHAR input_str;
/* 창, 캐릭터이미지의 크기 */
D2D1_SIZE_F rtSize;
D2D1_SIZE_U  heroSize;
//까마귀0움직임
bool crow0Fly = true;
int crow0X = 0;
/* 캐릭터 움직임을 위한 좌표 */
D2D1_POINT_2F heroMove = { 30, 600 };
/* 캐릭터_나 의 상태 */
int isRun = 0;
bool isHit = false;
//걷는 모션
int walkposition = 0;
//걷는 방향
bool isRight = true;

//점프 상태
bool character_jump = false;
bool isFire = false;
//중력 계산 , 점프
float gravity = 9.8;
float jump_time = 0;

/* 사운드 설정 */
CSoundManager* soundManager; //사운드매니저
int id = 0;					 //각 사운드별 id
bool isStop = false;		 //사운드 정지를 위한 flag
bool isMuse = false;
int vol = 80;				 //사운드 볼륨조절을 위한 변수값 (100이면 최대출력, -50이면 무음)
int exvol = 0;


int Knockback = 0;
float knockback_energe = 0;
std::vector<Boss*> enemyBoss;

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
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

DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pD2DFactory(NULL),
	m_pWICFactory(NULL),
	m_pRenderTarget(NULL),
	m_pCrowBitmap(NULL),
	m_pCrowBitmap2(NULL),
	m_pTreeBitmap(NULL),
	m_pEnemyBitmap(NULL),
	m_pLBullet(NULL),
	m_pRBullet(NULL),
	m_pLFireGun(NULL),
	m_pRFireGun(NULL),
	m_pSton(NULL),
	m_pTallSton(NULL),
	//m_pHeroBitmap(NULL),
	//m_pCrowMotionBitmap(NULL),
	m_pBitmap(NULL),
	m_pBitmapEnd(NULL),
	m_pBitmapWin(NULL),
	m_pMoonGeometry(NULL),
	//m_pStarGeometry(NULL),
	m_pStarFallGeometry(NULL),
	m_pSceneBrush(NULL),
	m_pStarBrush(NULL),
	m_pBossHpBrush(NULL),
	m_pRadialGradientBrush(NULL),
	m_pLinearStarGradientBrush(NULL),
	m_pCrow_Nomal_BitmapBrush(NULL),
	m_pCrow_Nomal_Bitmap2Brush(NULL),
	m_pTree_Nomal_BitmapBrush(NULL),
	m_pEnemy_Nomal_BitmapBrush(NULL),
	m_pLFireGun_BitmapBrush(NULL),
	m_pRFireGun_BitmapBrush(NULL),
	m_pLBullet_BitmapBrush(NULL),
	m_pRBullet_BitmapBrush(NULL),
	m_pBitmapEnd_BitmapBrush(NULL),
	m_pBitmapWin_BitmapBrush(NULL),
	m_pTallSton_BitmapBrush(NULL),
	m_pSton_BitmapBrush(NULL),
	m_pTextFormat(NULL)


{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pBitmapEnd);
	SAFE_RELEASE(m_pBitmapWin);
	SAFE_RELEASE(m_pCrowBitmap);
	SAFE_RELEASE(m_pCrowBitmap2);
	SAFE_RELEASE(m_pTreeBitmap);
	SAFE_RELEASE(m_pEnemyBitmap);
	SAFE_RELEASE(m_pLBullet);
	SAFE_RELEASE(m_pRBullet);
	SAFE_RELEASE(m_pTallSton);
	SAFE_RELEASE(m_pSton);
	SAFE_RELEASE(m_pLFireGun);
	SAFE_RELEASE(m_pRFireGun);
	//SAFE_RELEASE(m_pHeroBitmap);
	//SAFE_RELEASE(m_pCrowMotionBitmap);
	SAFE_RELEASE(m_pMoonGeometry);
	//SAFE_RELEASE(m_pStarGeometry);
	SAFE_RELEASE(m_pStarFallGeometry);
	SAFE_RELEASE(m_pSceneBrush);
	SAFE_RELEASE(m_pStarBrush);
	SAFE_RELEASE(m_pBossHpBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pLinearStarGradientBrush);
	SAFE_RELEASE(m_pCrow_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pCrow_Nomal_Bitmap2Brush);
	SAFE_RELEASE(m_pTree_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pEnemy_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pBitmapEnd_BitmapBrush);
	SAFE_RELEASE(m_pBitmapWin_BitmapBrush);
	SAFE_RELEASE(m_pLFireGun_BitmapBrush);
	SAFE_RELEASE(m_pRFireGun_BitmapBrush);
	SAFE_RELEASE(m_pLBullet_BitmapBrush);
	SAFE_RELEASE(m_pRBullet_BitmapBrush);
	SAFE_RELEASE(m_pTallSton_BitmapBrush);
	SAFE_RELEASE(m_pSton_BitmapBrush);
	SAFE_RELEASE(m_pTextFormat);



}

HRESULT DemoApp::Initialize()
{
	HRESULT hr;

	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = "201401527신원섭";
		RegisterClassEx(&wcex);

		m_hwnd = CreateWindow(
			"201401527신원섭", "201401527신원섭", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			1000, 700, NULL, NULL, HINST_THISCOMPONENT, this);

		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
		QueryPerformanceFrequency(&m_nFrequency);
		QueryPerformanceCounter(&m_nPrevTime);

	}
	if (SUCCEEDED(hr))
	{
		// sound
		InitSound(m_hwnd);
		//soundManager->pDSBPrimary->SetVolume(vol);
		soundManager->pDSBPrimary->SetVolume(DSVOLUME_TO_DB(vol));
		//enemyCharacter->soundManager = soundManager;
		soundManager->play(1, TRUE);
	}



	if (SUCCEEDED(hr))
	{
		float length = 0;
		hr = m_pCrow_PathGeometry->ComputeLength(NULL, &length);

		if (SUCCEEDED(hr))
		{
			m_Animation.SetStart(0); //start at beginning of path
			m_Animation.SetEnd(length); //length at end of path
			m_Animation.SetDuration(5.0f); //seconds

			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr;
	ID2D1GeometrySink *pSink = NULL;

	// WIC 팩토리를 생성함.
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	//d2d키보드 사용
	if (SUCCEEDED(hr))
	{
		inputKeyboard = new InputKeyboard(m_hwnd);
	}
	// D2D 팩토리를 생성함.
	if (SUCCEEDED(hr))
	{
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	}

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
			30.0f,
			L"en-us",
			&m_pTextFormat
		);
	}

	/* 경로를 그리는 기하 */
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_DASH,
				0.0f),
			NULL,
			0,
			&m_pStrokeStyleDash
		);
	}
	RECT rc;
	GetClientRect(m_hwnd, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	m_pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(m_hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		&m_pRenderTarget);

	//까마귀 경로 그리기
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pCrow_PathGeometry);

		if (SUCCEEDED(hr))
		{
			hr = m_pCrow_PathGeometry->Open(&pSink);

			if (SUCCEEDED(hr))
			{
				D2D1_POINT_2F currentLocation = { 1060.0f, 120.0f };

				pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_FILLED);

				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(700, 60), D2D1::Point2F(550, 60), D2D1::Point2F(250, 210)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(320, 160), D2D1::Point2F(450, 260), D2D1::Point2F(550, 310)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(820, 360), D2D1::Point2F(1050, 160), D2D1::Point2F(1260, 310)));

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSink->Close();
			}
			SAFE_RELEASE(pSink);
		}
	}

	//달의 경로기하 그리기
	if (SUCCEEDED(hr))
	{

		hr = m_pD2DFactory->CreatePathGeometry(&m_pMoonGeometry);
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink* pSink = NULL;

			hr = m_pMoonGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					D2D1::Point2F(180, 100),
					D2D1_FIGURE_BEGIN_FILLED
				);
				pSink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(280, 100),
						D2D1::SizeF(40, 40),
						0.0f,
						D2D1_SWEEP_DIRECTION_CLOCKWISE,
						D2D1_ARC_SIZE_SMALL
					));
				pSink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(180, 100),
						D2D1::SizeF(40, 40),
						0.0f,
						D2D1_SWEEP_DIRECTION_CLOCKWISE,
						D2D1_ARC_SIZE_SMALL
					));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			}
			hr = pSink->Close();

			SAFE_RELEASE(pSink);
		}
	}
	
	for (int i = 0; i < STARCOUNT; i++) {
		if (SUCCEEDED(hr)) //별모양 만들기
		{

			hr = m_pD2DFactory->CreatePathGeometry(&m_pStarGeometry[i]);
			if (SUCCEEDED(hr))
			{
				ID2D1GeometrySink* pSink = NULL;

				hr = m_pStarGeometry[i]->Open(&pSink);
				if (SUCCEEDED(hr))
				{
					pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

					pSink->BeginFigure(
						D2D1::Point2F(starMove[i].x, starMove[i].y),
						D2D1_FIGURE_BEGIN_FILLED
					);
					//D2D1_POINT_2F points[6] = { D2D1::Point2F(starMove[i].x + 25, starMove[i].y - 5),D2D1::Point2F(starMove[i].x - 25, starMove[i].y - 5), D2D1::Point2F(starMove[i].x + 15, starMove[i].y + 25),D2D1::Point2F(starMove[i].x, starMove[i].y - 25), D2D1::Point2F(starMove[i].x - 15, starMove[i].y + 25), D2D1::Point2F(starMove[i].x + 25, starMove[i].y - 5), };

					D2D1_POINT_2F points[5] = { D2D1::Point2F(starMove[i].x - 50, starMove[i].y), D2D1::Point2F(starMove[i].x - 10, starMove[i].y + 30),D2D1::Point2F(starMove[i].x - 25, starMove[i].y - 20), D2D1::Point2F(starMove[i].x - 40, starMove[i].y + 30), D2D1::Point2F(starMove[i].x, starMove[i].y), };
					pSink->AddLines(points, ARRAYSIZE(points));
					pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
				}
				hr = pSink->Close();
				SAFE_RELEASE(pSink);
			}
		}


		if (SUCCEEDED(hr)) //별떨어질때 불 기하 만들기
		{

			hr = m_pD2DFactory->CreatePathGeometry(&m_pStarFallFireGeometry[i]);
			if (SUCCEEDED(hr))
			{
				ID2D1GeometrySink* pSink = NULL;

				hr = m_pStarFallFireGeometry[i]->Open(&pSink);
				if (SUCCEEDED(hr))
				{
					pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

					pSink->BeginFigure(
						D2D1::Point2F(starMove[i].x - 50, starMove[i].y),
						D2D1_FIGURE_BEGIN_FILLED
					);

					pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(starMove[i].x - 45, starMove[i].y + 20), D2D1::Point2F(starMove[i].x - 35, starMove[i].y + 40), D2D1::Point2F(starMove[i].x - 25, starMove[i].y + 40)));
					pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(starMove[i].x - 15, starMove[i].y + 40), D2D1::Point2F(starMove[i].x - 5, starMove[i].y + 20), D2D1::Point2F(starMove[i].x, starMove[i].y)));
					pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(starMove[i].x - 15, starMove[i].y - 20), D2D1::Point2F(starMove[i].x - 20, starMove[i].y - 30), D2D1::Point2F(starMove[i].x - 25, starMove[i].y - 40)));
					pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(starMove[i].x - 30, starMove[i].y - 30), D2D1::Point2F(starMove[i].x - 35, starMove[i].y - 20), D2D1::Point2F(starMove[i].x - 50, starMove[i].y)));
					pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
				}
				hr = pSink->Close();
				SAFE_RELEASE(pSink);
			}
		}

	}

	if (SUCCEEDED(hr)) //확인용 기하 만들기(네모모양)
	{

		hr = m_pD2DFactory->CreatePathGeometry(&m_pStarFallGeometry);
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink* pSink = NULL;

			hr = m_pStarFallGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					//D2D1::Point2F(starMove.x-50, starMove.y-20),
					//D2D1::Point2F(heroMove.x-50, heroMove.y-20),
					D2D1::Point2F(300, 600),
					D2D1_FIGURE_BEGIN_FILLED
				);
				//D2D1_POINT_2F points[4] = { D2D1::Point2F(starMove.x,starMove.y-20),D2D1::Point2F(starMove.x,starMove.y+30), D2D1::Point2F(starMove.x-50,starMove.y+30), D2D1::Point2F(starMove.x-50,starMove.y-20) };
				//D2D1_POINT_2F points[4] = { D2D1::Point2F(heroMove.x-50,heroMove.y +50),D2D1::Point2F(heroMove.x+50,heroMove.y + 50), D2D1::Point2F(heroMove.x +50,heroMove.y -50), D2D1::Point2F(heroMove.x - 50,heroMove.y - 50) };
				//D2D1_POINT_2F points[4] = { D2D1::Point2F(heroMove.x,heroMove.y-20),D2D1::Point2F(heroMove.x,heroMove.y+30), D2D1::Point2F(heroMove.x-50,heroMove.y+30), D2D1::Point2F(heroMove.x-50,heroMove.y-20) };
				D2D1_POINT_2F points[4] = { D2D1::Point2F(400, 600),D2D1::Point2F(400, 650),D2D1::Point2F(300, 650),D2D1::Point2F(300, 600) };


				pSink->AddLines(points, ARRAYSIZE(points));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			}
			hr = pSink->Close();
			SAFE_RELEASE(pSink);
		}
	}
	return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// D2D 렌더타겟을 생성함.
		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);
		//hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory,L"SampleImage.jpg",200,0, &m_pBitmap);
		// 응용 프로그램 리소스로부터 비트맵 객체 m_pBitmap를 생성함.
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\Win.jpg", 1400, 800, &m_pBitmapWin);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\gameover.jpg", 1400, 800, &m_pBitmapEnd);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\배경.jpg", 1400, 800, &m_pBitmap);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\까마귀0.png", 100, 100, &m_pCrowBitmap);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\까마귀2.png", 100, 100, &m_pCrowBitmap2);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\나무.png", 500, 700, &m_pTreeBitmap);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\적.png", 300, 350, &m_pEnemyBitmap);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\걷기1.png", 100, 150, &m_pHeroBitmap[0]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\걷기2.png", 100, 150, &m_pHeroBitmap[1]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\걷기3.png", 100, 150, &m_pHeroBitmap[2]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\걷기4.png", 100, 150, &m_pHeroBitmap[3]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\걷기5.png", 100, 150, &m_pHeroBitmap[4]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\걷기6.png", 100, 150, &m_pHeroBitmap[5]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\앉기.png", 100, 150, &m_pHeroBitmap[6]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\점프1.png", 100, 150, &m_pHeroBitmap[7]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\점프2.png", 100, 150, &m_pHeroBitmap[8]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\점프3.png", 100, 150, &m_pHeroBitmap[9]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\까마귀모음.png", 600, 200, &m_pCrowMotionBitmap[0]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\까마귀모음1.png", 600, 200, &m_pCrowMotionBitmap[1]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\까마귀모음2.png", 600, 200, &m_pCrowMotionBitmap[2]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\까마귀모음3.png", 600, 200, &m_pCrowMotionBitmap[3]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\까마귀모음4.png", 600, 200, &m_pCrowMotionBitmap[4]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\까마귀모음5.png", 600, 200, &m_pCrowMotionBitmap[5]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\L총알.png", 30, 15, &m_pLBullet);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\R총알.png", 30, 15, &m_pRBullet);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\L불꽃.png", 30, 20, &m_pLFireGun);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\R불꽃.png", 30, 20, &m_pRFireGun);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\긴돌.png", 100, 50, &m_pTallSton);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\돌.png", 50, 50, &m_pSton);



		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.f), &m_pSceneBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.f), &m_pStarBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.f), &m_pBossHpBrush);
		}

		//달의 브러쉬
		if (SUCCEEDED(hr))
		{
			// D2D1_GRADIENT_STOP 구조체 배열을 생성함.
			ID2D1GradientStopCollection* pGradientStops = NULL;

			D2D1_GRADIENT_STOP gradientStops[3];
			gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke, 1);
			gradientStops[0].position = 0.0f;
			gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::DarkSlateGray, 0.8f);
			gradientStops[1].position = 0.85f;
			gradientStops[2].color = D2D1::ColorF(D2D1::ColorF::Gray, 0.7f);
			gradientStops[2].position = 1.0f;

			// D2D1_GRADIENT_STOP 구조체 배열로부터 ID2D1GradientStopCollection을 생성함.
			hr = m_pRenderTarget->CreateGradientStopCollection(
				gradientStops,
				3,
				D2D1_GAMMA_2_2,
				D2D1_EXTEND_MODE_CLAMP,
				&pGradientStops
			);

			// ID2D1GradientStopCollection로부터 방사형 계조 붓을 생성함.
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateRadialGradientBrush(
					D2D1::RadialGradientBrushProperties(
						D2D1::Point2F(200, 80),
						D2D1::Point2F(0, 0),
						100,
						100),
					pGradientStops,
					&m_pRadialGradientBrush
				);
			}

			SAFE_RELEASE(pGradientStops);
		}

		//별이 떨어지는 모습
		if (SUCCEEDED(hr))
		{
			// D2D1_GRADIENT_STOP 구조체 배열을 생성함.
			ID2D1GradientStopCollection* pGradientStarStops = NULL;

			D2D1_GRADIENT_STOP gradientStarStops[3];
			gradientStarStops[0].color = D2D1::ColorF(D2D1::ColorF::PaleVioletRed, 1);
			gradientStarStops[0].position = 0.0f;
			gradientStarStops[1].color = D2D1::ColorF(D2D1::ColorF::AntiqueWhite, 0.8f);
			gradientStarStops[1].position = 0.85f;
			gradientStarStops[2].color = D2D1::ColorF(D2D1::ColorF::Gold, 0.7f);
			gradientStarStops[2].position = 1.0f;

			// D2D1_GRADIENT_STOP 구조체 배열로부터 ID2D1GradientStopCollection을 생성함.
			hr = m_pRenderTarget->CreateGradientStopCollection(
				gradientStarStops,
				3,
				D2D1_GAMMA_2_2,
				D2D1_EXTEND_MODE_CLAMP,
				&pGradientStarStops
			);

			// ID2D1GradientStopCollection로부터 방사형 계조 붓을 생성함.
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(770, 80), D2D1::Point2F(770, 130)), pGradientStarStops, &m_pLinearStarGradientBrush);

			}

			SAFE_RELEASE(pGradientStarStops);
		}
		/* 배경 및 물체의 Brush 생성 */
		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
			);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					m_pCrowBitmap,
					propertiesXClampYClamp,
					&m_pCrow_Nomal_BitmapBrush
				);
			}
		}
		/* 배경 및 물체의 Brush 생성 */
		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
			);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					m_pCrowBitmap2,
					propertiesXClampYClamp,
					&m_pCrow_Nomal_Bitmap2Brush
				);
			}
		}
		
	}

	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pTreeBitmap,
				propertiesXClampYClamp,
				&m_pTree_Nomal_BitmapBrush
			);
		}
	}

	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pEnemyBitmap,
				propertiesXClampYClamp,
				&m_pEnemy_Nomal_BitmapBrush
			);
		}
	}
	//영웅 브러쉬
	for (int i= 0; i < 10; i++) {
		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
			);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					m_pHeroBitmap[i],
					propertiesXClampYClamp,
					&m_pHero_Nomal_BitmapBrush[i]
				);
			}
		}
	}

	//까마귀 모션 브러쉬
	for (int i = 0; i < 6; i++) {
		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
			);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					m_pCrowMotionBitmap[i],
					propertiesXClampYClamp,
					&m_pCrowMotion_Nomal_BitmapBrush[i]
				);
			}
		}
	}
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pBitmapEnd,
				propertiesXClampYClamp,
				&m_pBitmapEnd_BitmapBrush
			);
		}
	}
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pBitmapWin,
				propertiesXClampYClamp,
				&m_pBitmapWin_BitmapBrush
			);
		}
	}
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pLBullet,
				propertiesXClampYClamp,
				&m_pLBullet_BitmapBrush
			);
		}
	}
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pRBullet,
				propertiesXClampYClamp,
				&m_pRBullet_BitmapBrush
			);
		}
	}
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pLFireGun,
				propertiesXClampYClamp,
				&m_pLFireGun_BitmapBrush
			);
		}
	}
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pRFireGun,
				propertiesXClampYClamp,
				&m_pRFireGun_BitmapBrush
			);
		}
	}

	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pTallSton,
				propertiesXClampYClamp,
				&m_pTallSton_BitmapBrush
			);
		}
	}	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_EXTEND_MODE_CLAMP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(
				m_pSton,
				propertiesXClampYClamp,
				&m_pSton_BitmapBrush
			);
		}
	}


	/*보스 1마리 생성*/
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

		Boss* enemy = new Boss(Boss::TYPE_AI, enemyStateTransitions, 13, 900, 340, 0);
		enemyBoss.push_back(enemy);

	}

	return hr;
}



void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pBitmapEnd);
	SAFE_RELEASE(m_pBitmapWin);
	SAFE_RELEASE(m_pCrowBitmap);
	SAFE_RELEASE(m_pCrowBitmap2);
	SAFE_RELEASE(m_pTreeBitmap);
	SAFE_RELEASE(m_pEnemyBitmap);
	SAFE_RELEASE(m_pLBullet);
	SAFE_RELEASE(m_pRBullet);
	SAFE_RELEASE(m_pTallSton);
	SAFE_RELEASE(m_pSton);
	SAFE_RELEASE(m_pLFireGun);
	SAFE_RELEASE(m_pRFireGun);
	//SAFE_RELEASE(m_pHeroBitmap);
	SAFE_RELEASE(m_pSceneBrush);
	SAFE_RELEASE(m_pStarBrush);
	SAFE_RELEASE(m_pBossHpBrush);
	SAFE_RELEASE(m_pCrow_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pCrow_Nomal_Bitmap2Brush);
	SAFE_RELEASE(m_pTree_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pEnemy_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pLinearStarGradientBrush);
	SAFE_RELEASE(m_pBitmapEnd_BitmapBrush);
	SAFE_RELEASE(m_pBitmapWin_BitmapBrush);
	SAFE_RELEASE(m_pLFireGun_BitmapBrush);
	SAFE_RELEASE(m_pRFireGun_BitmapBrush);
	SAFE_RELEASE(m_pLBullet_BitmapBrush);
	SAFE_RELEASE(m_pRBullet_BitmapBrush);
	SAFE_RELEASE(m_pTallSton_BitmapBrush);
	SAFE_RELEASE(m_pSton_BitmapBrush);


}

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

HRESULT DemoApp::OnRender()
{
	HRESULT hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		// 렌더타겟의 크기를 얻음.
		D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));


		static float float_time = 0.0f;
		float length = m_Animation.GetValue(float_time);
		static float gun_time = 0.0f;

		//시간에 따른 거리계산
		D2D1_POINT_2F point, spoint;
		D2D1_POINT_2F tangent;
		//D2D1_MATRIX_3X2_F triangleMatrix;

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
		//까마귀의 경로기하 위치 얻기
		hr = m_pCrow_PathGeometry->ComputePointAtLength(
			length,
			NULL,
			&point,
			&tangent);
		//까마귀의 경로기하 위치 얻기
		D2D1::Matrix3x2F tmp = D2D1::Matrix3x2F::Translation(
			point.x - 50,
			point.y - 50
		);



		D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height); 

		// 비트맵 m_pBitmap의 크기를 얻음.
		D2D1_SIZE_F size = m_pBitmap->GetSize();

		// 첫 번째로 비트맵 m_pBitmap을 그리기.
		D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.f, 0.f);

		// 비트맵 m_pBitmap을 그림.
		m_pRenderTarget->DrawBitmap(
			m_pBitmap,
			D2D1::RectF(
				upperLeftCorner.x,
				upperLeftCorner.y,
				upperLeftCorner.x + size.width,
				upperLeftCorner.y + size.height)
		);

	
		////확인용
		//m_pRenderTarget->DrawGeometry(m_pStarFallGeometry, m_pSceneBrush, 1.f);
		//m_pRenderTarget->FillGeometry(m_pStarFallGeometry, m_pStarBrush);
		//달 그리기
		m_pRenderTarget->FillGeometry(m_pMoonGeometry, m_pRadialGradientBrush);
		m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
		m_pRenderTarget->DrawGeometry(m_pMoonGeometry, m_pSceneBrush, 1.f);

		//나무 그리기
		m_pRenderTarget->FillRectangle(&rcBrushRect, m_pTree_Nomal_BitmapBrush);
		//까마귀,별떨어지는 경로 보여주기 그림
		//m_pRenderTarget->DrawGeometry(m_pCrow_PathGeometry, m_pSceneBrush, 1.f);


		//까마귀0 일자로 움직이기
		if (crow0Fly) {
			if (crow0X > 1400)crow0X = 0;
			crow0X += 1;
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(crow0X, 0));
			m_pRenderTarget->FillRectangle(&rcBrushRect, m_pCrow_Nomal_Bitmap2Brush);
		}

		//까마귀 경로따라 움직이기
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->SetTransform(tmp);
		m_pRenderTarget->FillRectangle(&rcBrushRect, m_pCrow_Nomal_BitmapBrush);
		if (float_time >= m_Animation.GetDuration())
		{
			float_time = 0.0f;
		}
		else
		{
			LARGE_INTEGER CurrentTime;
			QueryPerformanceCounter(&CurrentTime);

			float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
			m_nPrevTime = CurrentTime;
			gun_time += elapsedTime;
			float_time += elapsedTime;
			total_time += elapsedTime;
		}



		//까마귀 모션 그리기 -불투명 마스크 사용
		D2D1_RECT_F rcCrowRect = D2D1::RectF(0, 0, 600, 200);
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(405, 105));
		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pRenderTarget->FillOpacityMask(m_pCrowMotionBitmap[3], m_pCrowMotion_Nomal_BitmapBrush[0], D2D1_OPACITY_MASK_CONTENT_GRAPHICS, &rcCrowRect);
		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	

		

		//영웅 그리기
		D2D1::Matrix3x2F tCharacter1;
		heroSize = m_pHeroBitmap[0]->GetPixelSize();

			if (isRight == true) { //이전에 오른쪽 방향을 눌렀으면 오른쪽으로 향하도록
				tCharacter1 = D2D1::Matrix3x2F::Translation(
					heroMove.x,
					heroMove.y
				);
			}
			else {//이전에 왼쪽방향을 눌렀으면 왼쪽으로 향하도록
				D2D1::Matrix3x2F ts = D2D1::Matrix3x2F::Translation(
					heroMove.x - heroSize.width,
					heroMove.y
				);
				D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(-1.0f, 1.0f), heroMove);
				tCharacter1 = ts * scale; //왼쪽방향을 누를 때 반전시켜서 움직이도록 한다
			}
		
			if (character_jump == true) {
				CharacterJump(jumpHeigh); //점프하기
				jumpHeigh = 10;
			}



		//돌 위가 아닌 경우 땅으로 떨어지게 하는 함
	/*	else if (heroMove.x > 120 && heroMove.x < 640 || heroMove.x>840) {
			CharacterJump(0);
		}*/

		// 영웅 그리기
		m_pRenderTarget->SetTransform(tCharacter1);

		if (isHit) {
			//m_pRenderTarget->FillRectangle(&rcBrushRect, m_pCharacter1_Hit_BitmapBrush);
		}
		
			if (isRun == -1) { //왼쪽 방향키를 눌렀을떄
				if (character_jump == true) {//왼쪽 ALT키를 눌렀을떄
					m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[7]);
				}
				else
				m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[walkposition]); //walkposition : 걷기1~6을 키보드를 누르면 순서대로 나타내게 한다
			}
			else if (isRun == 1) {//오른쪽 방향키를 눌렀을때
				if (character_jump == true) {//왼쪽 ALT키를 눌렀을떄
					m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[7]);
				}
				else
				m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[walkposition]);
			}
			else if (isRun == 2) {//아래쪽 방향키를 눌렀을떄
				m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[6]);
			}
		
			
			else { //가만히 있을때
				m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[0]);
	
			}
			/*총알 생성*/
			if (gun_time > 0.3 && isHit) {
				CreateBullet();
				isHit = !isHit;
				gun_time = 0.0f;
				isFire = true;

			}
			/*총알 Bullet*/
	//오른쪽으로 나가는 총알 
			std::queue <Bullet> temp_bullet;
			Bullet temp;
			int qSize;
			qSize = bullet_R.size();
			for (int i = qSize; i > 0; i--) {
				temp = bullet_R.front();
				bullet_R.pop();
				RightDrawBullet(&temp);
				BulletCollision(&temp);
				if (temp.x <= 1400 && temp.era == 0)
					temp_bullet.push(temp);
			}
			bullet_R = temp_bullet;

			qSize = temp_bullet.size();
			for (int i = 0; i < qSize; i++)
				temp_bullet.pop();
			//임시 큐 비워주기

			//왼쪽으로 나가는 총알
			qSize = bullet_L.size();
			for (int i = qSize; i > 0; i--) {
				temp = bullet_L.front();
				bullet_L.pop();
				LeftDrawBullet(&temp);
				BulletCollision(&temp);
				if (temp.x > 0 && temp.era == 0)
					temp_bullet.push(temp);
			}
			bullet_L = temp_bullet;
			//총발사 불, 총알
			if (isFire) {
				D2D1_MATRIX_3X2_F gunFire;
				m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
				if (isRight) {
					gunFire = D2D1::Matrix3x2F::Translation(heroMove.x + 90, heroMove.y + 65);
					m_pRenderTarget->SetTransform(gunFire);
					m_pRenderTarget->FillRectangle(&rcBrushRect, m_pRFireGun_BitmapBrush);
				}
				else {
					gunFire = D2D1::Matrix3x2F::Translation(heroMove.x - 20, heroMove.y + 65);
					m_pRenderTarget->SetTransform(gunFire);
					m_pRenderTarget->FillRectangle(&rcBrushRect, m_pLFireGun_BitmapBrush);
				}
				isFire = !isFire;
			}
		funcCollisionCheck();


		funcMovingStar(0);
		/*보스(인공지능) 그리기*/
			enemyBoss[0]->setHeroPosition(heroMove.x, heroMove.y);
			enemyBoss[0]->update();
			enemyBoss[0]->a = (enemyBoss[0]->destPosition_y - enemyBoss[0]->pos.y) / (enemyBoss[0]->destPosition_x - enemyBoss[0]->pos.x);
			enemyBoss[0]->b = (enemyBoss[0]->destPosition_y - enemyBoss[0]->a*enemyBoss[0]->destPosition_x);
			if (wcscmp(enemyBoss[0]->get_state_str(),L"FOLLOW")==0 || wcscmp(enemyBoss[0]->get_state_str(), L"ATTACK") == 0)enemyBoss[0]->force = 1000;
			else enemyBoss[0]->force = 0;
			enemyBoss[0]->velocity = FIRST_VELOCITY_CHASER+(enemyBoss[0]->force / enemyBoss[0]->mass) * 0.1;
			/* 랜덤한 좌표생성 */
			if (enemyBoss[0]->random)
			{
				enemyBoss[0]->random = false;
				enemyBoss[0]->destPosition_x = (float)(rand() % 1100);
				enemyBoss[0]->destPosition_y = (float)(rand() % 500);
				enemyBoss[0]->setDestPosition(enemyBoss[0]->destPosition_x, enemyBoss[0]->destPosition_y);
				enemyBoss[0]->chaser = true;
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

			D2D1_MATRIX_3X2_F targetmove = D2D1::Matrix3x2F::Translation(enemyBoss[0]->pos.x + Knockback * knockback_energe, enemyBoss[0]->pos.y);
			m_pRenderTarget->SetTransform(targetmove);
			D2D1_SIZE_F targetsize = m_pEnemyBitmap->GetSize();
			D2D1_RECT_F targetRect = D2D1::RectF(0, 0, targetsize.width, targetsize.height);
			m_pRenderTarget->FillRectangle(&targetRect, m_pEnemy_Nomal_BitmapBrush);



			if (!isShow) {
				/* 유한상태기계 텍스트 출력 */
				m_pRenderTarget->DrawText(
					enemyBoss[0]->get_state_str(),
					wcslen(enemyBoss[0]->get_state_str()),
					m_pTextFormat,
					D2D1::RectF(80, 130, 250.0f, 160),
					m_pBossHpBrush
				);
				//유한상태기계 범위 표시
				m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
				m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1));
				m_pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(enemyBoss[0]->pos.x + Knockback * knockback_energe, enemyBoss[0]->pos.y), 250, 250), m_pSceneBrush);
				//250 범위 안에(isRunable) 있으면 RunAway
				m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue, 1));
				m_pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(enemyBoss[0]->pos.x + Knockback * knockback_energe, enemyBoss[0]->pos.y), 350, 350), m_pSceneBrush);
				//350 범위 안에 (isVisible) 있으면 Follow
				m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Yellow, 1));
				m_pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(enemyBoss[0]->pos.x + Knockback * knockback_energe, enemyBoss[0]->pos.y), 600, 600), m_pSceneBrush);
			}
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			D2D1_RECT_F HealthBar = D2D1::RectF(0, 0, enemyBoss[0]->energe * 75, 20);
			m_pRenderTarget->FillRectangle(HealthBar, m_pBossHpBrush);
			//m_pMultiBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1));

			//m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			//D2D1_SIZE_F stonSize = m_pTallSton->GetSize();
			//m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(300, 650));
			//D2D1_RECT_F Ston = D2D1::RectF(0, 0, stonSize.width, stonSize.height);
			//m_pRenderTarget->FillRectangle(&Ston, m_pTallSton_BitmapBrush);
			//m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(450, 550));
			//m_pRenderTarget->FillRectangle(&Ston, m_pTallSton_BitmapBrush);


		/*Game_end*/
		if (win == false && end == true) {
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pBitmapEnd_BitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
			m_pBitmapEnd_BitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);
			m_pRenderTarget->FillRectangle(&rcBrushRect, m_pBitmapEnd_BitmapBrush);
			for(int i=0;i<4;i++)
			soundManager->stop(i);

		}
		if (end == false && enemyBoss[0]->energe <=0) {
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pBitmapEnd_BitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
			m_pBitmapEnd_BitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);
			m_pRenderTarget->FillRectangle(&rcBrushRect, m_pBitmapWin_BitmapBrush);
			for (int i = 0; i < 4; i++)
				soundManager->stop(i);
			win = true;
		}




		// 그리기 연산들을 제출함.
		hr = m_pRenderTarget->EndDraw();
		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}

	}

	return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}


LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		result = 1;
	}
	else {

		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
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

			case WM_GETMINMAXINFO: //윈도우 창 크기 고정
			{
				MINMAXINFO* mmi = (MINMAXINFO*)lParam;
				mmi->ptMinTrackSize.x = 1400;
				mmi->ptMinTrackSize.y = 800;
				mmi->ptMaxTrackSize.x = 1400;
				mmi->ptMaxTrackSize.y = 800;
			}
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
				//ValidateRect(hwnd, NULL);
			}
			result = 0;
			wasHandled = true;
			break;
			/* 토글키(t)와 스페이스를 위한 설정들 */
			case WM_KEYDOWN:
			{
				input_str = (TCHAR)wParam;
				switch (input_str)
				{

				}
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_KEYUP:
			{
				isRun = 0;
				isHit = false;
				walkposition = 0;
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
			}

		}
		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}


void DemoApp::CharacterJump(float jump_v) {
	float tempTime;

	jump_time = jump_time + 0.1;
	tempTime = jump_time;
	/*중력 가속도 계산*/
	heroMove.y = heroMove.y - ((jump_v*tempTime) - (gravity*tempTime*tempTime / 2.0));


	/*땅 밑으로 떨어지지 않게*/
	if (heroMove.y > 600) {
		heroMove.y = 600;
		character_jump = false;
		jump_time = 0;
	}

	///*돌 위로 올라가기 */
	//if (heroMove.x - 10 + heroSize.width >= 300 && heroMove.x <= 400 ) {
	//	//if (jump_time >= 0.7 &&heroMove.y < 650 && heroMove.y >600)
	//	{
	//		heroMove.y = 550;
	//		character_jump = false;
	//		jump_time = 0;
	//	}
	//}
}


void DemoApp::StarFallMove(float star_v, int i) {
	float tempTime;

		fall_time = fall_time + 0.1;
		tempTime = fall_time;
		/*중력 가속도 계산*/
		starMove[i].y = starMove[i].y - (-(star_v*tempTime) - (gravity*tempTime*tempTime / 2.0));


		/*땅 밑으로 떨어지지 않게*/
		if (starMove[i].y >= 650) {
			starMove[i].y = 100;
			
			starMove[i].x = rand() % 1200 + 100;
			if (i == STARCOUNT - 1) {
				fall_time = 0.0f;
				total_time = 0.0f;
			}
		}
	
}
bool keyinit(HWND hWnd)
{
	inputKeyboard = new InputKeyboard(hWnd);

	return true;
}

/* 주인공 이동, 점프 효과음만 담당*/
bool DemoApp::keyrender()
{
	inputKeyboard->update();

	key = 0;
	if (inputKeyboard->keyPressed(DIK_RIGHT)) {
		if (heroMove.x < 1400 - heroSize.width) {
			isRight = true;
			heroMove.x += 5;
			walkposition = (walkposition + 1) % 6;
			soundManager->play(0, FALSE);

		}
		isRun = 1;
	}
	if (inputKeyboard->keyPressed(DIK_LEFT)) {
		if (heroMove.x > 0) {
			isRight = false;
			heroMove.x -= 5;
			walkposition = (walkposition + 1) % 6;
			soundManager->play(0, FALSE);

		}
		isRun = -1;
	}

	if (inputKeyboard->keyPressed(DIK_DOWN)) {
		if ((heroMove.x > 0 && heroMove.x < 1400 - heroSize.width)) {
			isRun = 2;
		}
	}
	if (inputKeyboard->keyPressed(DIK_UP)) {
		jumpHeigh = 12;
		
	}
	if (inputKeyboard->keyPressed(DIK_LMENU)) { //왼쪽 alt키
		if (character_jump == false) {
			isRun = 3;
			character_jump = true;
			soundManager->play(3, false);

		}
	}
	if (inputKeyboard->keyPressed(DIK_LCONTROL)) {//왼쪽 control키
		if (isHit == false) {
			isHit = true;
			soundManager->play(2, false);

		}
	}
	// 숫자 키보드 - 가 눌렸을 때 볼륨 다운
	if (inputKeyboard->keyPressed(DIK_SUBTRACT)) {
		if (isMuse) {
			vol = exvol;
			isMuse = false;
		}
		if (vol > -50) {
			vol -= 5;
		}
		soundManager->pDSBPrimary->SetVolume(DSVOLUME_TO_DB(vol));
	}
	// 숫자 키보드 + 가 눌렸을 때 볼륨 업
	if (inputKeyboard->keyPressed(DIK_ADD)) {
		if (isMuse) {
			vol = exvol;
			isMuse = false;
		}
		if (vol < 100) {
			vol += 5;
		}
		soundManager->pDSBPrimary->SetVolume(DSVOLUME_TO_DB(vol));
	}
	// t, T가 눌렸을 때
	if (inputKeyboard->keyPressed(DIK_T)) {
		if (!isMuse) {
			exvol = vol;
			vol = -50;
			isMuse = true;
		}
		else {
			vol = exvol;
			isMuse = false;
		}
		isShow =!isShow;
		soundManager->pDSBPrimary->SetVolume(DSVOLUME_TO_DB(vol));
	}

	if (key) {
		::InvalidateRect(hWnd, NULL, FALSE);
	}

	return true;
}

void DemoApp::CreateBullet() {
	Bullet tempBullet;
	tempBullet.x = heroMove.x;
	tempBullet.y = heroMove.y;
	tempBullet.speed = 10;
	tempBullet.era = 0;
	tempBullet.mass = 1;
	if (isRight == true)
		bullet_R.push(tempBullet);
	else 
		bullet_L.push(tempBullet);
}
void DemoApp::RightDrawBullet(struct Bullet *temp) {

	D2D1_MATRIX_3X2_F pensilTranslation = D2D1::Matrix3x2F::Translation((*temp).x +90, (*temp).y +70);
	m_pRenderTarget->SetTransform(pensilTranslation);
	D2D1_SIZE_F pensilsize = m_pRBullet->GetSize();
	D2D1_RECT_F pnsilRect = D2D1::RectF(0, 0, pensilsize.width, pensilsize.height);
	m_pRenderTarget->FillRectangle(&pnsilRect, m_pRBullet_BitmapBrush);
	(*temp).x += (*temp).speed;
}
void DemoApp::LeftDrawBullet(struct Bullet *temp) {

	D2D1_MATRIX_3X2_F pensilTranslation = D2D1::Matrix3x2F::Translation((*temp).x -20, (*temp).y + 70);
	m_pRenderTarget->SetTransform(pensilTranslation);
	D2D1_SIZE_F pensilsize = m_pLBullet->GetSize();
	D2D1_RECT_F pnsilRect = D2D1::RectF(0, 0, pensilsize.width, pensilsize.height);
	m_pRenderTarget->FillRectangle(&pnsilRect, m_pLBullet_BitmapBrush);
	(*temp).x -= (*temp).speed;

}
void DemoApp::BulletCollision(struct Bullet *temp) {
	D2D1_SIZE_U bossSize = m_pEnemyBitmap->GetPixelSize();

		//if ( (*temp).x  >= bossMove.x +30 && (*temp).x <= bossMove.x + bossSize.width -20 &&  (*temp).y >= bossMove.y - 10 && (*temp).y <= bossMove.y + bossSize.height - 10) {
	if (isRight) {
		float bulletRX = (*temp).x + 130;
		float bulletY = (*temp).y + 70;
		float bossCollLX = enemyBoss[0]->pos.x + 30 + Knockback * knockback_energe;;
		float bossCollRX = enemyBoss[0]->pos.x + bossSize.width - 20 + Knockback * knockback_energe;
		//if (bulletRX > 300 && bulletRX <= 400 && bulletY >= 600 && bulletY <= 650)
		if (bulletRX >= bossCollLX && bulletRX <= bossCollRX && bulletY >= enemyBoss[0]->pos.y - 10 && bulletY <= enemyBoss[0]->pos.y + bossSize.height - 10) {
			Knockback++;
			(*temp).era = 1;
			enemyBoss[0]->energe--;
		}
	}
	else {
		float bulletLX = (*temp).x - 20;
		float bulletY = (*temp).y + 70;
		//if (bulletLX > 300 && bulletY <= 400 && bulletY >= 600 && bulletY <= 650)
		if (bulletLX >= enemyBoss[0]->pos.x + 30 && bulletLX <= enemyBoss[0]->pos.x + bossSize.width - 20 && bulletY >= enemyBoss[0]->pos.y - 10 && bulletY <= enemyBoss[0]->pos.y + bossSize.height - 10) {
			
			Knockback--;
			(*temp).era = 1;
			enemyBoss[0]->energe--;
		}

	}
			/*충돌 에너지 계산*/
		knockback_energe = ((*temp).mass*(*temp).speed*(*temp).speed) / 2;
		knockback_energe = knockback_energe - 40;
	
			/*if (isStop)
				soundManager->replay(3);*/

	
}
BOOL DemoApp::InitSound(HWND hWnd) {
	//SOUND
	soundManager = new CSoundManager;
	if (!soundManager->init(hWnd))
		return FALSE;

	// 사운드 파일을 추가함 (id=0부터 시작함)
	int id;

	if (!soundManager->add("./sound/ch1_run.wav", &id)) // 캐릭터 달리기
		return FALSE;
	if (!soundManager->add("./sound/bgm_start.wav", &id)) // 배경_시작
		return FALSE;
	if (!soundManager->add("./sound/herogun.wav", &id)) //캐릭터 총소리
		return FALSE;
	if (!soundManager->add("./sound/점프소리1.wav", &id)) //캐릭터 점프소리
		return FALSE;
	return true;
}
void DemoApp::funcCollisionCheck() {//충돌 이벤트
	//비트맵의 여분 때문에 x,y축으로 충돌 경계를 다시 설정 했다.
	D2D1_SIZE_U bossSize = m_pEnemyBitmap->GetPixelSize();
	float heroCollLX = heroMove.x - 10 + heroSize.width; 
	float heroCollRX = heroMove.x + 10;
	float heroCollY = heroMove.y;

	float bossCollLX = enemyBoss[0]->pos.x + 30 + Knockback * knockback_energe;;
	float bossCollRX = enemyBoss[0]->pos.x + bossSize.width - 20 + Knockback * knockback_energe;;
	if (isRun == 2) heroCollY= heroCollY + 30;
	if (heroCollLX > bossCollLX  && heroCollRX <= bossCollRX && heroCollY >= enemyBoss[0]->pos.y - 10 && heroCollY <= enemyBoss[0]->pos.y + bossSize.height - 10) { //주인공이랑 보스랑 충돌시 게임오버
		if (win == false)end = true;

	}
}


// Creates a Direct2D bitmap from a resource in the application resource file.
HRESULT DemoApp::LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void* pImageFile = NULL;
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
		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
					);
				}

				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
					);
				}
			}
		}
		else
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
	}

	if (SUCCEEDED(hr))
	{
		//create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hr;
}

// Creates a Direct2D bitmap from the specified file name.
HRESULT DemoApp::LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr = S_OK;

	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

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
		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
					);
				}

				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
					);
				}
			}
		}
		else // Don't scale the image.
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

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hr;
}





void DemoApp::funcStarCollisionCheck() {
	float heroCollLX = heroMove.x - 10 + heroSize.width;
	float heroCollRX = heroMove.x + 10;
	float heroCollY = heroMove.y;
	for (int i = 0; i < STARCOUNT; i++) {
		//충돌
		if (heroCollLX > starMove[i].x + 60 && heroCollRX <= starMove[i].x + 105 && heroCollY >= starMove[i].y - 20 && heroCollY <= starMove[i].y + 30) { //주인공이랑 유성 충돌시 게임오버
			if(win ==false)end = true;

		}
	}
}
void DemoApp::funcMovingStar(float _t) {
	for (int i = 0; i < STARCOUNT; i++) {
		//별그리기
		if (total_time < 10) {//별이 30초 이하이면 점점 커지게 보이기
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			//D2D1_POINT_2F starMiddle = {starMove.x+25,starMove.y+5};
			D2D1_MATRIX_3X2_F starFallMove = D2D1::Matrix3x2F::Translation(starMove[i].x, starMove[i].y);
			D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(1.0f*total_time / 10, 1.0f*total_time / 10), starMove[i]);
			m_pRenderTarget->SetTransform(starFallMove*scale);
			m_pRenderTarget->FillGeometry(m_pStarFallFireGeometry[i], m_pLinearStarGradientBrush);
			m_pRenderTarget->DrawGeometry(m_pStarFallFireGeometry[i], m_pSceneBrush, 1.f);
			m_pRenderTarget->FillGeometry(m_pStarGeometry[i], m_pStarBrush);

			//m_pRenderTarget->DrawGeometry(m_pStarFallGeometry, m_pSceneBrush, 1.f);

		}
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(200, -50));
		m_pRenderTarget->FillGeometry(m_pStarGeometry[i], m_pStarBrush);
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(600, 50));
		m_pRenderTarget->FillGeometry(m_pStarGeometry[i], m_pStarBrush);
		//total시간이 30초 이상이면 별 떨어지기
		if (total_time >= 10) {
			D2D1_MATRIX_3X2_F starFallMove = D2D1::Matrix3x2F::Translation(starMove[i].x, starMove[i].y);
			D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(1.0f*total_time / 10, 1.0f*total_time / 10), starMove[i]);
			m_pRenderTarget->SetTransform(starFallMove*scale);
			//별떨어질때 불
			m_pRenderTarget->FillGeometry(m_pStarFallFireGeometry[i], m_pLinearStarGradientBrush);
			m_pRenderTarget->DrawGeometry(m_pStarFallFireGeometry[i], m_pSceneBrush, 1.f);
			m_pRenderTarget->FillGeometry(m_pStarGeometry[i], m_pStarBrush);
			/*if (starMove.y  >= 600) {
				m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
				total_time = 0.0f;
				fall_time = 0.0f;
			}*/
			//m_pRenderTarget->DrawGeometry(m_pStarFallGeometry, m_pSceneBrush, 1.f);
			StarFallMove(0, i);

		}
	}
	funcStarCollisionCheck();

}


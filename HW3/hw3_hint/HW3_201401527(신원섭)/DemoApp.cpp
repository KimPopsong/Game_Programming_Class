#include "DemoApp.h"

//bool isShow = false;
// ȭ�麸��
WCHAR input_str;
/* â, ĳ�����̹����� ũ�� */
D2D1_SIZE_F rtSize;
D2D1_SIZE_U  heroSize;
//���0������
bool crow0Fly = true;
int crow0X = 0;
/* ĳ���� �������� ���� ��ǥ */
D2D1_POINT_2F heroMove = { 0, 0 };
/* ĳ����_�� �� ���� */
int isRun = 0;
BOOL isHit = false;
//�ȴ� ���
int walkposition = 0;
//�ȴ� ����
bool isRight = true;
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
	//m_pHeroBitmap(NULL),
	//m_pCrowMotionBitmap(NULL),
	m_pBitmap(NULL),
	m_pMoonGeometry(NULL),
	m_pStarGeometry(NULL),
	m_pStarFallGeometry(NULL),
	m_pSceneBrush(NULL),
	m_pStarBrush(NULL),
	m_pRadialGradientBrush(NULL),
	m_pLinearStarGradientBrush(NULL),
	m_pCrow_Nomal_BitmapBrush(NULL),
	m_pCrow_Nomal_Bitmap2Brush(NULL),
	m_pTree_Nomal_BitmapBrush(NULL),
	m_pEnemy_Nomal_BitmapBrush(NULL)


{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pCrowBitmap);
	SAFE_RELEASE(m_pCrowBitmap2);
	SAFE_RELEASE(m_pTreeBitmap);
	SAFE_RELEASE(m_pEnemyBitmap);
	//SAFE_RELEASE(m_pHeroBitmap);
	//SAFE_RELEASE(m_pCrowMotionBitmap);
	SAFE_RELEASE(m_pMoonGeometry);
	SAFE_RELEASE(m_pStarGeometry);
	SAFE_RELEASE(m_pStarFallGeometry);
	SAFE_RELEASE(m_pSceneBrush);
	SAFE_RELEASE(m_pStarBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pLinearStarGradientBrush);
	SAFE_RELEASE(m_pCrow_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pCrow_Nomal_Bitmap2Brush);
	SAFE_RELEASE(m_pTree_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pEnemy_Nomal_BitmapBrush);



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
		wcex.lpszClassName = L"201401527�ſ���";
		RegisterClassEx(&wcex);

		m_hwnd = CreateWindow(
			L"201401527�ſ���", L"201401527�ſ���", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
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

	// WIC ���丮�� ������.
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));

	// D2D ���丮�� ������.
	if (SUCCEEDED(hr))
	{
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	}
	/* ��θ� �׸��� ���� */
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

	//��� ��� �׸���
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

	//���� ��α��� �׸���
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
	
	
	if (SUCCEEDED(hr)) //����� �����
	{

		hr = m_pD2DFactory->CreatePathGeometry(&m_pStarGeometry);
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink* pSink = NULL;

			hr = m_pStarGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					D2D1::Point2F(795, 100),
					D2D1_FIGURE_BEGIN_FILLED
				);
				D2D1_POINT_2F points[5] = { D2D1::Point2F(745, 100), D2D1::Point2F(785, 130),D2D1::Point2F(770, 80), D2D1::Point2F(755, 130), D2D1::Point2F(795, 100), };
				pSink->AddLines(points, ARRAYSIZE(points));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			}
			hr = pSink->Close();
			SAFE_RELEASE(pSink);
		}
	}


	if (SUCCEEDED(hr)) //���������� ���� �����
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
					D2D1::Point2F(770, 100),
					D2D1_FIGURE_BEGIN_FILLED
				);
				D2D1_POINT_2F points[1] = { D2D1::Point2F(770, 700) };
				pSink->AddLines(points, ARRAYSIZE(points));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			}
			hr = pSink->Close();
			SAFE_RELEASE(pSink);
		}
	}
	if (SUCCEEDED(hr)) //���������� �� ���� �����
	{

		hr = m_pD2DFactory->CreatePathGeometry(&m_pStarFallFireGeometry);
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink* pSink = NULL;

			hr = m_pStarFallFireGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					D2D1::Point2F(745, 100),
					D2D1_FIGURE_BEGIN_FILLED
				);
				/*pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(795, 100), D2D1::SizeF(20, 20), 0.0f,
					D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_SMALL));*/
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(750, 120), D2D1::Point2F(760, 140), D2D1::Point2F(770, 140)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(780, 140), D2D1::Point2F(790, 120), D2D1::Point2F(795, 100)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(780, 80), D2D1::Point2F(775, 70), D2D1::Point2F(770,60)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(765, 70), D2D1::Point2F(760, 80), D2D1::Point2F(745, 100)));
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

		// D2D ����Ÿ���� ������.
		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);
		//hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory,L"SampleImage.jpg",200,0, &m_pBitmap);
		// ���� ���α׷� ���ҽ��κ��� ��Ʈ�� ��ü m_pBitmap�� ������.
		if (SUCCEEDED(hr))
		{
			//hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"IDB_PNG1", L"PNG", 200, 0, &m_pBitmap);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\���.jpg", 1400, 800, &m_pBitmap);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\���0.png", 100, 100, &m_pCrowBitmap);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\���2.png", 100, 100, &m_pCrowBitmap2);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\����.png", 500, 700, &m_pTreeBitmap);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\��.png", 400, 500, &m_pEnemyBitmap);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\�ȱ�1.png", 100, 150, &m_pHeroBitmap[0]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\�ȱ�2.png", 100, 150, &m_pHeroBitmap[1]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\�ȱ�3.png", 100, 150, &m_pHeroBitmap[2]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\�ȱ�4.png", 100, 150, &m_pHeroBitmap[3]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\�ȱ�5.png", 100, 150, &m_pHeroBitmap[4]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\�ȱ�6.png", 100, 150, &m_pHeroBitmap[5]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\�ɱ�.png", 100, 150, &m_pHeroBitmap[6]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\����1.png", 100, 150, &m_pHeroBitmap[7]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\����2.png", 100, 150, &m_pHeroBitmap[8]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\����3.png", 100, 150, &m_pHeroBitmap[9]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\��͸���.png", 600, 200, &m_pCrowMotionBitmap[0]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\��͸���1.png", 600, 200, &m_pCrowMotionBitmap[1]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\��͸���2.png", 600, 200, &m_pCrowMotionBitmap[2]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\��͸���3.png", 600, 200, &m_pCrowMotionBitmap[3]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\��͸���4.png", 600, 200, &m_pCrowMotionBitmap[4]);
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\image\\��͸���5.png", 600, 200, &m_pCrowMotionBitmap[5]);


		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.f), &m_pSceneBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.f), &m_pStarBrush);
		}

		//���� �귯��
		if (SUCCEEDED(hr))
		{
			// D2D1_GRADIENT_STOP ����ü �迭�� ������.
			ID2D1GradientStopCollection* pGradientStops = NULL;

			D2D1_GRADIENT_STOP gradientStops[3];
			gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke, 1);
			gradientStops[0].position = 0.0f;
			gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::DarkSlateGray, 0.8f);
			gradientStops[1].position = 0.85f;
			gradientStops[2].color = D2D1::ColorF(D2D1::ColorF::Gray, 0.7f);
			gradientStops[2].position = 1.0f;

			// D2D1_GRADIENT_STOP ����ü �迭�κ��� ID2D1GradientStopCollection�� ������.
			hr = m_pRenderTarget->CreateGradientStopCollection(
				gradientStops,
				3,
				D2D1_GAMMA_2_2,
				D2D1_EXTEND_MODE_CLAMP,
				&pGradientStops
			);

			// ID2D1GradientStopCollection�κ��� ����� ���� ���� ������.
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

		//���� �������� ���
		if (SUCCEEDED(hr))
		{
			// D2D1_GRADIENT_STOP ����ü �迭�� ������.
			ID2D1GradientStopCollection* pGradientStarStops = NULL;

			D2D1_GRADIENT_STOP gradientStarStops[3];
			gradientStarStops[0].color = D2D1::ColorF(D2D1::ColorF::PaleVioletRed, 1);
			gradientStarStops[0].position = 0.0f;
			gradientStarStops[1].color = D2D1::ColorF(D2D1::ColorF::AntiqueWhite, 0.8f);
			gradientStarStops[1].position = 0.85f;
			gradientStarStops[2].color = D2D1::ColorF(D2D1::ColorF::Gold, 0.7f);
			gradientStarStops[2].position = 1.0f;

			// D2D1_GRADIENT_STOP ����ü �迭�κ��� ID2D1GradientStopCollection�� ������.
			hr = m_pRenderTarget->CreateGradientStopCollection(
				gradientStarStops,
				3,
				D2D1_GAMMA_2_2,
				D2D1_EXTEND_MODE_CLAMP,
				&pGradientStarStops
			);

			// ID2D1GradientStopCollection�κ��� ����� ���� ���� ������.
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(770, 80), D2D1::Point2F(770, 130)), pGradientStarStops, &m_pLinearStarGradientBrush);

			}

			SAFE_RELEASE(pGradientStarStops);
		}
		/* ��� �� ��ü�� Brush ���� */
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
		/* ��� �� ��ü�� Brush ���� */
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
	//���� �귯��
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

	//��� ��� �귯��
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
	

	return hr;
}



void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pCrowBitmap);
	SAFE_RELEASE(m_pCrowBitmap2);
	SAFE_RELEASE(m_pTreeBitmap);
	SAFE_RELEASE(m_pEnemyBitmap);
	//SAFE_RELEASE(m_pHeroBitmap);
	SAFE_RELEASE(m_pSceneBrush);
	SAFE_RELEASE(m_pStarBrush);
	SAFE_RELEASE(m_pCrow_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pCrow_Nomal_Bitmap2Brush);
	SAFE_RELEASE(m_pTree_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pEnemy_Nomal_BitmapBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pLinearStarGradientBrush);


}

void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT DemoApp::OnRender()
{
	HRESULT hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		// ����Ÿ���� ũ�⸦ ����.
		D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));


		static float float_time = 0.0f;
		static float total_time = 0.0f;
		float length = m_Animation.GetValue(float_time);

		//�ð��� ���� �Ÿ����
		D2D1_POINT_2F point, spoint;
		D2D1_POINT_2F tangent;
		//D2D1_MATRIX_3X2_F triangleMatrix;

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
		//����� ��α��� ��ġ ���
		hr = m_pCrow_PathGeometry->ComputePointAtLength(
			length,
			NULL,
			&point,
			&tangent);
		//����� ��α��� ��ġ ���
		D2D1::Matrix3x2F tmp = D2D1::Matrix3x2F::Translation(
			point.x - 50,
			point.y - 50
		);

		//���� �������� ��α��� ��ġ ���
		hr = m_pStarFallGeometry->ComputePointAtLength(
			length,
			NULL,
			&spoint,
			&tangent);
		//���� �������� ��α��� ��ġ ���
		D2D1::Matrix3x2F starFall = D2D1::Matrix3x2F::Translation(
			spoint.x - 770,
			spoint.y -100
		);





		D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height); 

		// ��Ʈ�� m_pBitmap�� ũ�⸦ ����.
		D2D1_SIZE_F size = m_pBitmap->GetSize();

		// ù ��°�� ��Ʈ�� m_pBitmap�� �׸���.
		D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.f, 0.f);

		// ��Ʈ�� m_pBitmap�� �׸�.
		m_pRenderTarget->DrawBitmap(
			m_pBitmap,
			D2D1::RectF(
				upperLeftCorner.x,
				upperLeftCorner.y,
				upperLeftCorner.x + size.width,
				upperLeftCorner.y + size.height)
		);
		//�� �׸���
		m_pRenderTarget->FillGeometry(m_pMoonGeometry, m_pRadialGradientBrush);
		m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
		m_pRenderTarget->DrawGeometry(m_pMoonGeometry, m_pSceneBrush, 1.f);

		//���� �׸���
		m_pRenderTarget->FillRectangle(&rcBrushRect, m_pTree_Nomal_BitmapBrush);
		//���,���������� ��� �����ֱ� �׸�
		/*m_pRenderTarget->DrawGeometry(m_pCrow_PathGeometry, m_pSceneBrush, 1.f);
		m_pRenderTarget->DrawGeometry(m_pStarFallGeometry, m_pSceneBrush, 1.f);*/


		//���0 ���ڷ� �����̱�
		if (crow0Fly) {
			if (crow0X > 1400)crow0X = 0;
			crow0X += 1;
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(crow0X, 0));
			m_pRenderTarget->FillRectangle(&rcBrushRect, m_pCrow_Nomal_Bitmap2Brush);
		}

		//��� ��ε��� �����̱�
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->SetTransform(tmp);
		m_pRenderTarget->FillRectangle(&rcBrushRect, m_pCrow_Nomal_BitmapBrush);
		//m_pRenderTarget->FillGeometry(m_pStarGeometry, m_pStarBrush);
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

			float_time += elapsedTime;
			total_time += elapsedTime;
		}


		//���׸���
		if (total_time < 30) {//���� 30�� �����̸� ���� Ŀ���� ���̱�
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(1.0f*total_time / 10, 1.0f*total_time / 10), D2D1::Point2F(770.0f, 100.0f));
			m_pRenderTarget->SetTransform(scale);
			m_pRenderTarget->FillGeometry(m_pStarFallFireGeometry, m_pLinearStarGradientBrush);
			m_pRenderTarget->DrawGeometry(m_pStarFallFireGeometry, m_pSceneBrush, 1.f);
			m_pRenderTarget->FillGeometry(m_pStarGeometry, m_pStarBrush);
		}
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(200, -50));
		m_pRenderTarget->FillGeometry(m_pStarGeometry, m_pStarBrush);
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(-400, 50));
		m_pRenderTarget->FillGeometry(m_pStarGeometry, m_pStarBrush);
		//total�ð��� 30�� �̻��̸� �� ��������
		if (total_time >= 30) {
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(1.0f*total_time/10, 1.0f*total_time/10), spoint);
			m_pRenderTarget->SetTransform(starFall*scale);
			//���������� ��
			m_pRenderTarget->FillGeometry(m_pStarFallFireGeometry, m_pLinearStarGradientBrush);
			m_pRenderTarget->DrawGeometry(m_pStarFallFireGeometry, m_pSceneBrush, 1.f);
			m_pRenderTarget->FillGeometry(m_pStarGeometry, m_pStarBrush);
			if (spoint.y  >= 600) {
				m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
				total_time = 0.0f;
			}

		}
		//��� ��� �׸��� -������ ����ũ ���
		D2D1_RECT_F rcCrowRect = D2D1::RectF(0, 0, 600, 200);
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(405, 105));
		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pRenderTarget->FillOpacityMask(m_pCrowMotionBitmap[3], m_pCrowMotion_Nomal_BitmapBrush[0], D2D1_OPACITY_MASK_CONTENT_GRAPHICS, &rcCrowRect);
		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		//���׸���
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(950, 250));
		m_pRenderTarget->FillRectangle(&rcBrushRect, m_pEnemy_Nomal_BitmapBrush);

		//���� �׸���
		D2D1::Matrix3x2F tCharacter1;
		heroSize = m_pHeroBitmap[0]->GetPixelSize();

		if (isRun == -1) {//���ʹ����� ���� �� �������Ѽ� �����̵��� �Ѵ�
			D2D1::Matrix3x2F ts = D2D1::Matrix3x2F::Translation(
				heroMove.x - heroSize.width,
				rtSize.height - heroSize.height
			);
			D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(-1.0f, 1.0f), heroMove);
			tCharacter1 = ts * scale; 
		}
		else if (isRun == 1) {//������ ������ ���� �� ���������� �����δ�
			tCharacter1 = D2D1::Matrix3x2F::Translation(
				heroMove.x,
				rtSize.height - heroSize.height
			);
		}
		else { //������ �� ������
			if (isRight == true) { //������ ������ ������ �������� ���������� ���ϵ���
				tCharacter1 = D2D1::Matrix3x2F::Translation(
					heroMove.x,
					rtSize.height - heroSize.height
				);
			}
			else {//������ ���ʹ����� �������� �������� ���ϵ���
				D2D1::Matrix3x2F ts = D2D1::Matrix3x2F::Translation(
					heroMove.x - heroSize.width,
					rtSize.height - heroSize.height
				);
				D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(-1.0f, 1.0f), heroMove);
				tCharacter1 = ts * scale; //���ʹ����� ���� �� �������Ѽ� �����̵��� �Ѵ�
			}
		}
		// ���� �׸���
		m_pRenderTarget->SetTransform(tCharacter1);
		if (isHit && isRun == 0) {
			//m_pRenderTarget->FillRectangle(&rcBrushRect, m_pCharacter1_Hit_BitmapBrush);
		}
		else {
			if (isRun == -1) { //���� ����Ű�� ��������
				m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[walkposition]); //walkposition : �ȱ�1~6�� Ű���带 ������ ������� ��Ÿ���� �Ѵ�
			}
			else if (isRun == 1) {//������ ����Ű�� ��������
				m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[walkposition]);
			}
			else if (isRun == 2) {//�Ʒ��� ����Ű�� ��������
				m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[6]);
			}
			//else if (isRun == 3) {//���� ALTŰ�� ��������
			//	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(heroMove.x, heroMove.y+50));
			//	m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[7]);
			//	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(heroMove.x, heroMove.y + 100));
			//	m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[8]);
			//	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(heroMove.x, heroMove.y + 50));
			//	m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[9]);
			//	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(heroMove.x, heroMove.y ));
			//	m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[0]);


			//}
			else { //������ ������
				m_pRenderTarget->FillRectangle(&rcBrushRect, m_pHero_Nomal_BitmapBrush[0]);
			}
		}



		// �׸��� ������� ������.
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

			case WM_GETMINMAXINFO: //������ â ũ�� ����
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
			/* ���Ű(t)�� �����̽��� ���� ������ */
			case WM_KEYDOWN:
			{
				input_str = (TCHAR)wParam;
				switch (input_str)
				{
				case VK_UP:
					break;
				case VK_DOWN:
					if ((heroMove.x > 0 && heroMove.x < 1400 - heroSize.width)) {
						isRun = 2;
					}
						break;
				case VK_LEFT:
					if (heroMove.x > 0) {
						isRight = false;
						heroMove.x -= 10;
						walkposition = (walkposition + 1) % 6;

					}
					isRun = -1;
					break;
				case VK_RIGHT:
					if (heroMove.x < 1400 - heroSize.width) {
						isRight = true;
						heroMove.x += 10;
						walkposition = (walkposition+1)%6;

					}
					isRun = 1;
					break;
				case VK_SPACE:
					if (isRun == 0 && !isHit) {
						isHit = !isHit;
					}
					break;
				case LVKF_ALT:
					if ((heroMove.x > 0 && heroMove.x < 1400 - heroSize.width)) {
						isRun = 3;
					}
					break;
				case 't':
				case 'T':
					//isShow = !isShow;
					break;
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

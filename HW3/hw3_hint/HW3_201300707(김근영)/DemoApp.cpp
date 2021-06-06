#include "DemoApp.h"
#include <time.h>
#include <vector>
#include <list>
#include <queue>
#include <iostream>
#include <stack>
#include <stdlib.h>
#include <typeinfo>
#include "Fly.h"
using namespace std;

/*---------------------------------	
��								
��	
��		��������
��	
��	
----------------------------------*/

D2D1_POINT_2F current; //���� ��ǥ ������
BOOL makeBug = FALSE;
BOOL _select = FALSE; 
BOOL _catch = FALSE;
INT n = 0;
WCHAR szText[100]; //�ؽ�Ʈ
list <Fly*> v;
//vector <Fly*> v; //���� ��ü�� ���� ����.
queue <Fly*> s;
int catch_num = 0; //���� ������
int current_bugNUm = 0;
int goldbug_catchNum = 0; //�����װ� ���� ��


#if defined(_DEBUG)&&defined(WIN32)&&!defined(_AFX)&&!defined(_AFXDLL)
#define TRACE TRACE_WIN32
#pragma warning(disable: 4996)
void TRACE_WIN32(LPCTSTR lpszFormat, ...) {
	TCHAR lpszBuffer[0x160]; //buffer size
	va_list fmtList;
	va_start(fmtList, lpszFormat);
	_vswprintf(lpszBuffer, lpszFormat, fmtList);
	va_end(fmtList);
	OutputDebugString(lpszBuffer);
}
#endif



int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;
			srand(time(NULL));
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
	m_hwnd(NULL), //������ �ڵ�
	m_pD2DFactory(NULL), //D2D ���丮
	m_pRT(NULL), //����Ÿ��
	m_pPathGeometry(NULL),
	m_pObjectGeometry(NULL),
	m_pPathBrush(NULL), //��θ� �׸� ��
	m_pGoldBrush(NULL),  //Ȳ�� ��
	m_Animation(), //�ִϸ��̼� ��ü
	m_flyBitmap(NULL), //�ĸ���Ʈ��
	m_FlyGeometry(), //�ĸ� �̵� ���
	m_pWICFactory(NULL),
	m_backgroundBitmap(NULL), //��� ��Ʈ��
	m_GoldBugMask(NULL),
	m_pTextFormat(NULL),
	m_pDWriteFactory(NULL),
	m_rainbowBitmap(NULL),
	m_EffectBitmap(NULL),
	m_pEffectBrush(NULL),
	m_pRectGeo(NULL)
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRT);
	SAFE_RELEASE(m_pPathGeometry);
	SAFE_RELEASE(m_pObjectGeometry);
	SAFE_RELEASE(m_pPathBrush);
	SAFE_RELEASE(m_pGoldBrush);
	SAFE_RELEASE(m_flyBitmap);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_backgroundBitmap);
	SAFE_RELEASE(m_GoldBugMask);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_EffectBitmap);
	SAFE_RELEASE(m_rainbowBitmap);
	SAFE_RELEASE(m_pEffectBrush);
	for (int i = 0; i<ARRAYSIZE(m_FlyGeometry); ++i)
			SAFE_RELEASE(m_FlyGeometry[i]);
}

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
	wcex.lpszClassName = L"D2DDemoApp";
	RegisterClassEx(&wcex);

	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"�������", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			840, 840, NULL, NULL, HINST_THISCOMPONENT, this);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			if (SUCCEEDED(hr))
			{
				ZeroMemory(&m_DwmTimingInfo, sizeof(m_DwmTimingInfo));
				m_DwmTimingInfo.cbSize = sizeof(m_DwmTimingInfo);
				hr = DwmGetCompositionTimingInfo(NULL, &m_DwmTimingInfo);

				if (FAILED(hr)) {
					HDC hdc = GetDC(m_hwnd);
					m_DwmTimingInfo.rateCompose.uiDenominator = 1;
					m_DwmTimingInfo.rateCompose.uiNumerator = GetDeviceCaps(hdc, VREFRESH);
					ReleaseDC(m_hwnd, hdc);
				}

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}
	}

	//QueryPerformanceFrequency(&m_nFrequency);
	//QueryPerformanceCounter(&m_nPrevTime);

	return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr;
	ID2D1GeometrySink* pSink = NULL;

	// D2D ���丮�� ������.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	// WIC ���丮�� ������.
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	}
	if (SUCCEEDED(hr))
	{
		/*Text�� ���� �ڿ�����*/
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
			m_pDWriteFactory->CreateTextFormat(L"Verdana", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20, L"", &m_pTextFormat);
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreateRectangleGeometry(D2D1::RectF(0, 0, 70, 70), &m_pRectGeo);
	}

	//���ĸ��� ���ƴٴϴ� ��α��� 1�� (���� ������ ���� ���)
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_FlyGeometry[0]); 
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink* pSink = NULL;
			hr = m_FlyGeometry[0]->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(D2D1::Point2F(346, 255), D2D1_FIGURE_BEGIN_FILLED);
				D2D1_POINT_2F points[10] = {
					D2D1::Point2F(267, 177),
					D2D1::Point2F(236, 192),
					D2D1::Point2F(212, 760),
					D2D1::Point2F(156, 255),
					D2D1::Point2F(346, 255),
					D2D1::Point2F(546, 135),
					D2D1::Point2F(26, 555),
					D2D1::Point2F(321, 55),
					D2D1::Point2F(746, 655),
					D2D1::Point2F(100, 455),
				};
				pSink->AddLines(points, ARRAYSIZE(points));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

			}
			hr = pSink->Close();

			SAFE_RELEASE(pSink);
		}
	}
	//���ĸ��� ���ƴٴϴ� ��α��� 2�� (������� ���)
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_FlyGeometry[1]); 
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink* pSink = NULL;

			hr = m_FlyGeometry[1]->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(D2D1::Point2F(0, 0), D2D1_FIGURE_BEGIN_FILLED);
				D2D1_POINT_2F points[] = {
					D2D1::Point2F(0,0),
					D2D1::Point2F(740, 0),
					D2D1::Point2F(0, 80),
					D2D1::Point2F(740, 80),
					D2D1::Point2F(0, 160),
					D2D1::Point2F(740, 160),
					D2D1::Point2F(0, 240),
					D2D1::Point2F(740, 240),
					D2D1::Point2F(0, 320),
					D2D1::Point2F(740, 320),
					D2D1::Point2F(0, 400),
					D2D1::Point2F(740, 400),
					D2D1::Point2F(0, 480),
					D2D1::Point2F(740, 480),
					D2D1::Point2F(0, 560),
					D2D1::Point2F(740, 560),
					D2D1::Point2F(0, 640),
					D2D1::Point2F(740, 640),
					D2D1::Point2F(0,640),
				};
				pSink->AddLines(points, ARRAYSIZE(points));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			}
			hr = pSink->Close();

			SAFE_RELEASE(pSink);
		}
	}
	//���ĸ��� ���ƴٴϴ� ��α��� 3�� (������ ����� ���)
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_FlyGeometry[2]);
		if (SUCCEEDED(hr))
		{
			hr = m_FlyGeometry[2]->Open(&pSink);
		}
		if (SUCCEEDED(hr))
		{
			D2D1_POINT_2F currentLocation = { 350, 300 };

			pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_FILLED);

			D2D1_POINT_2F locDelta = { 2, 2 };
			float radius = 3;

			for (UINT i = 0; i < 30; ++i)
			{
				currentLocation.x += radius * locDelta.x;
				currentLocation.y += radius * locDelta.y;

				pSink->AddArc(
					D2D1::ArcSegment(
						currentLocation,
						D2D1::SizeF(2 * radius, 2 * radius), // radiusx/y
						0.0f, // rotation angle
						D2D1_SWEEP_DIRECTION_CLOCKWISE,
						D2D1_ARC_SIZE_SMALL
					)
				);

				locDelta = D2D1::Point2F(-locDelta.y, locDelta.x);

				radius += 5;
			}
			D2D1_POINT_2F points[] = {
				D2D1::Point2F(350,300) };
			pSink->AddLines(points, ARRAYSIZE(points));
			pSink->EndFigure(D2D1_FIGURE_END_OPEN);

			hr = pSink->Close();
		}

		SAFE_RELEASE(pSink);
	}
	//���ĸ��� ���ƴٴϴ� ��α��� 4�� (������ ��� �̿��� ���)
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_FlyGeometry[3]); //���ĸ��� ���ƴٴϴ� ��α��� 1�� (���� ������ ���� ���)
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink* pSink = NULL;
			hr = m_FlyGeometry[3]->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(D2D1::Point2F(5, 5), D2D1_FIGURE_BEGIN_FILLED);
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(5, 5), D2D1::Point2F(12, 59), D2D1::Point2F(64, 57)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(64, 57), D2D1::Point2F(134, 57), D2D1::Point2F(64, 107)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(64, 107), D2D1::Point2F(34, 127), D2D1::Point2F(123, 157)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(123, 157), D2D1::Point2F(303, 232), D2D1::Point2F(143, 307)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(143, 307), D2D1::Point2F(25,359), D2D1::Point2F(183, 407)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(183, 407), D2D1::Point2F(503, 507), D2D1::Point2F(503, 117)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(503, 117), D2D1::Point2F(513, 0), D2D1::Point2F(353, 57)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(353, 57), D2D1::Point2F(303, 75), D2D1::Point2F(453, 157)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(453, 157), D2D1::Point2F(613, 232), D2D1::Point2F(503, 357)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(503, 357), D2D1::Point2F(413, 470), D2D1::Point2F(353, 257)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(353, 257), D2D1::Point2F(253, 32), D2D1::Point2F(653, 157)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(653, 157), D2D1::Point2F(823, 213), D2D1::Point2F(453, 507)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(453, 507), D2D1::Point2F(75, 770), D2D1::Point2F(253, 407)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(253, 407), D2D1::Point2F(403, 53), D2D1::Point2F(703, 357)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(703, 357), D2D1::Point2F(880, 537), D2D1::Point2F(803, 327)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(803, 327), D2D1::Point2F(643, -10), D2D1::Point2F(5, 5)));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

			}
			hr = pSink->Close();

			SAFE_RELEASE(pSink);
		}
	}
	//���ĸ��� ���ƴٴϴ� ��α��� 5�� (

	return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRT)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// ���� ��Ʈ�ʵ��� �ε���.
		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRT);

		hr = LoadBitmapFromResource(m_pRT, m_pWICFactory, L"FlyImage", L"Image", 100, 0, &m_flyBitmap); //�ĸ��̹���

		hr = LoadBitmapFromResource(m_pRT, m_pWICFactory, L"BackgroundImage3", L"Image", 0, 0, &m_backgroundBitmap); //����̹���

		hr = LoadBitmapFromResource(m_pRT, m_pWICFactory, L"BugMask", L"Image", 0, 0, &m_GoldBugMask); //Ȳ�ݹ��� �̹��� ����ũ

		hr = LoadBitmapFromResource(m_pRT, m_pWICFactory, L"RainbowImage", L"Image", 150, 150, &m_rainbowBitmap); //�������̹���

		hr = LoadBitmapFromResource(m_pRT, m_pWICFactory, L"EffectImage", L"Image", 200, 200, &m_EffectBitmap); //ȿ���̹���

		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		// ���� �׵��� ������.
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White,0.0f), &m_pPathBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gold), &m_pGoldBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateBitmapBrush(m_rainbowBitmap, propertiesXClampYClamp, &m_pBitmapBrush);
		}
		if(SUCCEEDED(hr))
		{
			hr = m_pRT->CreateBitmapBrush(m_EffectBitmap, propertiesXClampYClamp, &m_pEffectBrush);
		}
		if (SUCCEEDED(hr))
		{
			ID2D1GradientStopCollection* pGradientStops = NULL;

			static const D2D1_GRADIENT_STOP gradientStops[] =
			{
				{ 0.f,  D2D1::ColorF(D2D1::ColorF::Black, 1.0f) },
			{ 1.f,  D2D1::ColorF(D2D1::ColorF::White, 0.0f) },
			};

			hr = m_pRT->CreateGradientStopCollection(gradientStops, 2, &pGradientStops);

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateLinearGradientBrush(
					D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(150, 150)),
					pGradientStops, &m_pLinearGradientBrush);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateRadialGradientBrush(
					D2D1::RadialGradientBrushProperties(D2D1::Point2F(75, 75), D2D1::Point2F(0, 0), 75, 75),
					pGradientStops, &m_pRadialGradientBrush);
			}
			pGradientStops->Release();
		}
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRT);
	SAFE_RELEASE(m_pPathBrush);
	SAFE_RELEASE(m_pGoldBrush);
	SAFE_RELEASE(m_flyBitmap);
	SAFE_RELEASE(m_backgroundBitmap);
	SAFE_RELEASE(m_GoldBugMask);
	SAFE_RELEASE(m_pBitmapBrush);
	SAFE_RELEASE(m_rainbowBitmap);
	SAFE_RELEASE(m_EffectBitmap);
	SAFE_RELEASE(m_pEffectBrush);
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
	HRESULT hr;
	hr = CreateDeviceResources();
	if (SUCCEEDED(hr))
	{
		// �׸��⸦ �غ���.
		m_pRT->BeginDraw();

		// ��ȯ�� �׵���ķ� ������.
		m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

		m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
		m_pRT->DrawBitmap(m_backgroundBitmap, D2D1::RectF(840.0f, 840.0f));

		
		/*�ؽ�Ʈ ���*/
		_swprintf(szText, L"���� ������ �� : %d\n���� ������ �� : %d", current_bugNUm,catch_num);
		m_pRT->DrawTextW(szText, wcslen(szText), m_pTextFormat, D2D1::RectF(0, 0, 200, 100),m_pBitmapBrush);

		if (_select)
			GoldBug();
		/*������ ���� ������*/
		for (auto b_iter=v.begin(); b_iter!=v.end(); ++b_iter) {
			m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());
			FlyMove(b_iter);
		}
		hr = m_pRT->EndDraw();
	}

	return hr;
}

void DemoApp::GoldBug()
{
	D2D1_RECT_F rcBrushRect = D2D1::RectF(current.x - 25, current.y - 25, current.x + 25, current.y + 25);

	m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	m_pRT->FillOpacityMask(
		m_GoldBugMask,
		m_pBitmapBrush,
		D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
		&rcBrushRect);
	m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void DemoApp::FlyMove(list <Fly*>::iterator iter)
{
	HRESULT hr;
	D2D1_POINT_2F point;
	D2D1_POINT_2F tangent;
	D2D1_MATRIX_3X2_F triangleMatrix;
	D2D1_RECT_F rcBrushRect = D2D1::RectF(0,0,50,50);
	D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(0, 0);
	D2D1_SIZE_F rtSize = DemoApp::m_pRT->GetSize();
	float minWidthHeightScale = 1;
	D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(minWidthHeightScale, minWidthHeightScale);
	if ((*iter)->_isCatch == TRUE) {
		//m_pRT->SetTransform(D2D1::Matrix3x2F::Translation((*iter)->point.x-50, (*iter)->point.y-50));
		m_pRT->DrawBitmap(m_EffectBitmap,D2D1::RectF((*iter)->point.x-15, (*iter)->point.y-15, (*iter)->point.x + 15, (*iter)->point.y + 15));
		//m_pRT->FillGeometry(m_pRectGeo, m_pEffectBrush, m_pRadialGradientBrush);
	}
	else{
		float length = (*iter)->m_Animation.GetValue((*iter)->anim_time);
		// ���� �ð��� �ش��ϴ� ���� ���̿� ��ġ�ϴ� �̵� ���� ���� ������ ����.
		(*iter)->m_FlyGeometry->ComputePointAtLength(length, NULL, &point, &tangent);
		// ��ο� ���� �ĸ��� �������.
		triangleMatrix = D2D1::Matrix3x2F(
			tangent.x, tangent.y,
			-tangent.y, tangent.x,
			point.x, point.y);
		
		m_pRT->SetTransform(triangleMatrix * scale * translation);
		(*iter)->point.x = point.x; (*iter)->point.y = point.y;

		if ((*iter)->unigue) //������ �Ϲ� ������ ��
			m_pRT->DrawBitmap(m_flyBitmap, D2D1::RectF(20.0f, 20.0f));
		else { //������ �������� �� ������ ����ũ�� �̿��Ͽ� �罿���� ����� �������� ĥ�Ѵ�.
			m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
			m_pRT->FillOpacityMask(
				m_GoldBugMask,
				m_pGoldBrush,
				D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
				&rcBrushRect);
			m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		}

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}

		//�ִϸ��̼��� ���� �����ϸ� �ٽ� ó������ �ǵ����� �ݺ��ǵ��� ��.
		if ((*iter)->anim_time >= (*iter)->m_Animation.GetDuration())
		{
			(*iter)->anim_time = 0.0f;
		}
		else
		{
			float elapsedTime = 0.0; //DWM ����Ͽ� ����ð��� ����.
			elapsedTime += static_cast<float>(m_DwmTimingInfo.rateCompose.uiDenominator) /
				static_cast<float>(m_DwmTimingInfo.rateCompose.uiNumerator);
			(*iter)->anim_time += elapsedTime;
		}
	}
}

void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRT)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;

		m_pRT->Resize(size);
	}
}

VOID CALLBACK MakeFly(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if(current_bugNUm<30)
		makeBug = TRUE;
}

VOID CALLBACK SetGoldBug(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if (_select)
		_select = FALSE;
}

VOID CALLBACK eraseObject(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	for (auto i = v.begin(); i != v.end(); ++i) {
		if ((*i)->id == (int)lpParam) {
			v.erase(i);
			break;
		}
	}
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	HANDLE hTimer = NULL;
	HANDLE hTimer2 = NULL;
	HANDLE hTimer3 = NULL;
	HANDLE hTimerQueue = NULL;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;
		/*Ÿ�̸� ť�� ���� 5�ʸ��� ���� ����.*/
		hTimerQueue = CreateTimerQueue();
		CreateTimerQueueTimer(&hTimer, hTimerQueue,MakeFly, NULL, 5000, 5000, WT_EXECUTEDEFAULT);

		/*ù ���۽� ���� 1���� ����*/
		Fly *fly = new Fly(pDemoApp->m_FlyGeometry[0],rand()%10+4);
		fly->id = n++;
		++current_bugNUm;
		v.push_back(fly);

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		result = 1;
	}
	else
	{
		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

		bool wasHandled = false;

		if (pDemoApp)
		{
			switch(message)
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
					if (makeBug) { //Ÿ�̸Ӱ� �Լ��� ������ makeBug �÷��װ� on �϶��� ���� ����.
						Fly *fly = new Fly(pDemoApp->m_FlyGeometry[rand()%4], rand() % 10 + 30); //���� ������ duration�� ���� ������ �༭ �� �������� �ӵ��� �ٸ��� ����
						fly->id = n++;
						v.push_back(fly);
						++current_bugNUm;
						makeBug = false;
					}
					pDemoApp->OnRender();
					InvalidateRect(hwnd,NULL,FALSE);
					
				}
				result = 0;
				wasHandled = true;
				break;
			case WM_LBUTTONDOWN:
				{
					current.x = LOWORD(lParam); //���� ���콺 Ŭ���� ��ǥ�� ����
					current.y = HIWORD(lParam);

					/*�������� �κ�*/
					for (list<Fly*>::iterator i = v.begin(); i!=v.end();++i) { //��� ��ü�� ��ȸ�ϸ鼭 ������ǥ���� ��ġ�ϴ� ��ǥ�� ������ ��ü����.
						if ((*i)->unigue == 0) { //�����׸� �巡���Ͽ� ��������
							if (((*i)->point.x + 50 >= current.x&&(*i)->point.x - 50 <= current.x) && ((*i)->point.y + 50 >= current.y&&(*i)->point.y - 50 <= current.y)) {
								v.erase(i);
								--current_bugNUm;
								_select = TRUE;
								CreateTimerQueueTimer(&hTimer2, hTimerQueue, SetGoldBug, NULL, 2500, 0, WT_EXECUTEDEFAULT);
								break;
							}
						} //���콺 Ŭ������ �������� 
						else if (((*i)->point.x + 10 >= current.x&&(*i)->point.x - 10 <= current.x) && ((*i)->point.y + 10 >= current.y&&(*i)->point.y - 10 <= current.y)&&(*i)->_isCatch!=TRUE) {
							(*i)->_isCatch = TRUE;
							CreateTimerQueueTimer(&hTimer3, hTimerQueue, eraseObject, (PVOID)((*i)->id), 5000, 0, WT_EXECUTEONLYONCE);
							++catch_num;
							--current_bugNUm;
						}
					}
				}
				result = 0;
				wasHandled = true;
				break;
			case WM_MOUSEMOVE:
				{
					current.x = LOWORD(lParam); //���� ���콺 �̵� �� ��ǥ�� ����
					current.y = HIWORD(lParam);
					if (_select) {
						for (auto i = v.begin(); i !=v.end();++i) {
							if (((*i)->point.x + 25 >= current.x&&(*i)->point.x - 25 <= current.x) && ((*i)->point.y + 25 >= current.y&&(*i)->point.y - 25 <= current.y)&& (*i)->_isCatch != TRUE) {
								v.erase(i);
								++catch_num;
								--current_bugNUm;
								break;
							}
						}
					}
				}
				result = 0;
				wasHandled = true;
				break;
			case WM_DESTROY:
				{
					PostQuitMessage(0);
				}
				DeleteTimerQueue(hTimerQueue);
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
	HRESULT hr = imageResHandle ? S_OK : E_FAIL;
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
		hr = pStream->InitializeFromMemory(reinterpret_cast<BYTE*>(pImageFile), imageFileSize);
	}
	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);
	}
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create an IWICBitmapScaler and use it to resize the image.
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
					hr = pScaler->Initialize(pSource, destinationWidth, destinationHeight, WICBitmapInterpolationModeCubic);
					if (SUCCEEDED(hr))
					{
						hr = pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
					}
				}
			}
		}
		else
		{
			hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
		}
	}
	if (SUCCEEDED(hr))
	{
		//create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hr;
}


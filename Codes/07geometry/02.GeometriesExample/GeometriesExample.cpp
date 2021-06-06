#include "GeometriesExample.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;

			if (SUCCEEDED(app.Initialize(hInstance)))
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
	m_pRenderTarget(NULL),
	m_pBlackBrush(NULL),
	m_pGridPatternBitmapBrush(NULL),
	m_pPathGeometry(NULL),
	m_pPathGeometry1(NULL),
	m_pEllipseGeometry(NULL),
	m_pRectangleGeometry(NULL),
	m_pRoundedRectangleGeometry(NULL)
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
	SAFE_RELEASE(m_pPathGeometry);
	SAFE_RELEASE(m_pPathGeometry1);
	SAFE_RELEASE(m_pEllipseGeometry);
	SAFE_RELEASE(m_pRectangleGeometry);
	SAFE_RELEASE(m_pRoundedRectangleGeometry);
}

HRESULT DemoApp::Initialize(HINSTANCE hInstance)
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
		wcex.hInstance = hInstance;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"D2DDemoApp";
		RegisterClassEx(&wcex);

		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"Direct2D Demo Application",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			840, 500, NULL, NULL, hInstance, this);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	ID2D1GeometrySink* pSink = NULL;

	ID2D1GeometrySink* pSimplifiedSink = NULL;

	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(100.f, 60.f), 100.f, 50.f), &m_pEllipseGeometry);
	}

	// 선분 조각들로 구성된 경로 기하를 생성함.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry1);

		if (SUCCEEDED(hr))
		{
			// 경로 기하에 조각들을 기록하기 위해서 기하 싱크를 얻음.
			hr = m_pPathGeometry1->Open(&pSimplifiedSink);

			if (SUCCEEDED(hr))
			{
				pSimplifiedSink->BeginFigure(
					D2D1::Point2F(0, 0),
					D2D1_FIGURE_BEGIN_FILLED
					);

				D2D1_POINT_2F points[3] = {
					D2D1::Point2F(200, 0),
					D2D1::Point2F(300, 200),
					D2D1::Point2F(100, 200)
					};

				pSimplifiedSink->AddLines(points, 3);

				pSimplifiedSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSimplifiedSink->Close();
			}
		}
		SAFE_RELEASE(pSimplifiedSink);
	}

	// 여러 타입의 조각들이 혼합된 경로 기하를 생성함.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry);

		if (SUCCEEDED(hr))
		{
			// 경로 기하에 조각들을 기록하기 위해서 기하 싱크를 얻음.
			hr = m_pPathGeometry->Open(&pSink);

			if (SUCCEEDED(hr))
			{
				pSink->BeginFigure(
					D2D1::Point2F(10, 50),
					D2D1_FIGURE_BEGIN_FILLED
					);
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(100, 0),
						D2D1::Point2F(200, 200),
						D2D1::Point2F(300, 100))
					);

				pSink->AddLine(D2D1::Point2F(400, 100));

				pSink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(200, 100),
						D2D1::SizeF(50, 50),
						30.f,
						D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
						D2D1_ARC_SIZE_LARGE)
					);

				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(150, 50),
						D2D1::Point2F(100, 150),
						D2D1::Point2F(0, 0))
					);

				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				pSink->BeginFigure(
					D2D1::Point2F(10, 300),
					D2D1_FIGURE_BEGIN_FILLED
					);

				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(100, 250),
						D2D1::Point2F(200, 450),
						D2D1::Point2F(300, 350))
					);

				pSink->AddLine(D2D1::Point2F(400, 350));

				pSink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(200, 350),
						D2D1::SizeF(50, 50),
						30.f,
						D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
						D2D1_ARC_SIZE_LARGE)
					);

				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(150, 300),
						D2D1::Point2F(100, 400),
						D2D1::Point2F(0, 250))
					);

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSink->Close();
			}
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

		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
			);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.f), &m_pBlackBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
		}

	}
	return hr;
}

HRESULT DemoApp::CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, ID2D1BitmapBrush** ppBitmapBrush)
{
	HRESULT hr = S_OK;

	ID2D1BitmapRenderTarget* pCompatibleRenderTarget = NULL;
	hr = pRenderTarget->CreateCompatibleRenderTarget(D2D1::SizeF(10.0f, 10.0f), &pCompatibleRenderTarget);

	if (SUCCEEDED(hr))
	{
		ID2D1SolidColorBrush* pGridBrush = NULL;
		hr = pCompatibleRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f), &pGridBrush);

		if (SUCCEEDED(hr))
		{
			pCompatibleRenderTarget->BeginDraw();
			pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 10.0f, 1.0f), pGridBrush);
			pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 1.0f, 10.0f), pGridBrush);
			hr = pCompatibleRenderTarget->EndDraw();

			if (hr == D2DERR_RECREATE_TARGET)
			{
				DiscardDeviceResources();
			}
			if (SUCCEEDED(hr))
			{
				ID2D1Bitmap* pGridBitmap = NULL;
				hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);

				if (SUCCEEDED(hr))
				{
					hr = m_pRenderTarget->CreateBitmapBrush(
						pGridBitmap,
						D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP),
						ppBitmapBrush
						);

					pGridBitmap->Release();
				}
			}

			pGridBrush->Release();
		}

		pCompatibleRenderTarget->Release();
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
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
		m_pRenderTarget->BeginDraw();

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		m_pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height), m_pGridPatternBitmapBrush);

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(10, 10));
		m_pRenderTarget->DrawGeometry(m_pPathGeometry, m_pBlackBrush, 5.f, 0);

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(500, 65));
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(100.f, 60.f), 100.f, 50.f);
		m_pRenderTarget->DrawEllipse(ellipse, m_pBlackBrush, 5.f, 0);

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(500, 5));
		m_pRenderTarget->DrawGeometry(m_pEllipseGeometry, m_pBlackBrush, 5);

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(500, 200));
		m_pRenderTarget->DrawGeometry(m_pPathGeometry1, m_pBlackBrush, 5);

		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
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
	else
	{
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
				wasHandled = true;
				result = 0;
				break;

			case WM_DISPLAYCHANGE:
				{
					InvalidateRect(hwnd, NULL, FALSE);
				}
				wasHandled = true;
				result = 0;
				break;

			case WM_PAINT:
				{
					pDemoApp->OnRender();
					ValidateRect(hwnd, NULL);
				}
				wasHandled = true;
				result = 0;
				break;

			case WM_DESTROY:
				{
					PostQuitMessage(0);
				}
				wasHandled = true;
				result = 1;
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

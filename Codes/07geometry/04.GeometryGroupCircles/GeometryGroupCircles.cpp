#include "GeometryGroupCircles.h"

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
	m_pRenderTarget(NULL),
	m_pStrokeBrush(NULL),
	m_pFillBrush(NULL),
	m_pGridPatternBitmapBrush(NULL),
	m_pEllipseGeometry1(NULL),
	m_pEllipseGeometry2(NULL),
	m_pEllipseGeometry3(NULL),
	m_pEllipseGeometry4(NULL),
	m_pGeoGroup_AlternateFill(NULL),
	m_pGeoGroup_WindingFill(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL)
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pStrokeBrush);
	SAFE_RELEASE(m_pFillBrush);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
	SAFE_RELEASE(m_pEllipseGeometry1);
	SAFE_RELEASE(m_pEllipseGeometry2);
	SAFE_RELEASE(m_pEllipseGeometry3);
	SAFE_RELEASE(m_pEllipseGeometry4);
	SAFE_RELEASE(m_pGeoGroup_AlternateFill);
	SAFE_RELEASE(m_pGeoGroup_WindingFill);
	SAFE_RELEASE(m_pDWriteFactory);
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
		wcex.lpszClassName = L"D2DDemoApp";
		RegisterClassEx(&wcex);

		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"Direct2D Demo Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			640, 480, NULL, NULL, HINST_THISCOMPONENT, this);
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
	HRESULT hr;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
			);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			12.0f,
			L"en-us",
			&m_pTextFormat
			);
	}

	if (SUCCEEDED(hr))
	{
		hr = CreateGeometryResources();
	}

	return hr;
}

HRESULT DemoApp::CreateGeometryResources()
{
	HRESULT hr;

	const D2D1_ELLIPSE ellipse1 = D2D1::Ellipse(D2D1::Point2F(105.0f, 105.0f), 25.0f, 25.0f);

	hr = m_pD2DFactory->CreateEllipseGeometry(ellipse1, &m_pEllipseGeometry1);

	if (SUCCEEDED(hr))
	{
		const D2D1_ELLIPSE ellipse2 = D2D1::Ellipse(D2D1::Point2F(105.0f, 105.0f), 50.0f, 50.0f);
		hr = m_pD2DFactory->CreateEllipseGeometry(ellipse2, &m_pEllipseGeometry2);
	}

	if (SUCCEEDED(hr))
	{
		const D2D1_ELLIPSE ellipse3 = D2D1::Ellipse(D2D1::Point2F(105.0f, 105.0f), 75.0f, 75.0f);
		hr = m_pD2DFactory->CreateEllipseGeometry(ellipse3, &m_pEllipseGeometry3);
	}

	if (SUCCEEDED(hr))
	{
		const D2D1_ELLIPSE ellipse4 = D2D1::Ellipse(D2D1::Point2F(105.0f, 105.0f), 100.0f, 100.0f);
		hr = m_pD2DFactory->CreateEllipseGeometry(ellipse4, &m_pEllipseGeometry4);
	}

	if (SUCCEEDED(hr))
	{
		ID2D1Geometry* ppGeometries[] = { m_pEllipseGeometry1, m_pEllipseGeometry2, m_pEllipseGeometry3, m_pEllipseGeometry4 };

		hr = m_pD2DFactory->CreateGeometryGroup(D2D1_FILL_MODE_ALTERNATE, ppGeometries, ARRAYSIZE(ppGeometries), &m_pGeoGroup_AlternateFill);

		if (SUCCEEDED(hr))
		{
			hr = m_pD2DFactory->CreateGeometryGroup(D2D1_FILL_MODE_WINDING, ppGeometries, ARRAYSIZE(ppGeometries), &m_pGeoGroup_WindingFill);
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

		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.062f, 0.247f, 0.620f), &m_pStrokeBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.839f, 0.875f, 0.961f), &m_pFillBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
		}
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pStrokeBrush);
	SAFE_RELEASE(m_pFillBrush);
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
		static const WCHAR sc_fillModeAlternateText[] = L"D2D1_FILL_MODE_ALTERNATE";
		static const WCHAR sc_fillModeWindingText[] = L"D2D1_FILL_MODE_WINDING";

		m_pRenderTarget->BeginDraw();

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
		m_pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height), m_pGridPatternBitmapBrush);

		hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		if (SUCCEEDED(hr))
		{
			// 기하 그룹을 D2D1_FILL_MODE_ALTERNATE 채우기 모드로 채워 그림.
			m_pRenderTarget->FillGeometry(m_pGeoGroup_AlternateFill, m_pFillBrush);
			m_pRenderTarget->DrawGeometry(m_pGeoGroup_AlternateFill, m_pStrokeBrush, 1.0f);

			m_pRenderTarget->DrawText(sc_fillModeAlternateText, ARRAYSIZE(sc_fillModeAlternateText) - 1, m_pTextFormat, D2D1::RectF(5, 215, 205, 240), m_pStrokeBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(300, 0));

			// 기하 그룹을 D2D1_FILL_MODE_WINDING 채우기 모드로 채워 그림.
			m_pRenderTarget->FillGeometry(m_pGeoGroup_WindingFill, m_pFillBrush);
			m_pRenderTarget->DrawGeometry(m_pGeoGroup_WindingFill, m_pStrokeBrush, 1.0f);

			m_pRenderTarget->DrawText(sc_fillModeWindingText, ARRAYSIZE(sc_fillModeWindingText) - 1, m_pTextFormat, D2D1::RectF(5, 215, 205, 240), m_pStrokeBrush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);

			hr = m_pRenderTarget->EndDraw();

			if (hr == D2DERR_RECREATE_TARGET)
			{
				hr = S_OK;
				DiscardDeviceResources();
			}
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

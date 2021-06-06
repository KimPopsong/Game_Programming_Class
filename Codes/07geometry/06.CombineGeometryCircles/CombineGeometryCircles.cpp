#include "CombineGeometryCircles.h"

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
	m_pOutlineBrush(NULL),
	m_pTextBrush(NULL),
	m_pShapeFillBrush(NULL),
	m_pGridPatternBitmapBrush(NULL),
	m_pCircleGeometry1(NULL),
	m_pCircleGeometry2(NULL),
	m_pPathGeometryUnion(NULL),
	m_pPathGeometryIntersect(NULL),
	m_pPathGeometryXOR(NULL),
	m_pPathGeometryExclude(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL),
	m_pStrokeStyle(NULL)
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pOutlineBrush);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pShapeFillBrush);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
	SAFE_RELEASE(m_pCircleGeometry1);
	SAFE_RELEASE(m_pCircleGeometry2);
	SAFE_RELEASE(m_pPathGeometryUnion);
	SAFE_RELEASE(m_pPathGeometryIntersect);
	SAFE_RELEASE(m_pPathGeometryXOR);
	SAFE_RELEASE(m_pPathGeometryExclude);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pStrokeStyle);
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
			10.0f,
			L"en-us",
			&m_pTextFormat
			);
	}

	if (SUCCEEDED(hr))
	{
		hr = CreateGeometryResources();
	}

	if (SUCCEEDED(hr))
	{
		float dashes[] = {1.f, 1.f, 2.f, 3.f, 5.f};

		m_pD2DFactory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_ROUND,
				10.f,
				D2D1_DASH_STYLE_CUSTOM,
				0.f
				),
			 dashes,
			 ARRAYSIZE(dashes)-1,
			 &m_pStrokeStyle
			 );
	}

	return hr;
}

HRESULT DemoApp::CreateGeometryResources()
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink* pGeometrySink = NULL;

	//  첫 번째 기하를 생성함.
	const D2D1_ELLIPSE circle1 = D2D1::Ellipse(D2D1::Point2F(75.0f, 75.0f), 50.0f, 50.0f);
	hr = m_pD2DFactory->CreateEllipseGeometry(circle1, &m_pCircleGeometry1);

	// 두 번째 기하를 생성함.
	if (SUCCEEDED(hr))
	{
		const D2D1_ELLIPSE circle2 = D2D1::Ellipse(D2D1::Point2F(125.0f, 75.0f), 50.0f, 50.0f);
		hr = m_pD2DFactory->CreateEllipseGeometry(circle2, &m_pCircleGeometry2);
	}

	// 두 기하를 결합함. 결합 옵션으로 D2D1_COMBINE_MODE_UNION을 사용함.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometryUnion);
		if (SUCCEEDED(hr))
		{
			hr = m_pPathGeometryUnion->Open(&pGeometrySink);
			if (SUCCEEDED(hr))
			{
				hr = m_pCircleGeometry1->CombineWithGeometry(m_pCircleGeometry2, D2D1_COMBINE_MODE_UNION, NULL, NULL, pGeometrySink);
			}
			if (SUCCEEDED(hr))
			{
				hr = pGeometrySink->Close();
			}

			SAFE_RELEASE(pGeometrySink);
		}
	}

	// 두 기하를 결합함. 결합 옵션으로 D2D1_COMBINE_MODE_INTERSECT을 사용함.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometryIntersect);
		if (SUCCEEDED(hr))
		{
			hr = m_pPathGeometryIntersect->Open(&pGeometrySink);
			if (SUCCEEDED(hr))
			{
				hr = m_pCircleGeometry1->CombineWithGeometry(m_pCircleGeometry2, D2D1_COMBINE_MODE_INTERSECT, NULL, NULL, pGeometrySink);
			}
			if (SUCCEEDED(hr))
			{
				hr = pGeometrySink->Close();
			}

			SAFE_RELEASE(pGeometrySink);
		}
	}

	// 두 기하를 결합함. 결합 옵션으로 D2D1_COMBINE_MODE_XOR을 사용함.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometryXOR);
		if (SUCCEEDED(hr))
		{
			hr = m_pPathGeometryXOR->Open(&pGeometrySink);
			if (SUCCEEDED(hr))
			{
				hr = m_pCircleGeometry1->CombineWithGeometry(m_pCircleGeometry2, D2D1_COMBINE_MODE_XOR, NULL, NULL, pGeometrySink);
			}
			if (SUCCEEDED(hr))
			{
				hr = pGeometrySink->Close();
			}

			SAFE_RELEASE(pGeometrySink);
		}
	}

	// 두 기하를 결합함. 결합 옵션으로 D2D1_COMBINE_MODE_EXCLUDE을 사용함.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometryExclude);
		if (SUCCEEDED(hr))
		{
			hr = m_pPathGeometryExclude->Open(&pGeometrySink);
			if (SUCCEEDED(hr))
			{
				hr = m_pCircleGeometry1->CombineWithGeometry(m_pCircleGeometry2, D2D1_COMBINE_MODE_EXCLUDE, NULL, NULL, pGeometrySink);
			}
			if (SUCCEEDED(hr))
			{
				hr = pGeometrySink->Close();
			}
			SAFE_RELEASE(pGeometrySink);
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
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkSlateBlue, 1.f), &m_pOutlineBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue, 0.5f), &m_pShapeFillBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.f), &m_pTextBrush);
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

			ID2D1Bitmap* pGridBitmap = NULL;
			hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);
			if (SUCCEEDED(hr))
			{
				D2D1_BITMAP_BRUSH_PROPERTIES brushProperties = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP);

				hr = m_pRenderTarget->CreateBitmapBrush(pGridBitmap, brushProperties, ppBitmapBrush);

				pGridBitmap->Release();
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
	SAFE_RELEASE(m_pOutlineBrush);
	SAFE_RELEASE(m_pShapeFillBrush);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
	SAFE_RELEASE(m_pTextBrush);
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

// 여러 결합 옵션으로 두 기하를 결합하여 결합된 기하를 생성하고 이를 보여줌.
void DemoApp::RenderCombinedGeometry()
{
	static const WCHAR szBeforeText[] = L"The circles before combining";
	static const WCHAR szUnionText[] = L"D2D1_COMBINE_MODE_UNION";
	static const WCHAR szIntersectText[] = L"D2D1_COMBINE_MODE_INTERSECT";
	static const WCHAR szXorText[] = L"D2D1_COMBINE_MODE_XOR";
	static const WCHAR szExcludeText[] = L"D2D1_COMBINE_MODE_EXCLUDE";

	// 결합 전의 기하들을 그림.

	m_pRenderTarget->FillGeometry(m_pCircleGeometry1, m_pShapeFillBrush);
	m_pRenderTarget->DrawGeometry(m_pCircleGeometry1, m_pOutlineBrush, 1.0f);
	m_pRenderTarget->FillGeometry(m_pCircleGeometry2, m_pShapeFillBrush);
	m_pRenderTarget->DrawGeometry(m_pCircleGeometry2, m_pOutlineBrush, 1.0f);

	m_pRenderTarget->DrawText(szBeforeText, ARRAYSIZE(szBeforeText) - 1, m_pTextFormat, D2D1::RectF(25.0f, 130.0f, 200.0f, 300.0f), m_pTextBrush);

	// UNION 결합 모드로 결환된 기하를 그림.

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(200, 0));

	m_pRenderTarget->FillGeometry(m_pPathGeometryUnion, m_pShapeFillBrush, NULL);
	m_pRenderTarget->DrawGeometry(m_pPathGeometryUnion, m_pOutlineBrush, 1.0f);

	m_pRenderTarget->DrawText(szUnionText, ARRAYSIZE(szUnionText) - 1, m_pTextFormat, D2D1::RectF(25, 130, 200, 300), m_pTextBrush);

	// INTERSECT 결합 모드로 결환된 기하를 그림.

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(400, 0));

	m_pRenderTarget->FillGeometry(m_pPathGeometryIntersect, m_pShapeFillBrush, NULL);
	m_pRenderTarget->DrawGeometry(m_pPathGeometryIntersect, m_pOutlineBrush, 1.0f);

	m_pRenderTarget->DrawText(szIntersectText, ARRAYSIZE(szIntersectText) - 1, m_pTextFormat, D2D1::RectF(25, 130, 400, 300), m_pTextBrush);

	// XOR 결합 모드로 결환된 기하를 그림.

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(200, 150));

	m_pRenderTarget->FillGeometry(m_pPathGeometryXOR, m_pShapeFillBrush, NULL);
	m_pRenderTarget->DrawGeometry(m_pPathGeometryXOR, m_pOutlineBrush, 1.0f);

	m_pRenderTarget->DrawText(szXorText, ARRAYSIZE(szXorText) - 1, m_pTextFormat, D2D1::RectF(25, 130, 200, 470), m_pTextBrush);

	// EXCLUDE 결합 모드로 결환된 기하를 그림.

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(400, 150));

	m_pRenderTarget->FillGeometry(m_pPathGeometryExclude, m_pShapeFillBrush, NULL);
	m_pRenderTarget->DrawGeometry(m_pPathGeometryExclude, m_pOutlineBrush, 1.0f);

	m_pRenderTarget->DrawText(szExcludeText, ARRAYSIZE(szExcludeText) - 1, m_pTextFormat, D2D1::RectF(25, 130, 400, 470), m_pTextBrush);
}

// 기하 인터페이스의 함수들을 사용하는 예제를 보여줌.
void DemoApp::UseGeometryFunctions()
{
	// 함수 CompareWithGeometry의 사용 예시.

	D2D1_GEOMETRY_RELATION result = D2D1_GEOMETRY_RELATION_UNKNOWN;

	// Compare circle1 with circle2
	HRESULT hr = m_pCircleGeometry1->CompareWithGeometry(m_pCircleGeometry2, D2D1::IdentityMatrix(), 0.1f, &result);

	if (SUCCEEDED(hr))
	{
		static const WCHAR szGeometryRelation[] = L"Two circles overlap.";
		m_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
		if (result == D2D1_GEOMETRY_RELATION_OVERLAP)
		{
			m_pRenderTarget->DrawText(szGeometryRelation, ARRAYSIZE(szGeometryRelation) - 1, m_pTextFormat, D2D1::RectF(25.0f, 160.0f, 200.0f, 300.0f), m_pTextBrush);
		}
	}

	// 함수 ComputeArea의 사용 예시.

	float area;
	// Compute the area of circle1
	hr = m_pCircleGeometry1->ComputeArea(D2D1::IdentityMatrix(), &area);

	// 함수 ComputeLength의 사용 예시.

	float length;
	// Compute the area of circle1
	hr = m_pCircleGeometry1->ComputeLength(D2D1::IdentityMatrix(), &length);
	if (SUCCEEDED(hr))
	{
		// Process the length of the geometry.
	}

	// 함수 ComputePointAtLength의 사용 예시.

	D2D1_POINT_2F point;
	D2D1_POINT_2F tangent;

	// Ask the geometry to give us the point that corresponds with the length 10 at the current time.
	hr = m_pCircleGeometry1->ComputePointAtLength(10, NULL, &point, &tangent);

	if (SUCCEEDED(hr))
	{
		// Retrieve the point and tangent point.
	}

	// 함수 GetBounds의 사용 예시.

	D2D1_RECT_F bounds;
	hr = m_pCircleGeometry1->GetBounds(D2D1::IdentityMatrix(), &bounds);
	if (SUCCEEDED(hr))
	{
		// Retrieve the bounds.
	}

	// 함수 GetWidenedBounds의 사용 예시.

	D2D1_RECT_F bounds1;
	hr = m_pCircleGeometry1->GetWidenedBounds(5.0, m_pStrokeStyle, D2D1::IdentityMatrix(), &bounds1);
	if (SUCCEEDED(hr))
	{
		// Retrieve the widened bounds.
	}

	// 함수 StrokeContainsPoint의 사용 예시.

	BOOL containsPoint;
	hr = m_pCircleGeometry1->StrokeContainsPoint(D2D1::Point2F(0, 0), 10, NULL, NULL, &containsPoint);
	if (SUCCEEDED(hr))
	{
		// Process containsPoint.
	}

	// 함수 FillContainsPoint의 사용 예시.

	BOOL containsPoint1;
	hr = m_pCircleGeometry1->FillContainsPoint(D2D1::Point2F(0, 0), D2D1::Matrix3x2F::Identity(), &containsPoint1);
	if (SUCCEEDED(hr))
	{
		// Process containsPoint.
	}

}

HRESULT DemoApp::OnRender()
{
	HRESULT hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		// 격자 패턴을 배경으로 그림.

		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

		m_pRenderTarget->FillRectangle(D2D1::RectF(0.f, 0.f, renderTargetSize.width, renderTargetSize.height), m_pGridPatternBitmapBrush);

		// 기하 결합을 보여줌.
		RenderCombinedGeometry();

		// 기하 함수들의 사용을 보여줌.
		UseGeometryFunctions();

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

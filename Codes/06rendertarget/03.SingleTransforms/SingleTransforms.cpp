#include "SingleTransforms.h"

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
	m_pStrokeStyleDash(NULL),
	m_pGridPatternBitmapBrush(NULL),
	m_pOriginalShapeBrush(NULL),
	m_pTransformedShapeBrush(NULL),
	m_pFillBrush(NULL),
	m_pTextBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL)
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pStrokeStyleDash);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
	SAFE_RELEASE(m_pOriginalShapeBrush);
	SAFE_RELEASE(m_pTransformedShapeBrush);
	SAFE_RELEASE(m_pFillBrush);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
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
			640, 480, NULL, NULL, hInstance, this
			);
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
	HRESULT hr = S_OK;

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
			13.0f,
			L"en-us",
			&m_pTextFormat
			);
	}

	if (SUCCEEDED(hr))
	{
		// ���� ȹ ��Ÿ�� ��ü�� ������.
		float dashes[] = {10.0f, 2.0f};
		hr = m_pD2DFactory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_CUSTOM,
				0.0f),
			dashes,
			ARRAYSIZE(dashes),
			&m_pStrokeStyleDash
			);
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
			// ��ȯ �� ����� �ܰ����� ���� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &m_pOriginalShapeBrush);
		}

		if (SUCCEEDED(hr))
		{
			// ��ȯ �� ������ ä��� �뵵�� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pFillBrush);
		}

		if (SUCCEEDED(hr))
		{
			// �ؽ�Ʈ�� ���� ���� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrush);
		}

		if (SUCCEEDED(hr))
		{
			// ��ȯ �� ����� �ܰ����� ���� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &m_pTransformedShapeBrush);
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

	// ȣȯ ����Ÿ���� ����.
	ID2D1BitmapRenderTarget* pCompatibleRenderTarget = NULL;
	hr = pRenderTarget->CreateCompatibleRenderTarget(D2D1::SizeF(10.0f, 10.0f), &pCompatibleRenderTarget);

	if (SUCCEEDED(hr))
	{
		// ������ �׸�.
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
				// ��� ��ġ ������ �ڿ����� �ݳ�.
				DiscardDeviceResources();
			}
			if (SUCCEEDED(hr))
			{
				// ����Ÿ�����κ��� ��Ʈ���� ����.
				ID2D1Bitmap* pGridBitmap = NULL;
				hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);

				if (SUCCEEDED(hr))
				{
					// ��Ʈ�� ���� ������.
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
	SAFE_RELEASE(m_pTransformedShapeBrush);
	SAFE_RELEASE(m_pFillBrush);
	SAFE_RELEASE(m_pOriginalShapeBrush);
	SAFE_RELEASE(m_pTextBrush);
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
		// �׸��⸦ ������.
		m_pRenderTarget->BeginDraw();

		// ����Ÿ�� ��ȯ�� �׵� ��ȯ���� ������.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// ����Ÿ�� ������ Ŭ������.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		m_pRenderTarget->FillRectangle(
			D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height),
			m_pGridPatternBitmapBrush);

		m_pOriginalShapeBrush->SetOpacity(0.5);

		m_pFillBrush->SetOpacity(0.5f);

		DrawAndTranslateARectangle();
		DrawAndRotateARectangle();
		DrawAndScaleARectangle();
		DrawAndSkewARectangle();

		// �׸��� ������� ������.
		m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}

// �̵� ��ȯ�� ����Ÿ�ٿ� �����ϰ� �׸�.
void DemoApp::DrawAndTranslateARectangle()
{
	D2D1_RECT_F rectangle = D2D1::Rect(126.0f, 80.5f, 186.0f, 140.5f);

	// ����Ÿ�� ��ȯ�� �׵� ��ȯ���� ������. �簢���� �׸�.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	m_pRenderTarget->DrawRectangle(rectangle, m_pOriginalShapeBrush, 1.0f, m_pStrokeStyleDash);

	// �̵� ��ȯ�� ����Ÿ�ٿ� �����ϰ� �簢���� ä�� �׸�.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(20, 10));
	m_pRenderTarget->FillRectangle(rectangle, m_pFillBrush);
	m_pRenderTarget->DrawRectangle(rectangle, m_pTransformedShapeBrush);

	// ĸ�� �ؽ�Ʈ�� ǥ����.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	static const WCHAR szText[] = L"Translate";
	m_pRenderTarget->DrawText(szText, ARRAYSIZE(szText) - 1, m_pTextFormat, D2D1::RectF(126.0f, 170.5f, 236.0f, 190.5f), m_pTextBrush);
}

// ȸ�� ��ȯ�� ����Ÿ�ٿ� �����ϰ� �׸�.
void DemoApp::DrawAndRotateARectangle()
{
	D2D1_RECT_F rectangle = D2D1::Rect(438.0f, 301.5f, 498.0f, 361.5f);

	// ����Ÿ�� ��ȯ�� �׵� ��ȯ���� ������. �簢���� �׸�.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	m_pRenderTarget->DrawRectangle(rectangle, m_pOriginalShapeBrush, 1.0f, m_pStrokeStyleDash);

	// ȸ�� ��ȯ�� ����Ÿ�ٿ� �����ϰ� �簢���� ä�� �׸�.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(45.0f, D2D1::Point2F(468.0f, 331.5f)));
	m_pRenderTarget->FillRectangle(rectangle, m_pFillBrush);
	m_pRenderTarget->DrawRectangle(rectangle, m_pTransformedShapeBrush);

	// ĸ�� �ؽ�Ʈ�� ǥ����.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	static const WCHAR szText[] = L"Rotate";
	m_pRenderTarget->DrawText(szText, ARRAYSIZE(szText) - 1, m_pTextFormat, D2D1::RectF(438.0f, 391.5f, 498.0f, 411.5f), m_pTextBrush);
}

// ũ������ ��ȯ�� �����ؼ� ����Ÿ�ٿ� �����ϰ� �׸�.
void DemoApp::DrawAndScaleARectangle()
{
	D2D1_RECT_F rectangle = D2D1::Rect(438.0f, 80.5f, 498.0f, 140.5f);

	// ����Ÿ�� ��ȯ�� �׵� ��ȯ���� ������. �簢���� �׸�.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	m_pRenderTarget->DrawRectangle(rectangle, m_pOriginalShapeBrush, 1.0f, m_pStrokeStyleDash);

	// ũ������ ��ȯ�� ����Ÿ�ٿ� �����ϰ� �簢���� ä�� �׸�.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(D2D1::Size(1.3f, 1.3f), D2D1::Point2F(438.0f, 80.5f)));
	m_pRenderTarget->FillRectangle(rectangle, m_pFillBrush);
	m_pRenderTarget->DrawRectangle(rectangle, m_pTransformedShapeBrush);

	// �ؽ�Ʈ�� ǥ����.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	static const WCHAR szText[] = L"Scale";
	m_pRenderTarget->DrawText(szText, ARRAYSIZE(szText) - 1, m_pTextFormat, D2D1::RectF(438.0f, 170.5f, 498.0f, 190.5f), m_pTextBrush);
}

// ����� ��ȯ�� �����ؼ� ����Ÿ�ٿ� �����ϰ� �׸�.
void DemoApp::DrawAndSkewARectangle()
{
	D2D1_RECT_F rectangle = D2D1::Rect(126.0f, 301.5f, 186.0f, 361.5f);

	// ����Ÿ�� ��ȯ�� �׵� ��ȯ���� ������. �簢���� �׸�.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	m_pRenderTarget->DrawRectangle(rectangle, m_pOriginalShapeBrush, 1.0f, m_pStrokeStyleDash);

	// ����� ��ȯ�� ����Ÿ�ٿ� �����ϰ� �簢���� ä�� �׸�.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Skew(45.0f, 0.0f, D2D1::Point2F(126.0f, 301.5f)));
	m_pRenderTarget->FillRectangle(rectangle, m_pFillBrush);
	m_pRenderTarget->DrawRectangle(rectangle, m_pTransformedShapeBrush);

	// ĸ�� �ؽ�Ʈ�� ǥ����.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	static const WCHAR szText[] = L"Skew";
	m_pRenderTarget->DrawText(szText, ARRAYSIZE(szText) - 1, m_pTextFormat, D2D1::RectF(126.0f, 391.5f, 186.0f, 411.5f), m_pTextBrush);
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
	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		return 1;
	}

	DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

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
			return 0;

		case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;

		case WM_PAINT:
			{
				pDemoApp->OnRender();
				ValidateRect(hwnd, NULL);
			}
			return 0;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			return 1;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

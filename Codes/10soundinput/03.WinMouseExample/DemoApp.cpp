#include "DemoApp.h"

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
	m_pDWriteFactory(NULL),
	m_pRenderTarget(NULL),
	m_pTextFormat(NULL),
	m_pBlackBrush(NULL)
{

}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pBlackBrush);

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

		m_hwnd = CreateWindow(L"D2DDemoApp", L"Direct2D Demo Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, this);
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
	static const WCHAR msc_fontName[] = L"Verdana";
	static const FLOAT msc_fontSize = 20;
	HRESULT hr;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteFactory->CreateTextFormat(msc_fontName, NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, msc_fontSize, L"", &m_pTextFormat);
	}
	if (SUCCEEDED(hr))
	{
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
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
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);
		}
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
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
	static int i = 0;

	WCHAR strbuf[180];
	int strbuf_len;

	if (SUCCEEDED(hr))
	{
		D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));


		// 스크린 좌표계로 마우스 위치를 표시한다.

		wsprintf(strbuf, L"[screen coord]:\n X: %4d, Y: %4d", m_mousePosScreen.x, m_mousePosScreen.y);
		strbuf_len = wcslen(strbuf);
		m_pRenderTarget->DrawText(strbuf, strbuf_len, m_pTextFormat, D2D1::RectF(0, 0, renderTargetSize.width / 2, renderTargetSize.height/2), m_pBlackBrush);


		// 클라이언트 좌표계로 마우스 위치를 표시한다.

		POINT m_mousePosClient = m_mousePosScreen;
		ScreenToClient(m_hwnd, &m_mousePosClient); // 해당 클라이언트 기준으로 좌표를 변환한다
		wsprintf(strbuf, L"[client coord]:\n X: %4d, Y: %4d", m_mousePosClient.x, m_mousePosClient.y);
		strbuf_len = wcslen(strbuf);
		m_pRenderTarget->DrawText(strbuf, strbuf_len, m_pTextFormat, D2D1::RectF(renderTargetSize.width / 2, 0, renderTargetSize.width, renderTargetSize.height / 2), m_pBlackBrush);

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
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;

		m_pRenderTarget->Resize(size);
	}
}

LRESULT CALLBACK DemoApp::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	if (iMessage == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

		SetWindowLongPtrW(hWnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		SetTimer(hWnd, 1, /*0*/ 0, NULL); // 현재 상황을 알기 위한 타이머 설정

		GetCursorPos(&pDemoApp->m_mousePosScreen); //초기 마우스 위치를 받아온다.

		return 1;
	}
	else
	{
		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hWnd, GWLP_USERDATA)));

		if (pDemoApp)
		{
			switch (iMessage)
			{
			case WM_TIMER:
			{
				pDemoApp->OnRender();
				return 0;
			}

			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pDemoApp->OnResize(width, height);
				return 0;
			}

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hWnd, NULL, FALSE);
				return 0;
			}

			case WM_MOUSEMOVE:
			{
				GetCursorPos(&pDemoApp->m_mousePosScreen); // 마우스 위치를 받아온다
				//InvalidateRect(hWnd, NULL, FALSE);
				return 0;
			}

			case WM_LBUTTONDOWN:
			{
				POINT pos;
				pos.x = 300; // 윈도우 클라이언트 좌표계 기준으로 (300, 300)으로 설정
				pos.y = 300;
				ClientToScreen(hWnd, &pos); // 클라이언트 기준 좌표를 바탕화면 기준으로 변환한다
				SetCursorPos(pos.x, pos.y); // 커서를 윈도우 클라이언트 기준으로 (300, 300)에 위치시킨다
				//InvalidateRect(hWnd, NULL, FALSE);
				return 0;
			}
			case WM_RBUTTONDOWN:
			{
				POINT pos;
				pos.x = 300; // 윈도우 클라이언트 좌표계 기준으로 (300, 300)으로 설정
				pos.y = 300;
				SetCursorPos(pos.x, pos.y); // 커서를 스크린 기준으로 (300, 300)에 위치시킨다
				//InvalidateRect(hWnd, NULL, FALSE);
				return 0;
			}

			case WM_PAINT:
			{
				pDemoApp->OnRender();
				// 윈도우가 반복해서 화면을 그리도록 하려면 ValidateRect을 호출하지 말아야 함. 단, WM_TIMER로 주기적으로 호출하도록 하면 상관없음.
				ValidateRect(hWnd, NULL);
				return 0;
			}

			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 1;
			}

			}
		}
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}
}

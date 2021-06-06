#include "DemoApp.h"

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
	m_pBitmap(NULL),
	m_pRenderTarget(NULL),
	m_pBlackBrush(NULL),
	m_pYellowGreenBrush(NULL),
	m_pLinearGradientBrush(NULL),
	m_pRadialGradientBrush(NULL),
	m_pBitmapBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL)
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(m_pYellowGreenBrush);
	SAFE_RELEASE(m_pLinearGradientBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pBitmapBrush);
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

	// WIC 팩토리를 생성함.
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));

	if (SUCCEEDED(hr))
	{
		// Direct2D 팩토리를 생성함.
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	}

	if (SUCCEEDED(hr))
	{
		// DirectWrite 팩토리를 생성함.
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	}

	if (SUCCEEDED(hr))
	{
		// DirectWrite 텍스트 포맷 객체를 생성함.
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana", //font family name
			NULL, //font collection (NULL sets it to use the system font collection)
			DWRITE_FONT_WEIGHT_BOLD,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			10.5f,
			L"en-us",
			&m_pTextFormat
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

		// Direct2D 렌더타겟을 생성함.
		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &m_pBlackBrush);
		}

		// 연두색 붓을 생성함.
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x9ACD32, 1.0f), &m_pYellowGreenBrush);
		}

		if (SUCCEEDED(hr))
		{
			// 계조 명세 지점 구조체인 D2D1_GRADIENT_STOP 구조체의 배열을 생성.
			ID2D1GradientStopCollection* pGradientStops = NULL;

			D2D1_GRADIENT_STOP gradientStops[2];
			gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow, 1);
			gradientStops[0].position = 0.0f;
			gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::ForestGreen, 1);
			gradientStops[1].position = 1.0f;
			// D2D1_GRADIENT_STOP 구조체의 배열로부터 ID2D1GradientStopCollection 객체 생성.
			hr = m_pRenderTarget->CreateGradientStopCollection(gradientStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pGradientStops);

			// ID2D1GradientStopCollection 객체로부터 선형 계조 붓 생성. 계조 축은 사각형의 왼쪽 상단 모서리에서 시작해서 오른쪽 하단 모서리에서 끝남.
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(150, 150)), pGradientStops, &m_pLinearGradientBrush);
			}

			// ID2D1GradientStopCollection 객체로부터 방사형 계조 붓 생성. 계조의 중심은 상자의 중심이며 계조 원점의 오프셋은 (0,0)임.
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(75, 75), D2D1::Point2F(0, 0), 75, 75), pGradientStops, &m_pRadialGradientBrush);
			}

			SAFE_RELEASE(pGradientStops);
		}

		// 비트맵 붓으로 사용될 비트맵 객체를 생성함.
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"FERN", L"Image", &m_pBitmap);
		}
		// 비트맵 붓을 생성함.
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(m_pBitmap, &m_pBitmapBrush);
		}
	}
	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(m_pYellowGreenBrush);
	SAFE_RELEASE(m_pLinearGradientBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pBitmapBrush);

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
		static const WCHAR szSolidBrushText[] = L"ID2D1SolidColorBrush";
		static const WCHAR szBitmapBrushText[] = L"ID2D1BitmapBrush";
		static const WCHAR szLinearGradientText[] = L"ID2D1LinearGradientBrush";
		static const WCHAR szRadialGradientText[] = L"ID2D1RadialGradientBrush";

		m_pRenderTarget->BeginDraw();

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// 칠할 사각형 모양을 정의함.
		D2D1_RECT_F rcBrushRect = D2D1::RectF(5, 0, 150, 150);
 
		// 캡션 텍스트가 그려질 영역을 정의함.
		D2D1_RECT_F rcTextRect = D2D1::RectF(5, 165, 175, 200);
   
		// 배경을 흰색으로 클리어함.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		// 단색 붓 데모.

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(5, 5)));

		m_pRenderTarget->FillRectangle(&rcBrushRect, m_pYellowGreenBrush);
		m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);

		m_pRenderTarget->DrawText(szSolidBrushText, ARRAYSIZE(szSolidBrushText) - 1, m_pTextFormat, &rcTextRect, m_pBlackBrush);

		// 선형 계조 붓 데모.

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(200, 5)));

		m_pRenderTarget->FillRectangle(&rcBrushRect, m_pLinearGradientBrush);
		m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);

		m_pRenderTarget->DrawText(szLinearGradientText, ARRAYSIZE(szLinearGradientText) - 1, m_pTextFormat, &rcTextRect, m_pBlackBrush);
   
		// 방사형 계조 붓 데모.

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(5, 200)));

		D2D1_ELLIPSE   ellipse = D2D1::Ellipse(D2D1::Point2F(75.f, 75.f), 75.f, 75.f);
		m_pRenderTarget->FillEllipse(ellipse, m_pRadialGradientBrush);
		m_pRenderTarget->DrawEllipse(ellipse, m_pBlackBrush, 1, NULL);

		m_pRenderTarget->DrawText(szRadialGradientText, ARRAYSIZE(szRadialGradientText) - 1, m_pTextFormat, &rcTextRect, m_pBlackBrush);

		// 비트맵 붓 데모.

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(200, 200)));

		m_pRenderTarget->DrawText(szBitmapBrushText, ARRAYSIZE(szBitmapBrushText) - 1, m_pTextFormat, &rcTextRect, m_pBlackBrush);
		
		m_pRenderTarget->FillRectangle(&rcBrushRect, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);

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
					ValidateRect(hwnd, NULL);
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

HRESULT DemoApp::LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;

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
		hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);

	return hr;
}

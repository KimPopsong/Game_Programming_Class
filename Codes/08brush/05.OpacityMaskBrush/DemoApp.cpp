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
	m_pDirect2DFactory(NULL),
	m_pRenderTarget(NULL),
	m_pBlackBrush(NULL),
	m_pWICFactory(NULL),
	m_pLinearGradientBrush(NULL),
	m_pRadialGradientBrush(NULL),
	m_pDirectWriteFactory(NULL),
	m_pRectGeo(NULL),
	m_pLinearFadeFlowersBitmapBrush(NULL),
	m_pRadialFadeFlowersBitmapBrush(NULL),
	m_pLinearFadeFlowersBitmap(NULL),
	m_pRadialFadeFlowersBitmap(NULL)
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pDirect2DFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pLinearGradientBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pDirectWriteFactory);
	SAFE_RELEASE(m_pRectGeo);
	SAFE_RELEASE(m_pLinearFadeFlowersBitmapBrush);
	SAFE_RELEASE(m_pRadialFadeFlowersBitmapBrush);
	SAFE_RELEASE(m_pLinearFadeFlowersBitmap);
	SAFE_RELEASE(m_pRadialFadeFlowersBitmap);
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
			640, 440, NULL, NULL, HINST_THISCOMPONENT, this);

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

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2DFactory);

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pDirect2DFactory->CreateRectangleGeometry(D2D1::RectF(0,0,150,150), &m_pRectGeo);
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

		hr = m_pDirect2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);

			if (SUCCEEDED(hr))
			{
				hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"LinearFadeFlowers", L"Image", &m_pLinearFadeFlowersBitmap);
			}
			if (SUCCEEDED(hr))
			{
				hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"RadialFadeFlowers", L"Image", &m_pRadialFadeFlowersBitmap);
			}

			if (SUCCEEDED(hr))
			{
				D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
					D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

				hr = m_pRenderTarget->CreateBitmapBrush(m_pLinearFadeFlowersBitmap, propertiesXClampYClamp, &m_pLinearFadeFlowersBitmapBrush);

				if (SUCCEEDED(hr))
				{
					hr = m_pRenderTarget->CreateBitmapBrush(m_pRadialFadeFlowersBitmap, propertiesXClampYClamp, &m_pRadialFadeFlowersBitmapBrush);
				}
			}

			if (SUCCEEDED(hr))
			{
				ID2D1GradientStopCollection* pGradientStops = NULL;

				static const D2D1_GRADIENT_STOP gradientStops[] =
				{
					{   0.f,  D2D1::ColorF(D2D1::ColorF::Black, 1.0f)  },
					{   1.f,  D2D1::ColorF(D2D1::ColorF::White, 0.0f)  },
				};

				hr = m_pRenderTarget->CreateGradientStopCollection(gradientStops, 2, &pGradientStops);

				if (SUCCEEDED(hr))
				{
					hr = m_pRenderTarget->CreateLinearGradientBrush(
						D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(150, 150)),
						pGradientStops, &m_pLinearGradientBrush);
				}

				if (SUCCEEDED(hr))
				{
					hr = m_pRenderTarget->CreateRadialGradientBrush(
						D2D1::RadialGradientBrushProperties(D2D1::Point2F(75, 75), D2D1::Point2F(0, 0), 75, 75),
						pGradientStops, &m_pRadialGradientBrush);
				}
				pGradientStops->Release();
			}
		}
	}
	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(m_pLinearGradientBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pLinearFadeFlowersBitmapBrush);
	SAFE_RELEASE(m_pRadialFadeFlowersBitmapBrush);
	SAFE_RELEASE(m_pLinearFadeFlowersBitmap);
	SAFE_RELEASE(m_pRadialFadeFlowersBitmap);
}

void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)>0)
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

		// 붓으로 채울 사각형 모양을 정의함.
		D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, 150, 150);

		// 선형 계조 붓을 불투명 마스크로 사용하는 데모.

		// 원본 이미지 비트맵 붓인 m_pLinearFadeFlowersBitmapBrush로 사각형을 채움.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 5));
		m_pRenderTarget->FillRectangle(rcBrushRect, m_pLinearFadeFlowersBitmapBrush);
		m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

		// 선형 계조 붓인 m_pLinearGradientBrush으로 사각형을 채움.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(205, 5));
		m_pRenderTarget->FillRectangle(rcBrushRect, m_pLinearGradientBrush);
		m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

		// 사각형을 m_pLinearFadeFlowersBitmapBrush 붓으로 채우면서 m_pLinearGradientBrush을 불투명 마스크로 사용함.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(405, 5));
		m_pRenderTarget->FillGeometry(m_pRectGeo, m_pLinearFadeFlowersBitmapBrush, m_pLinearGradientBrush);
		m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

		// 방사형 계조 붓을 불투명 마스크로 사용하는 데모.

		// 원본 이미지 비트맵 붓인 m_pRadialFadeFlowersBitmapBrush로 사각형을 채움.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 205));
		m_pRenderTarget->FillRectangle(rcBrushRect, m_pRadialFadeFlowersBitmapBrush);
		m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

		// 방사형 계조 붓인 m_pRadialGradientBrush으로 사각형을 채움.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(205, 205));
		m_pRenderTarget->FillRectangle(rcBrushRect, m_pRadialGradientBrush);
		m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

		// 사각형을 m_pRadialFadeFlowersBitmapBrush 붓으로 채우면서 m_pRadialGradientBrush을 불투명 마스크로 사용함.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(405, 205));
		m_pRenderTarget->FillGeometry(m_pRectGeo, m_pRadialFadeFlowersBitmapBrush, m_pRadialGradientBrush);
		m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

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
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
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

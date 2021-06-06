#include "DrawExtendMode.h"

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
	m_pWICFactory(NULL),
	m_pD2DFactory(NULL),
	m_pRenderTarget(NULL),
	m_pBlackBrush(NULL),
	m_pBitmapBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL),
	m_pTextLayout(NULL)
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(m_pBitmapBrush);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pTextLayout);
}

HRESULT DemoApp::Initialize()
{
	HRESULT hr;

	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		WNDCLASSEX wcex= { sizeof(WNDCLASSEX) };
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
			800, 800, NULL, NULL, HINST_THISCOMPONENT, this);
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

	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));

	if (SUCCEEDED(hr))
	{
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	}

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
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			10.0f,
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

		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);
		}

		ID2D1Bitmap* pBitmap = NULL;

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromResource(
				m_pRenderTarget,
				m_pWICFactory,
				L"Triangle",
				L"Image",
				&pBitmap
				);

			if (SUCCEEDED(hr))
			{
				D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
					D2D1_EXTEND_MODE_CLAMP,
					D2D1_EXTEND_MODE_CLAMP,
					D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
					);

				hr = m_pRenderTarget->CreateBitmapBrush(
					pBitmap,
					propertiesXClampYClamp,
					&m_pBitmapBrush
					);
			}
		}

		SAFE_RELEASE(pBitmap);
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
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
		// Define captions.
		static const WCHAR sc_captionOne[] = L"CLAMP for X\nCLAMP for Y";
		static const WCHAR sc_captionTwo[] = L"CLAMP for X\nWRAP for Y";
		static const WCHAR sc_captionThree[] = L"CLAMP for X\nMIRROR for Y";
		static const WCHAR sc_captionFour[] = L"WRAP for X\nCLAMP for Y";
		static const WCHAR sc_captionFive[] = L"WRAP for X\nWRAP for Y";
		static const WCHAR sc_captionSix[] = L"WRAP for X\nMIRROR for Y";
		static const WCHAR sc_captionSeven[] = L"MIRROR for X\nCLAMP for Y";
		static const WCHAR sc_captionEight[] = L"MIRROR for X\nWRAP for Y";
		static const WCHAR sc_captionNine[] = L"MIRROR for X\nMIRROR for Y";

		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_RECT_F exampleRectangle = D2D1::RectF(5, 5, 105, 105);
		D2D1_RECT_F captionBounds = D2D1::RectF(5, 110, 105, 145);

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5,5));
		m_pBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
		m_pBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);

		m_pRenderTarget->FillRectangle(exampleRectangle, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(exampleRectangle, m_pBlackBrush);

		m_pRenderTarget->DrawText(
			sc_captionOne,
			ARRAYSIZE(sc_captionOne) - 1,
			m_pTextFormat,
			captionBounds,
			m_pBlackBrush
			);


		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(115, 5));
		m_pBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
		m_pBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);

		m_pRenderTarget->FillRectangle(exampleRectangle, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(exampleRectangle, m_pBlackBrush);

		m_pRenderTarget->DrawText(sc_captionTwo,
			ARRAYSIZE(sc_captionTwo) - 1,
			m_pTextFormat,
			captionBounds,
			m_pBlackBrush
			);


		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(225, 5));
		m_pBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
		m_pBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_MIRROR);

		m_pRenderTarget->FillRectangle(exampleRectangle, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(exampleRectangle, m_pBlackBrush);

		m_pRenderTarget->DrawText(
			sc_captionThree,
			ARRAYSIZE(sc_captionThree) - 1,
			m_pTextFormat,
			captionBounds,
			m_pBlackBrush
			);


		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 155));
		m_pBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_WRAP);
		m_pBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);

		m_pRenderTarget->FillRectangle(exampleRectangle, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(exampleRectangle, m_pBlackBrush);

		m_pRenderTarget->DrawText(
			sc_captionFour,
			ARRAYSIZE(sc_captionFour) - 1,
			m_pTextFormat,
			captionBounds,
			m_pBlackBrush
			);


		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(115, 155));
		m_pBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_WRAP);
		m_pBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);

		m_pRenderTarget->FillRectangle(exampleRectangle, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(exampleRectangle, m_pBlackBrush);

		m_pRenderTarget->DrawText(
			sc_captionFive,
			ARRAYSIZE(sc_captionFive) - 1,
			m_pTextFormat,
			captionBounds,
			m_pBlackBrush
			);


		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(225, 155));
		m_pBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_WRAP);
		m_pBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_MIRROR);

		m_pRenderTarget->FillRectangle(exampleRectangle, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(exampleRectangle, m_pBlackBrush);

		m_pRenderTarget->DrawText(
			sc_captionSix,
			ARRAYSIZE(sc_captionSix) - 1,
			m_pTextFormat,
			captionBounds,
			m_pBlackBrush
			);

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 305));
		m_pBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
		m_pBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);

		m_pRenderTarget->FillRectangle(exampleRectangle, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(exampleRectangle, m_pBlackBrush);

		m_pRenderTarget->DrawText(
			sc_captionSeven,
			ARRAYSIZE(sc_captionSeven) - 1,
			m_pTextFormat,
			captionBounds,
			m_pBlackBrush
			);

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(115, 305));
		m_pBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
		m_pBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);

		m_pRenderTarget->FillRectangle(exampleRectangle, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(exampleRectangle, m_pBlackBrush);

		m_pRenderTarget->DrawText(
			sc_captionEight,
			ARRAYSIZE(sc_captionEight) - 1,
			m_pTextFormat,
			captionBounds,
			m_pBlackBrush
			);


		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(225, 305));
		
		m_pBitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
		m_pBitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_MIRROR);

		m_pRenderTarget->FillRectangle(exampleRectangle, m_pBitmapBrush);
		m_pRenderTarget->DrawRectangle(exampleRectangle, m_pBlackBrush);

		m_pRenderTarget->DrawText(
			sc_captionNine,
			ARRAYSIZE(sc_captionNine) - 1,
			m_pTextFormat,
			captionBounds,
			m_pBlackBrush
			);

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

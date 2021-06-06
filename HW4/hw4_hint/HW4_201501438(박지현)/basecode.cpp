#include "hw3.h"
#include "Character.h"

const int NUM_ENEMIES = 3;


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

// DemoApp constructor. Initialize members.
DemoApp::DemoApp() :

	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL),
	m_pWICFactory(NULL),

	// Text
	m_pDWriteFactory(NULL),
	m_pTextBrush(NULL),
	m_pTextFormat(NULL),

	// Brush
	m_pSceneBrush(NULL),
	fly_path_red_Brush(NULL),
	m_pRedBrush(NULL),

	// Bitmap
	m_pBackgroundBitmap(NULL),
	ramenTypeBitmap(),
	gasrangeBitmap(NULL),
	gasrangeBitmapBrush(NULL),
	kettleGeometry(NULL),
	// Bitmap_Brush
	m_pBackgroundBitmapBrush(NULL),
	ramenTypeBrush(),
	ramenTypeOpacityBrush(),


	// PathGeomatry & Animation
	fly_PathGeometry(NULL),

	m_Animation(),
	m_Animation2()
{
}

// DemoApp destructor. Releases the application's resources.
DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pDirect2dFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pWICFactory);

	// Text
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pTextFormat);

	// Brush
	SAFE_RELEASE(m_pSceneBrush);
	SAFE_RELEASE(fly_path_red_Brush);
	SAFE_RELEASE(m_pRedBrush);

	// Bitmap
	SAFE_RELEASE(gasrangeBitmap);
	SAFE_RELEASE(m_pBackgroundBitmap);


	// Bitmap_Brush
	SAFE_RELEASE(gasrangeBitmapBrush);
	SAFE_RELEASE(m_pBackgroundBitmapBrush);


	// PathGeomatry & Animation
	SAFE_RELEASE(kettleGeometry);
	SAFE_RELEASE(fly_PathGeometry);

}

void DemoApp::setRandomSkullPosition() {
	int posx[NUM_ENEMIES], posy[NUM_ENEMIES], cx, cy;
	int k = 0;
	bool duplicated;
	for (int i = 0; i < NUM_ENEMIES; i++)
	{
		do {
			cx = rand() % PLANE_SIZE_X;
			cy = rand() % PLANE_SIZE_Y;
			duplicated = false;
			if (cx < 0 || cy < 0 || cx > 1400 || cy> 800)
				duplicated = true;
			if (i != 0)
			{
				for (int j = 0; j < i; j++)
				{
					if (sqrt((float)(cx - posx[i])*(cx - posx[i]) + (float)(cy - posy[i])*(cy - posy[i])) < 10)
					{
						duplicated = true;
						break;
					}
				}
			}
		} while (duplicated);
		posx[k] = cx; posy[k] = cy; k++;
	}
	for (int i = 0; i < character.size(); i++) {
		character[i]->setPosition(D2D1::Point2F((float)posx[i], (float)posy[i]));
	}
}

HRESULT DemoApp::Initialize(HINSTANCE hInstance)
{
	HRESULT hr;

	// Initialize device-indpendent resources, such as the Direct2D factory.
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"201501438 박지현 HW3";
		RegisterClassEx(&wcex);

		// Create the window.
		m_hwnd = CreateWindow(
			L"201501438 박지현 HW3", L"201501438 박지현 HW3",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			1400, 800, NULL, NULL, HINST_THISCOMPONENT, this
		);
		hr = m_hwnd ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// sound
		addSound(m_hwnd);
		//soundManager->pDSBPrimary->SetVolume(vol);
		soundManager->play(0, TRUE);soundManager->play(1, TRUE);soundManager->play(2, TRUE);soundManager->play(3, TRUE); // background
		SetVolume(soundManager->pDSBPrimary, 90);
	}

	if (SUCCEEDED(hr))
	{
		float length = 0;
		hr = fly_PathGeometry->ComputeLength(
			NULL, //noatransform
			&length
		);

		if (SUCCEEDED(hr))
		{
			m_Animation.SetStart(0);        //start at beginning of path
			m_Animation.SetEnd(length);     //length at end of path
			m_Animation.SetDuration(7.0f);  //seconds

			ZeroMemory(&fly_DwmTimingInfo, sizeof(fly_DwmTimingInfo));
			fly_DwmTimingInfo.cbSize = sizeof(fly_DwmTimingInfo);

			// Get the composition refresh rate. If the DWM isn't running,
			// get the refresh rate from GDI -- probably going to be 60Hz
			if (FAILED(DwmGetCompositionTimingInfo(NULL, &fly_DwmTimingInfo)))
			{
				HDC hdc = GetDC(m_hwnd);
				fly_DwmTimingInfo.rateCompose.uiDenominator = 1;
				fly_DwmTimingInfo.rateCompose.uiNumerator = GetDeviceCaps(hdc, VREFRESH);
				ReleaseDC(m_hwnd, hdc);
			}

			for (int i = 0; i < NUM_ENEMIES; i++)
			{
				DWORD enemyStateTransitions[][3] = {
					{ Character::STATE_STAND, Character::EVENT_FINDTARGET, Character::STATE_FOLLOW },
					{ Character::STATE_STAND, Character::EVENT_BEATTACKED, Character::STATE_ATTACK },
					{ Character::STATE_STAND, Character::EVENT_DUBIOUS, Character::STATE_MOVE },
					{ Character::STATE_MOVE, Character::EVENT_FINDTARGET, Character::STATE_FOLLOW },
					{ Character::STATE_MOVE, Character::EVENT_STOPWALK, Character::STATE_STAND },
					{ Character::STATE_ATTACK, Character::EVENT_LOSTTARGET, Character::STATE_STAND },
					{ Character::STATE_ATTACK, Character::EVENT_HEALTHDRAINED, Character::STATE_RUNAWAY },
					{ Character::STATE_ATTACK, Character::EVENT_OUTOFATTACK, Character::STATE_FOLLOW },
					{ Character::STATE_FOLLOW, Character::EVENT_WITHINATTACK, Character::STATE_ATTACK },
					{ Character::STATE_FOLLOW, Character::EVENT_LOSTTARGET, Character::STATE_STAND },
					{ Character::STATE_RUNAWAY, Character::EVENT_LOSTTARGET, Character::STATE_STAND },
				};

				Character* enemy = new Character(Character::TYPE_AI, enemyStateTransitions, 11);
				//enemy->setPosition(D2D1::Point2F((float)posx[i], (float)posy[i]));
				character.push_back(enemy);
			}
			setRandomSkullPosition();

			DWORD myStateTransitions[][3] =
			{
				{ Character::STATE_MOVE, Character::EVENT_UNDEFINED, Character::STATE_MOVE }
				/*	{ Character::STATE_STAND, Character::EVENT_FINDTARGET, Character::STATE_MOVE },
					{ Character::STATE_MOVE, Character::EVENT_STOPWALK, Character::STATE_STAND },*/
			};

			myCharacter = new Character(Character::TYPE_HUMAN, myStateTransitions, 1);

		}
	}

	if (SUCCEEDED(hr))
	{
		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);
	}


	return hr;
}
void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pSceneBrush);
	SAFE_RELEASE(fly_path_red_Brush);
	SAFE_RELEASE(m_pRedBrush);
}

// Runs the main window message loop.
void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//  If the application receives a WM_SIZE message, this method resizes the render target appropriately.
void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// Note: This method can fail, but it's okay to ignore the error here, because the error will be returned again the next time EndDraw is called.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

// Creates a Direct2D bitmap from a resource in the application resource file.
HRESULT DemoApp::LoadResourceBitmap(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	PCWSTR resourceName,
	PCWSTR resourceType,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap **ppBitmap
)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
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

#include "Hw3.h"
#include <mmsystem.h>

D2D_POINT_2F MainCharPoint = { 500,410 };

int MainHP = 1;

Bullet Enemy1[10];
Bullet Enemy2[10];
Bullet Enemy3[10];
Bullet Enemy4[10];
Bullet Enemy5[100];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		DemoApp app;

		if (SUCCEEDED(app.Initialize(hInstance)))
		{
			app.RunMessageLoop();
		}

		CoUninitialize();
	}

	return 0;
}

DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pD2DFactory(NULL),
	m_pWICFactory(NULL),
	m_pRenderTarget(NULL),
	m_pBackGround2(NULL),
	m_pBackGround2_mask(NULL),
	m_pMain_Bitmap1(NULL),
	m_pMain_Bitmap2(NULL),
	m_pMain_Bitmap3(NULL),
	m_pBackGround2Brush(NULL),
	m_pBackGround2_maskBrush(NULL),
	m_pGameOver_Bitmap(NULL),
	m_pMain_Bitmap1_Brush(NULL),
	m_pBoss_PathGeometry(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL),
	m_pTextFormat2(NULL),
	m_pWhiteBrush(NULL),
	m_pYellowBrush(NULL),
	m_pRedBrush(NULL),
	m_Animation()
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBackGround2);
	SAFE_RELEASE(m_pBackGround2_mask);
	SAFE_RELEASE(m_pMain_Bitmap1);
	SAFE_RELEASE(m_pMain_Bitmap2);
	SAFE_RELEASE(m_pMain_Bitmap3);
	SAFE_RELEASE(m_pEnemyBullet1_Bitmap);
	SAFE_RELEASE(m_pEnemyBullet2_Bitmap);
	SAFE_RELEASE(m_pEnemyBullet_Bitmap1_Brush);
	SAFE_RELEASE(m_pEnemyBullet_Bitmap2_Brush);
	SAFE_RELEASE(m_pGameOver_Bitmap);
	SAFE_RELEASE(m_pBackGround2Brush);
	SAFE_RELEASE(m_pBackGround2_maskBrush);
	SAFE_RELEASE(m_pMain_Bitmap1_Brush);
	SAFE_RELEASE(m_pMainBullet_Bitmap_Brush);
	SAFE_RELEASE(m_pBoss_PathGeometry);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pTextFormat2);
	SAFE_RELEASE(m_pWhiteBrush);
	SAFE_RELEASE(m_pYellowBrush);
	SAFE_RELEASE(m_pRedBrush);
}

HRESULT DemoApp::Initialize(HINSTANCE hInstance)
{
	HRESULT hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = hInstance;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = "D2DDemoApp";
		RegisterClassEx(&wcex);

		m_hwnd = CreateWindow("D2DDemoApp", "201801527김대성", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 820, NULL, NULL, hInstance, this);

		hr = m_hwnd ? S_OK : E_FAIL;

		if (SUCCEEDED(hr))
		{
			float length = 0;
			hr = m_pBoss_PathGeometry->ComputeLength(NULL, &length);

			if (SUCCEEDED(hr))
			{
				m_Animation.SetStart(0); //start at beginning of path
				m_Animation.SetEnd(length); //length at end of path
				m_Animation.SetDuration(8.0f); //seconds

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}

		//메인샷 배열 초기화 및 좌표 설정
		srand(time(NULL));
		ZeroMemory(&Enemy1, sizeof(Enemy1));
		ZeroMemory(&Enemy2, sizeof(Enemy2));
		Enemy1[0].Source = D2D1::Rect(0, 0, 25, 25);
		Enemy2[0].Source = D2D1::Rect(0, 0, 25, 25);
		Enemy3[0].Source = D2D1::Rect(0, 0, 25, 25);
		Enemy4[0].Source = D2D1::Rect(0, 0, 25, 25);
		Enemy5[0].Source = D2D1::Rect(0, 0, 25, 25);

		for (int i = 1; i < 100; ++i)
		{
			Enemy5[i].Source = Enemy5[0].Source;
		}

		for (int i = 1; i < 10; ++i)
		{
			Enemy1[i].Source = Enemy1[0].Source;
			Enemy2[i].Source = Enemy2[0].Source;
			Enemy3[i].Source = Enemy3[0].Source;
			Enemy4[i].Source = Enemy4[0].Source;
		}

		QueryPerformanceFrequency(&m_nFrequency);
		QueryPerformanceCounter(&m_nPrevTime);
	}

	return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	static const WCHAR msc_fontName[] = L"Adobe Garamond";
	static const FLOAT msc_fontSize = 30;
	ID2D1GeometrySink* pSink = NULL;
	// D2D 팩토리를 생성함.
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	// WIC 팩토리를 생성함.
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	}

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr))
	{
		// DirectWrite 팩토리를 생성함.
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	}

	if (SUCCEEDED(hr))
	{
		//DirectWrite 텍스트 포맷 객체를 생성함.
		hr = m_pDWriteFactory->CreateTextFormat(msc_fontName, NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, msc_fontSize, L"", &m_pTextFormat);
	}

	if (SUCCEEDED(hr))
	{
		//DirectWrite 텍스트 포맷 객체를 생성함.
		hr = m_pDWriteFactory->CreateTextFormat(msc_fontName, NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, msc_fontSize, L"", &m_pTextFormat2);
	}

	if (SUCCEEDED(hr))
	{
		// 텍스트를 수평으로 중앙 정렬하고 수직으로도 중앙 정렬함.
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		m_pTextFormat2->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat2->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pBoss_PathGeometry);

		if (SUCCEEDED(hr))
		{
			hr = m_pBoss_PathGeometry->Open(&pSink);

			if (SUCCEEDED(hr))
			{
				D2D1_POINT_2F currentLocation = { 120.0f, 150.0f };

				pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_FILLED);

				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(120, 200), D2D1::Point2F(150, 300), D2D1::Point2F(600, 150)));
				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(600, 100), D2D1::Point2F(570, 50), D2D1::Point2F(120, 150)));

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

		// D2D 렌더타겟을 생성함.
		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);
		//배경 객체 생성

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\BackGround.jpg", 1000, 800, &m_pBackGround2);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\BackGround.jpg", 1000, 800, &m_pBackGround2_mask);
		}

		//이미지 파일로부터 메인캐릭터용 비트맵 객체 생성
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\User.png", 60, 60, &m_pMain_Bitmap1);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\User.png", 60, 60, &m_pMain_Bitmap2);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\User.png", 60, 60, &m_pMain_Bitmap3);
		}

		//적 샷 비트맵 객체 생성
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\bullet.png", 25, 25, &m_pEnemyBullet1_Bitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\bullet.png", 25, 25, &m_pEnemyBullet2_Bitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\GameOver.jpg", 1000, 800, &m_pGameOver_Bitmap);
		}
	}

	//비트맵 브러쉬들 생성
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		hr = m_pRenderTarget->CreateBitmapBrush(m_pBackGround2, propertiesXClampYClamp, &m_pBackGround2Brush);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(m_pBackGround2_mask, propertiesXClampYClamp, &m_pBackGround2_maskBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(m_pMain_Bitmap1, propertiesXClampYClamp, &m_pMain_Bitmap1_Brush);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(m_pEnemyBullet1_Bitmap, propertiesXClampYClamp, &m_pEnemyBullet_Bitmap1_Brush);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(m_pEnemyBullet2_Bitmap, propertiesXClampYClamp, &m_pEnemyBullet_Bitmap2_Brush);
		}
	}

	if (SUCCEEDED(hr))
	{
		// 하얀색 붓을 생성함.
		hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pWhiteBrush);
	}

	if (SUCCEEDED(hr))
	{
		// 노란색 붓을 생성함
		hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pYellowBrush);
	}

	if (SUCCEEDED(hr))
	{
		// 회색 붓을 생성함
		hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pRedBrush);
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBackGround2);
	SAFE_RELEASE(m_pBackGround2_mask);
	SAFE_RELEASE(m_pMain_Bitmap1);
	SAFE_RELEASE(m_pMain_Bitmap2);
	SAFE_RELEASE(m_pMain_Bitmap3);
	SAFE_RELEASE(m_pBackGround2Brush);
	SAFE_RELEASE(m_pBackGround2_maskBrush);
	SAFE_RELEASE(m_pEnemyBullet1_Bitmap);
	SAFE_RELEASE(m_pEnemyBullet2_Bitmap);
	SAFE_RELEASE(m_pGameOver_Bitmap);
	SAFE_RELEASE(m_pEnemyBullet_Bitmap1_Brush);
	SAFE_RELEASE(m_pEnemyBullet_Bitmap2_Brush);
	SAFE_RELEASE(m_pMain_Bitmap1_Brush);
	SAFE_RELEASE(m_pBoss_PathGeometry);
	SAFE_RELEASE(m_pMainBullet_Bitmap_Brush);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pTextFormat2);
	SAFE_RELEASE(m_pWhiteBrush);
	SAFE_RELEASE(m_pYellowBrush);
	SAFE_RELEASE(m_pRedBrush);
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
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	static float anim_time = 0.0f;
	float length = m_Animation.GetValue(anim_time);
	static float displayTimer = 0.0f;
	static float timer = 0.0f;
	static int random_Shot;

	if (MainHP <= 0)
	{
		if (SUCCEEDED(hr))
		{
			// 렌더타겟의 크기를 얻음.
			D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

			m_pRenderTarget->BeginDraw();
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			// 게임 클리어배경을 칠함.
			m_pRenderTarget->DrawBitmap(m_pGameOver_Bitmap, D2D1::Rect(0, 0, 1000, 800));

			WCHAR szText[100];
			_swprintf(szText, L"시간 : %.1f\n", displayTimer);  // 시간 표시
			m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, D2D1::RectF(710.0f, 0.0f, 970.0f, 100.0), m_pWhiteBrush);

			hr = m_pRenderTarget->EndDraw();
		}
	}

	else if (MainHP > 0)
	{
		if (SUCCEEDED(hr))
		{
			// 렌더타겟의 크기를 얻음.
			D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

			m_pRenderTarget->BeginDraw();
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			//게임 인터페이스 배경을 칠함
			D2D1_RECT_F BackRect = D2D1::Rect(0, 0, 1000, 800);
			m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
			m_pRenderTarget->FillOpacityMask(m_pBackGround2_mask, m_pBackGround2Brush, D2D1_OPACITY_MASK_CONTENT_GRAPHICS, &BackRect);
			m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

			displayTimer += 0.009;
			timer += 0.01667;

			m_pRenderTarget->DrawGeometry(m_pBoss_PathGeometry, m_pYellowBrush);

			WCHAR szText[100];
			_swprintf(szText, L"시간 : %.1f\n", displayTimer);  // 시간 표시
			m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, D2D1::RectF(710.0f, 0.0f, 970.0f, 100.0), m_pWhiteBrush);

			//메인캐릭터
			D2D1_RECT_F charRect = D2D1::Rect(0, 0, 60, 60);
			D2D1_MATRIX_3X2_F MainChar;
			MainChar = D2D1::Matrix3x2F::Translation(MainCharPoint.x, MainCharPoint.y);
			m_pRenderTarget->SetTransform(MainChar);

			m_pRenderTarget->FillRectangle(charRect, m_pMain_Bitmap1_Brush);

			D2D1_MATRIX_3X2_F damagePointMove;

			D2D1_POINT_2F point;
			D2D1_POINT_2F tangent;

			m_pBoss_PathGeometry->ComputePointAtLength(length, NULL, &point, &tangent);

			random_Shot = rand() % 1000;

			D2D1_RECT_F enemyBullet1 = D2D1::Rect(0, 0, 25, 25);
			D2D1_MATRIX_3X2_F enemyBullet1_T;

			for (INT i = 0; i < 10; ++i)
			{
				if (Enemy1[i].Position.x < MainCharPoint.x + 35 && MainCharPoint.x + 25 < Enemy1[i].Position.x + Enemy1[i].Source.right && Enemy1[i].Position.y < MainCharPoint.y + 35 && MainCharPoint.y + 25 < Enemy1[i].Position.y + Enemy1[i].Source.bottom)
				{
					if (MainHP >= 0)
					{
						MainHP--;
					}
				}

				if (Enemy2[i].Position.x < MainCharPoint.x + 35 && MainCharPoint.x + 25 < Enemy2[i].Position.x + Enemy2[i].Source.right && Enemy2[i].Position.y < MainCharPoint.y + 35 && MainCharPoint.y + 25 < Enemy2[i].Position.y + Enemy2[i].Source.bottom)
				{
					if (MainHP >= 0)
					{
						MainHP--;
					}
				}

				if (Enemy3[i].Position.x < MainCharPoint.x + 35 && MainCharPoint.x + 25 < Enemy3[i].Position.x + Enemy3[i].Source.right && Enemy3[i].Position.y < MainCharPoint.y + 35 && MainCharPoint.y + 25 < Enemy3[i].Position.y + Enemy3[i].Source.bottom)
				{
					if (MainHP >= 0)
					{
						MainHP--;
					}
				}

				if (Enemy4[i].Position.x < MainCharPoint.x + 35 && MainCharPoint.x + 25 < Enemy4[i].Position.x + Enemy4[i].Source.right && Enemy4[i].Position.y < MainCharPoint.y + 35 && MainCharPoint.y + 25 < Enemy4[i].Position.y + Enemy4[i].Source.bottom)
				{
					if (MainHP >= 0)
					{
						MainHP--;
					}
				}
			}

			D2D1_MATRIX_3X2_F enemyBullet2_T;

			if (timer > 0.25)
			{
				for (int i = 0; i < 100; ++i)
				{
					if (Enemy5[i].Visible == FALSE)
					{
						Enemy5[i].Visible = TRUE;
						Enemy5[i].Position.x = random_Shot;
						Enemy5[i].Position.y = 55;
						break;
					}
				}

				timer = 0;
			}

			for (int i = 0; i < 100; ++i)
			{
				Enemy5[i].Position.y += 1.0f;

				enemyBullet2_T = D2D1::Matrix3x2F::Translation(Enemy5[i].Position.x, Enemy5[i].Position.y);
				if (Enemy5[i].Visible)
				{
					m_pRenderTarget->SetTransform(enemyBullet2_T);
					m_pRenderTarget->FillRectangle(Enemy5[i].Source, m_pEnemyBullet_Bitmap2_Brush);
				}
			}

			for (int i = 0; i < 100; ++i)
			{
				if (Enemy5[i].Position.x < MainCharPoint.x + 35 && MainCharPoint.x + 25 < Enemy5[i].Position.x + Enemy5[i].Source.right && Enemy5[i].Position.y < MainCharPoint.y + 35 && MainCharPoint.y + 25 < Enemy5[i].Position.y + Enemy5[i].Source.bottom)
				{
					if (MainHP >= 0)
					{
						MainHP--;
					}
				}
			}

			hr = m_pRenderTarget->EndDraw();

			if (hr == D2DERR_RECREATE_TARGET)
			{
				hr = S_OK;
				DiscardDeviceResources();
			}

			// 애니메이션의 끝에 도달하면 다시 처음으로 되돌려서 반복되도록 함.
			if (anim_time >= m_Animation.GetDuration())
			{
				anim_time = 0.0f;
			}

			else
			{
				LARGE_INTEGER CurrentTime;
				QueryPerformanceCounter(&CurrentTime);

				float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
				m_nPrevTime = CurrentTime;

				anim_time += elapsedTime;
			}
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

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		result = 1;
	}

	else
	{
		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

		bool wasHandled = false;

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			if (MainCharPoint.x > 4)
				MainCharPoint.x -= 4;
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			if (MainCharPoint.x < 920)
				MainCharPoint.x += 4;
		}

		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			if (MainCharPoint.y > 4)
				MainCharPoint.y -= 4;
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			if (MainCharPoint.y < 720)
				MainCharPoint.y += 4;
		}

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

			case WM_GETMINMAXINFO:
			{
				MINMAXINFO* mmi = (MINMAXINFO*)lParam;
				mmi->ptMinTrackSize.x = 1000;
				mmi->ptMinTrackSize.y = 800;
				mmi->ptMaxTrackSize.x = 1000;
				mmi->ptMaxTrackSize.y = 800;
			}
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

				// Do not call ValidateRect so that the window will be redrawn.
				// ValidateRect(hwnd, NULL);
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

// Creates a Direct2D bitmap from the specified file name.
HRESULT DemoApp::LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap)
{
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(uri, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

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
				}

				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
				}
			}
		}

		else // Don't scale the image.
		{
			hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
		}
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
	SAFE_RELEASE(pScaler);

	return hr;
}

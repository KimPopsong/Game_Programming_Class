#include "pch.h"
#include "DemoApp.h"
//#include <time.h>

static constexpr FLOAT FLOAT_DRAWABLE_SMALL_NUM = 0.000001F;

int WINAPI WinMain(
  HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
  if (SUCCEEDED(CoInitialize(nullptr))) {
    DemoApp app;

    if (SUCCEEDED(app.Initialize())) {
      app.RunMessageLoop();
    }
    CoUninitialize();
  }

  return 0;
}

DemoApp::~DemoApp() {
  SAFE_RELEASE(m_pD2DFactory);
  SAFE_RELEASE(m_pWICFactory);
  SAFE_RELEASE(m_pDWriteFactory);
  SAFE_RELEASE(m_pRenderTarget);

  SAFE_RELEASE(m_pTextFormat);

  SAFE_RELEASE(m_pSpaceBitmap);
  SAFE_RELEASE(m_pRockBitmap);
  SAFE_RELEASE(m_pShooterBitmap);
  SAFE_RELEASE(m_pCometBitmap);
  SAFE_RELEASE(m_pSeagullInPeaceBitmap);
  SAFE_RELEASE(m_pSeagullAwareBitmap);
  SAFE_RELEASE(m_pSeagullFlyingBitmap);

  SAFE_RELEASE(m_pRockAnimationGeometry);
  SAFE_RELEASE(m_pMissileDrawingGeometry);
  SAFE_RELEASE(m_pMissileAnimationGeometry);
  SAFE_RELEASE(m_pCometClipGeometry);

  SAFE_RELEASE(m_pRedBrush);
  SAFE_RELEASE(m_pYellowBrush);
  SAFE_RELEASE(m_pDarkBlueBrush);
  SAFE_RELEASE(m_pRadialOpacityBrush);
  SAFE_RELEASE(m_pCometBitmapBrush);

  SAFE_RELEASE(m_pXAudio2);
}

DemoApp::DemoApp() :
  m_hwnd(nullptr),

  m_pD2DFactory(nullptr),
  m_pWICFactory(nullptr),
  m_pDWriteFactory(nullptr),
  m_pRenderTarget(nullptr),

  m_pTextFormat(nullptr),

  m_pSpaceBitmap(nullptr),
  m_pRockBitmap(nullptr),
  m_pShooterBitmap(nullptr),
  m_pCometBitmap(nullptr),
  m_pSeagullInPeaceBitmap(nullptr),
  m_pSeagullAwareBitmap(nullptr),
  m_pSeagullFlyingBitmap(nullptr),

  m_SeagullState(SeagullState::IN_PEACE),
  m_ppSeagullBitmap(&m_pSeagullInPeaceBitmap),
  m_pSeagullStateWStr(L"IN_PEACE"),
  m_SeagullStartCoord(D2D1::Point2F(NAN, NAN)),

  m_pRockAnimationGeometry(nullptr),
  m_pMissileDrawingGeometry(nullptr),
  m_pMissileAnimationGeometry(nullptr),
  m_pCometClipGeometry(nullptr),

  m_pRedBrush(nullptr),
  m_pYellowBrush(nullptr),
  m_pDarkBlueBrush(nullptr),
  m_pRadialOpacityBrush(nullptr),
  m_pCometBitmapBrush(nullptr),

  m_RockAnimation(),
  m_MissileAnimation(),

  m_pXAudio2(nullptr),
  m_pMasterVoice(nullptr),

  m_pHitVoice(nullptr),
  m_HitBuffer({ 0 }),
  m_pMissileReadyVoice(nullptr),
  m_MissileReadyBuffer({ 0 }),
  m_pMissileNotReadyVoice(nullptr),
  m_MissileNotReadyBuffer({ 0 }),
  m_pMissileLaunchVoice(nullptr),
  m_MissileLaunchBuffer({ 0 }),

  m_MissileState(MissileState::READY),

  m_GameState(GameState::PLAYING),

  m_VolumeLog(5),
  m_IsMuted(false) {
}

HRESULT DemoApp::Initialize() {
  HRESULT hr;

  //register window class
  WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = DemoApp::WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = sizeof(LONG_PTR);
  wcex.hInstance = HINST_THISCOMPONENT;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = nullptr;
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = L"DemoApp";
  RegisterClassEx(&wcex);

  hr = CreateDeviceIndependentResources();
  if (SUCCEEDED(hr)) {
    m_hwnd = CreateWindow(
      L"DemoApp", L"우주알까기",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 512, 512,
      nullptr, nullptr, HINST_THISCOMPONENT,
      this
    );
    hr = m_hwnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
      float length = 0.0F;

      hr = m_pRockAnimationGeometry->ComputeLength(nullptr, &length);
      if (SUCCEEDED(hr)) {
        m_RockAnimation.SetStart(0.0F); //start at beginning of path
        m_RockAnimation.SetEnd(length); //length at end of path
        m_RockAnimation.SetDuration(5.0f); //seconds
      }

      hr = m_pMissileAnimationGeometry->ComputeLength(nullptr, &length);
      if (SUCCEEDED(hr)) {
        m_MissileAnimation.SetStart(0.0F); //start at beginning of path
        m_MissileAnimation.SetEnd(length); //length at end of path
        m_MissileAnimation.SetDuration(5.0f); //seconds
      }
    }

    //  XAudio2
    if (SUCCEEDED(hr)) {
      hr = XAudio2Create(&m_pXAudio2);
    }
    if (SUCCEEDED(hr)) {
      hr = m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice);
      m_pMasterVoice->SetVolume(GetVolumeLinear());
    }

    //// hit.wav
    if (SUCCEEDED(hr)) {
      WCHAR filename[] = L".\\resource\\sound\\hit.wav";
      hr = LoadWave(filename, &m_pHitVoice, &m_HitBuffer);
    }

    // missile ready.wav
    if (SUCCEEDED(hr)) {
      WCHAR filename[] = L".\\resource\\sound\\missile ready.wav";
      hr = LoadWave(filename, &m_pMissileReadyVoice, &m_MissileReadyBuffer);
    }

    // missile not ready.wav
    if (SUCCEEDED(hr)) {
      WCHAR filename[] = L".\\resource\\sound\\missile not ready.wav";
      hr = LoadWave(filename, &m_pMissileNotReadyVoice, &m_MissileNotReadyBuffer);
    }

    // missile launch.wav
    if (SUCCEEDED(hr)) {
      WCHAR filename[] = L".\\resource\\sound\\missile launch.wav";
      hr = LoadWave(filename, &m_pMissileLaunchVoice, &m_MissileLaunchBuffer);
    }
    
    if (SUCCEEDED(hr)) {
      ShowWindow(m_hwnd, SW_SHOWNORMAL);
      UpdateWindow(m_hwnd);

      QueryPerformanceFrequency(&m_Frequency);
      QueryPerformanceCounter(&m_PrevTime);
    }
  }

  return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources() {
  HRESULT hr;
  ID2D1GeometrySink* pSink = nullptr;

  // D2D 팩토리를 생성함
  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

  // WIC 팩토리를 생성함.
  if (SUCCEEDED(hr)) {
    hr = CoCreateInstance(
      CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory)
    );
  }

  // Create a DirectWrite factory.
  if (SUCCEEDED(hr)) {
    hr = DWriteCreateFactory(
      DWRITE_FACTORY_TYPE_SHARED,
      __uuidof(m_pDWriteFactory),
      reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
    );
  }

  // Create a DirectWrite text format object.
  if (SUCCEEDED(hr)) {
    hr = m_pDWriteFactory->CreateTextFormat(
      L"",
      nullptr, // font collection
      DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      30,  // font size
      L"",  // locale
      &m_pTextFormat
    );
    m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
  }

  // Path geometry for drawing a path of the rock
  if (SUCCEEDED(hr)) {
    hr = m_pD2DFactory->CreatePathGeometry(&m_pRockAnimationGeometry);
  }
  if (SUCCEEDED(hr)) {
    hr = m_pRockAnimationGeometry->Open(&pSink);
  }
  if (SUCCEEDED(hr)) {
    pSink->BeginFigure({ FLOAT_DRAWABLE_SMALL_NUM, 150.0F }, D2D1_FIGURE_BEGIN_HOLLOW);

    pSink->AddArc(D2D1::ArcSegment(
      { -FLOAT_DRAWABLE_SMALL_NUM, 150.0F }, { 300.0F, 150.0F }, 0.0F,
      D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));

    pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

    hr = pSink->Close();
  }
  SAFE_RELEASE(pSink);

  // Path geometry for drawing a shape of a missile
  if (SUCCEEDED(hr)) {
    hr = m_pD2DFactory->CreatePathGeometry(&m_pMissileDrawingGeometry);
  }
  if (SUCCEEDED(hr)) {
    hr = m_pMissileDrawingGeometry->Open(&pSink);
  }
  if (SUCCEEDED(hr)) {
    pSink->BeginFigure(D2D1::Point2F(-20.0F, 40.0F), D2D1_FIGURE_BEGIN_FILLED);

    pSink->AddLine({ -20.0F, -10.0F });
    pSink->AddArc(D2D1::ArcSegment(
      { 0.0F, -40.0F }, { 40.0F, 40.0F }, 0.0F,
      D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    pSink->AddArc(D2D1::ArcSegment(
      { 20.0F, -10.0F }, { 40.0F, 40.0F }, 0.0F,
      D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    pSink->AddLine({ 20.0F, 40.0F });

    pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

    hr = pSink->Close();
  }
  SAFE_RELEASE(pSink);

  // Path geometry for drawing a path of a missile
  if (SUCCEEDED(hr)) {
    hr = m_pD2DFactory->CreatePathGeometry(&m_pMissileAnimationGeometry);
  }
  if (SUCCEEDED(hr)) {
    hr = m_pMissileAnimationGeometry->Open(&pSink);
  }
  if (SUCCEEDED(hr)) {
    pSink->BeginFigure(D2D1::Point2F(0.0F, 0.0F), D2D1_FIGURE_BEGIN_HOLLOW);

    pSink->AddLine({ 0.0F, -1500.0F });

    pSink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = pSink->Close();
  }
  SAFE_RELEASE(pSink);
  
  // Rectangle geometry for drawing the comet
  if (SUCCEEDED(hr)) {
    hr = m_pD2DFactory->CreateRectangleGeometry(
      D2D1::RectF(0.0F, -100.0F, 200.0F, 0.0F),
      &m_pCometClipGeometry
    );
  }

  return hr;
}

HRESULT DemoApp::CreateDeviceResources() {
  HRESULT hr = S_OK;

  if (!m_pRenderTarget) {
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    // D2D 렌더타겟을 생성함.
    hr = m_pD2DFactory->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(),
      D2D1::HwndRenderTargetProperties(m_hwnd, size),
      &m_pRenderTarget
    );

    // 응용 프로그램 리소스로부터 비트맵 객체 m_pSpaceBitmap를 생성함.
    if (SUCCEEDED(hr)) {
      hr = LoadBitmapFromResource(
        m_pRenderTarget, m_pWICFactory,
        L"IDB_PNG_SPACE", L"PNG",
        0, 0,
        &m_pSpaceBitmap
      );
    }
    // 응용 프로그램 리소스로부터 비트맵 객체 m_pRockBitmap를 생성함.
    if (SUCCEEDED(hr)) {
      hr = LoadBitmapFromResource(
        m_pRenderTarget, m_pWICFactory,
        L"IDB_PNG_ROCK", L"PNG",
        0, 0,
        &m_pRockBitmap
      );
    }
    // 응용 프로그램 리소스로부터 비트맵 객체 m_pShooterBitmap를 생성함.
    if (SUCCEEDED(hr)) {
      hr = LoadBitmapFromResource(
        m_pRenderTarget, m_pWICFactory,
        L"IDB_PNG_SHOOTER", L"PNG",
        0, 0,
        &m_pShooterBitmap
      );
    }
    // 응용 프로그램 리소스로부터 비트맵 객체 m_pCometBitmap를 생성함.
    if (SUCCEEDED(hr)) {
      hr = LoadBitmapFromResource(
        m_pRenderTarget, m_pWICFactory,
        L"IDB_PNG_COMET", L"PNG",
        200, 100,
        &m_pCometBitmap
      );
    }
    // 응용 프로그램 리소스로부터 비트맵 객체 m_pSpaceSeagullInPeaceBitmap를 생성함.
    if (SUCCEEDED(hr)) {
      hr = LoadBitmapFromResource(
        m_pRenderTarget, m_pWICFactory,
        L"IDB_PNG_SEAGULL_IN_PEACE", L"PNG",
        0, 0,
        &m_pSeagullInPeaceBitmap
      );
    }
    // 응용 프로그램 리소스로부터 비트맵 객체 m_pSeagullAwareBitmap를 생성함.
    if (SUCCEEDED(hr)) {
      hr = LoadBitmapFromResource(
        m_pRenderTarget, m_pWICFactory,
        L"IDB_PNG_SEAGULL_AWARE", L"PNG",
        0, 0,
        &m_pSeagullAwareBitmap
      );
    }
    // 응용 프로그램 리소스로부터 비트맵 객체 m_pSeagullFlyingBitmap를 생성함.
    if (SUCCEEDED(hr)) {
      hr = LoadBitmapFromResource(
        m_pRenderTarget, m_pWICFactory,
        L"IDB_PNG_SEAGULL_FLYING", L"PNG",
        0, 0,
        &m_pSeagullFlyingBitmap
      );
    }

    // Create a red brush.
    if (SUCCEEDED(hr)) {
      hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Red), &m_pRedBrush
      );
    }
    // Create a yellow brush.
    if (SUCCEEDED(hr)) {
      hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Yellow), &m_pYellowBrush
      );
    }
    // Create a dark blue brush.
    if (SUCCEEDED(hr)) {
      hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::DarkBlue), &m_pDarkBlueBrush
      );
    }
    // Create a radial opacity brush.
    if (SUCCEEDED(hr)) {
      ID2D1GradientStopCollection* pGradientStopCollection = nullptr;
      static const D2D1_GRADIENT_STOP gradientStops[] = {
        { 0.0F, D2D1::ColorF(D2D1::ColorF::Black, 1.0F) },
        { 1.0F, D2D1::ColorF(D2D1::ColorF::Black, 0.0F) },
      };
      m_pRenderTarget->CreateGradientStopCollection(
        gradientStops, std::size(gradientStops), &pGradientStopCollection
      );
      hr = m_pRenderTarget->CreateRadialGradientBrush(
        D2D1::RadialGradientBrushProperties(
          { 0.0F, 0.0F }, { 0.0F, 0.0F }, 150.0F, 150.0F
        ),
        pGradientStopCollection,
        &m_pRadialOpacityBrush
      );
      pGradientStopCollection->Release();
    }
    // Create the comet bitmap brush
    if (SUCCEEDED(hr)) {
      hr = m_pRenderTarget->CreateBitmapBrush(
        m_pCometBitmap, &m_pCometBitmapBrush
      );
      D2D1_RECT_F rect;
      m_pCometClipGeometry->GetRect(&rect);
      m_pCometBitmapBrush->SetTransform(D2D1::Matrix3x2F::Translation({ rect.left, rect.top }));
    }
  }

  return hr;
}

void DemoApp::DiscardDeviceResources() {
  SAFE_RELEASE(m_pRenderTarget);
  SAFE_RELEASE(m_pRedBrush);
  SAFE_RELEASE(m_pYellowBrush);
}

void DemoApp::RunMessageLoop() {
  MSG msg;

  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

HRESULT DemoApp::OnRender() {
  HRESULT hr;

  static float rockAnimationTime = 0.0F;
  static float missileAnimationTime = 0.0F;
  static float seagullAnimationTime = 0.0F;

  hr = CreateDeviceResources();
  if (SUCCEEDED(hr)) {
    const D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();
    const FLOAT minWidthHeight = min(renderTargetSize.width, renderTargetSize.height);
    const FLOAT minWidthHeightScale = minWidthHeight / 1000.0F;
    const D2D1_MATRIX_3X2_F scaleToFitRenderTarget =
      D2D1::Matrix3x2F::Scale(minWidthHeightScale, minWidthHeightScale);
    const D2D1_MATRIX_3X2_F translationToRenderTargetCenter =
      D2D1::Matrix3x2F::Translation(renderTargetSize.width / 2.0F, renderTargetSize.height / 2.0F);
    const D2D1_MATRIX_3X2_F lastMatrix =
      scaleToFitRenderTarget * translationToRenderTargetCenter;

    const D2D1_MATRIX_3X2_F translationToBlackHole =
      D2D1::Matrix3x2F::Translation(0.0F, -200.0F);
    const D2D1_MATRIX_3X2_F translationToShooter =
      D2D1::Matrix3x2F::Translation(0.0F, 400.0F);
    const D2D1_MATRIX_3X2_F translationToComet =
      D2D1::Matrix3x2F::Translation(-300.0F, 100.0F);
    const D2D1_MATRIX_3X2_F translationToTextInfo =
      D2D1::Matrix3x2F::Translation(-500.0F, 500.0F);

    const D2D1_POINT_2F transformedMouseMove = {
      (m_MouseMove.x - renderTargetSize.width / 2.0F) / minWidthHeightScale - translationToShooter.dx,
      (m_MouseMove.y - renderTargetSize.height / 2.0F) / minWidthHeightScale - translationToShooter.dy
    };
    const D2D1_POINT_2F transformedLButtonUp = {
      (m_LButtonUp.x - renderTargetSize.width / 2.0F) / minWidthHeightScale - translationToShooter.dx,
      (m_LButtonUp.y - renderTargetSize.height / 2.0F) / minWidthHeightScale - translationToShooter.dy
    };

    const D2D1_POINT_2F seagullLeavingCoord = D2D1::Point2F(800.0F, 0.0F);
    constexpr float seagullLeavingTime = 5.0F;

    D2D1_POINT_2F rockCoord = D2D1::Point2F(NAN, NAN);
    D2D1_POINT_2F missileCoord = D2D1::Point2F(NAN, NAN);

    // 그리기를 준비함.
    m_pRenderTarget->BeginDraw();

    // 렌더타겟을 클리어함.
    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    // Drawing background.
    {
      m_pRenderTarget->SetTransform(lastMatrix);
      m_pRenderTarget->DrawBitmap(
        m_pSpaceBitmap, D2D1::RectF(-500.0F, -500.0F, 500.0F, 500.0F)
      );
    }

    // Drawing comet.
    {
      m_pRenderTarget->SetTransform(translationToComet * lastMatrix);
      m_pRenderTarget->FillGeometry(m_pCometClipGeometry, m_pCometBitmapBrush, m_pRadialOpacityBrush);
    }

    // m_pRockBitmap, seagull
    // 이동 동선 기하 경로가 화면 위쪽에 그려지도록 함.
    m_pRenderTarget->SetTransform(translationToBlackHole * lastMatrix);
    // 이동 동선을 노란색으로 그림.
    m_pRenderTarget->DrawGeometry(m_pRockAnimationGeometry, m_pYellowBrush);
    {
      const float length = m_RockAnimation.GetValue(rockAnimationTime);
      D2D1_POINT_2F tangent;
      // 현재 시간에 해당하는 기하 길이에 일치하는 이동 동선 상의 지점을 얻음.
      m_pRockAnimationGeometry->ComputePointAtLength(
        length, translationToBlackHole, &rockCoord, &tangent
      );
      // 우주 암석의 방향을 조절하여 이동 동선을 따라가는 방향이 되도록 함.
      const D2D1_MATRIX_3X2_F rockMatrix =
        D2D1::Matrix3x2F(
          tangent.x,  tangent.y,
          -tangent.y, tangent.x,
          rockCoord.x, rockCoord.y
        );

      if (m_GameState != GameState::SUCCEESS) {
        m_pRenderTarget->SetTransform(rockMatrix * lastMatrix);
        const D2D1_SIZE_F rockSize = m_pRockBitmap->GetSize();
        m_pRenderTarget->DrawBitmap(
          m_pRockBitmap,
          D2D1::RectF(
            -rockSize.width / 2.0F, -rockSize.height / 2.0F,
            rockSize.width / 2.0F, rockSize.height / 2.0F
          )
        );
      }
      if (m_ppSeagullBitmap) {
        float p = seagullAnimationTime / seagullLeavingTime;
        m_pRenderTarget->SetTransform(
          (
            m_SeagullState == SeagullState::LEAVING ?
            D2D1::Matrix3x2F::Translation(
              (1.0F - p) * m_SeagullStartCoord.x + p * seagullLeavingCoord.x,
              (1.0F - p) * m_SeagullStartCoord.y + p * seagullLeavingCoord.y
            ) :
            rockMatrix
          ) *
          lastMatrix
        );
        const D2D1_SIZE_F seagullSize = (*m_ppSeagullBitmap)->GetSize();
        m_pRenderTarget->DrawBitmap(
          *m_ppSeagullBitmap,
          D2D1::RectF(
            -seagullSize.width / 8.0F, -seagullSize.height / 8.0F,
            seagullSize.width / 8.0F,  seagullSize.height / 8.0F
          )
        );
      }
    }

    // Drawing shooter.
    {
      const D2D1_SIZE_F size = m_pShooterBitmap->GetSize();
      const D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.0F, 0.0F);
      m_pRenderTarget->SetTransform(translationToShooter * lastMatrix);
      m_pRenderTarget->DrawBitmap(
        m_pShooterBitmap,
        D2D1::RectF(
          -size.width / 2, -size.height / 2,
          size.width / 2,  size.height / 2
        )
      );
    }

    // Drawing missile.
    if (m_MissileState != MissileState::HIT) {
      const D2D1_POINT_2F& translatedCoord =
        m_MissileState == MissileState::FLYING ? transformedLButtonUp : transformedMouseMove;
      const float r = Dist(translatedCoord);
      const float s = translatedCoord.x / r;
      const float c = -translatedCoord.y / r;
      const D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F(
        c,    s,
        -s,   c,
        0.0F, 0.0F
      );

      const float length = m_MissileAnimation.GetValue(missileAnimationTime);
      // 현재 시간에 해당하는 기하 길이에 일치하는 이동 동선 상의 지점을 얻음.
      m_pMissileAnimationGeometry->ComputePointAtLength(length, rotation * translationToShooter, &missileCoord, nullptr);
      // 미사일이 이동 동선을 따라가도록 함.
      const D2D1_MATRIX_3X2_F missileMatrix =
        rotation * D2D1::Matrix3x2F::Translation(missileCoord.x, missileCoord.y);

      // Event processing related with the missile.
      if (m_GameState == GameState::PLAYING) {
        const FLOAT d = Dist(rockCoord, missileCoord);
        if (d <= 100.0F) {
          // The missile hit the rock.
          m_MissileState = MissileState::HIT;
          m_GameState = GameState::SUCCEESS;
          IssueSeagullEvent(SeagullEvent::ROCK_DESTROYED);
          m_SeagullStartCoord = rockCoord;
          seagullAnimationTime = 0.0F;

          m_pHitVoice->Stop();
          m_pHitVoice->FlushSourceBuffers();
          m_pHitVoice->SubmitSourceBuffer(&m_HitBuffer);
          m_pHitVoice->Start();
        } else if (d <= 250.0F) {
          // The missile come close to the rock.
          IssueSeagullEvent(SeagullEvent::MISSILE_NEAR);
        }
      }

      m_pRenderTarget->SetTransform(missileMatrix * lastMatrix);
      m_pRenderTarget->FillGeometry(m_pMissileDrawingGeometry, m_pRedBrush);
    }

    // Drawing black hole.
    m_pRenderTarget->SetTransform(translationToBlackHole * lastMatrix);
    m_pRenderTarget->FillEllipse(
      D2D1::Ellipse(D2D1::Point2F(0.0F, 0.0F), 50.0F, 50.0F), m_pDarkBlueBrush
    );

    // Text.
    {
      WCHAR szText[BUFSIZ];
      swprintf(
        szText, std::size(szText),
        L"F2 - Mute\nF3 - Volume Down\nF4 - Volume Up\nVolume: %d%s\nFSM state: %s",
        m_VolumeLog,
        m_IsMuted ? L"(MUTE)" : L"",
        m_pSeagullStateWStr
      );

      m_pRenderTarget->SetTransform(translationToTextInfo * lastMatrix);
      m_pRenderTarget->DrawText(
        szText,
        wcslen(szText),
        m_pTextFormat,
        D2D1::RectF(0.0F, -500.0F, 500.0F, 0.0F),
        m_pYellowBrush
      );
    }

    // 그리기 연산들을 제출함.
    hr = m_pRenderTarget->EndDraw();

    if (hr == D2DERR_RECREATE_TARGET) {
      hr = S_OK;
      DiscardDeviceResources();
    }

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    float elapsedTime = 
      (currentTime.QuadPart - m_PrevTime.QuadPart) /
      static_cast<float>(m_Frequency.QuadPart);
    m_PrevTime = currentTime;

    if (rockAnimationTime >= m_RockAnimation.GetDuration()) {
      // 우주 암석 애니메이션의 끝에 도달하면 다시 처음으로 되돌려서 반복되도록 함.
      rockAnimationTime = 0.0F;
    } else {
      // 아니면 시간 추가.
      rockAnimationTime += elapsedTime;
    }

    if (missileAnimationTime >= m_MissileAnimation.GetDuration()) {
      // 미사일 애니메이션의 끝에 도달하면
      // 발사되기 전의 우주선에 장착된 모습으로 되돌림.
      missileAnimationTime = 0.0F;
      m_MissileState = MissileState::READY;
      m_pMissileReadyVoice->SubmitSourceBuffer(&m_MissileReadyBuffer);
      // 갈매기 이벤트 발생.
      IssueSeagullEvent(SeagullEvent::MISSILE_MISSED);
    } else if (m_MissileState != MissileState::READY) {
      // 아니면 시간 추가.
      missileAnimationTime += elapsedTime;
    }

    if (seagullAnimationTime >= seagullLeavingTime) {
      // 갈매기 애니메이션의 끝에 도달하면 이벤트 발생.
      IssueSeagullEvent(SeagullEvent::FAR_AWAY);
      seagullAnimationTime = 0.0F;
    } else if (m_SeagullState == SeagullState::LEAVING) {
      // 아니면 시간 추가.
      seagullAnimationTime += elapsedTime;
    }
  }

  return hr;
}

void DemoApp::OnResize(UINT width, UINT height) {
  if (m_pRenderTarget) {
    D2D1_SIZE_U size;
    size.width = width;
    size.height = height;

    m_pRenderTarget->Resize(size);
  }
}

HRESULT DemoApp::LoadWave(LPWSTR filename, IXAudio2SourceVoice** ppsv, XAUDIO2_BUFFER* buf) {
  HRESULT hr;

  CWaveFile wav;
  hr = wav.Open(filename, nullptr, WAVEFILE_READ);
  if (SUCCEEDED(hr)) {
    buf->AudioBytes = wav.GetSize();
    buf->Flags = XAUDIO2_END_OF_STREAM;
    BYTE* pbWaveData = new BYTE[buf->AudioBytes];
    buf->pAudioData = pbWaveData;
    hr = wav.Read(
      pbWaveData,
      buf->AudioBytes,
      reinterpret_cast<DWORD*>(&buf->AudioBytes)
    );
    wav.Close();
    hr = m_pXAudio2->CreateSourceVoice(ppsv, wav.GetFormat());
    (*ppsv)->Start();
  }

  return hr;
}

void DemoApp::IssueSeagullEvent(SeagullEvent ev) {
  switch (m_SeagullState) {
  case SeagullState::IN_PEACE :
    if (ev == SeagullEvent::MISSILE_LAUNCHED) {
      m_SeagullState = SeagullState::AWARE;
      m_ppSeagullBitmap = &m_pSeagullAwareBitmap;
      m_pSeagullStateWStr = L"AWARE";
    }
  break;

  case SeagullState::AWARE :
    if (ev == SeagullEvent::MISSILE_NEAR) {
      m_SeagullState = SeagullState::EVACUATING;
      m_ppSeagullBitmap = &m_pSeagullFlyingBitmap;
      m_pSeagullStateWStr = L"EVACUATING";
    }
  break;

  case SeagullState::EVACUATING :
    switch (ev) {
    case SeagullEvent::MISSILE_MISSED :
      m_SeagullState = SeagullState::AWARE;
      m_ppSeagullBitmap = &m_pSeagullAwareBitmap;
      m_pSeagullStateWStr = L"AWARE";
    break;

    case SeagullEvent::ROCK_DESTROYED :
      m_SeagullState = SeagullState::LEAVING;
      m_ppSeagullBitmap = &m_pSeagullFlyingBitmap;
      m_pSeagullStateWStr = L"LEAVING";
    break;
    }
  break;

  case SeagullState::LEAVING :
    if (ev == SeagullEvent::FAR_AWAY) {
      m_SeagullState = SeagullState::GONE;
      m_ppSeagullBitmap = nullptr;
      m_pSeagullStateWStr = L"LEFT";
    }
  break;
  }
}

float DemoApp::GetVolumeLinear() const {
  if (m_IsMuted || m_VolumeLog == 0) {
    return 0.0F;
  } else {
    return 2.0F / pow(sqrt(2.0F), 10 - m_VolumeLog);
  }
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  LRESULT result = 0;

  if (message == WM_CREATE) {
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
    DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(pcs->lpCreateParams);

    SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

    result = 1;
  } else {
    DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(
      static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

    bool wasHandled = false;

    if (pDemoApp) {
      switch (message) {
      case WM_SIZE :
        {
          UINT width = LOWORD(lParam);
          UINT height = HIWORD(lParam);
          pDemoApp->OnResize(width, height);
        }
        result = 0;
        wasHandled = true;
      break;

      case WM_DISPLAYCHANGE :
        {
          InvalidateRect(hwnd, nullptr, FALSE);
        }
        result = 0;
        wasHandled = true;
      break;

      case WM_PAINT :
        {
          pDemoApp->OnRender();
          // 여기에서 ValidateRect를 호출하지 말아야 OnRender 함수가 계속 반복 호출됨
        }
        result = 0;
        wasHandled = true;
      break;

      case WM_MOUSEMOVE :
        pDemoApp->m_MouseMove.x = LOWORD(lParam);
        pDemoApp->m_MouseMove.y = HIWORD(lParam);
        result = 0;
        wasHandled = true;
      break;

      case WM_LBUTTONUP :
        if (pDemoApp->m_MissileState == MissileState::READY) {
          pDemoApp->m_MissileState = MissileState::FLYING;
          pDemoApp->IssueSeagullEvent(SeagullEvent::MISSILE_LAUNCHED);
          pDemoApp->m_LButtonUp.x = LOWORD(lParam);
          pDemoApp->m_LButtonUp.y = HIWORD(lParam);
          pDemoApp->m_pMissileLaunchVoice->SubmitSourceBuffer(&pDemoApp->m_MissileLaunchBuffer);
        } else {
          pDemoApp->m_pMissileNotReadyVoice->Stop();
          pDemoApp->m_pMissileNotReadyVoice->FlushSourceBuffers();
          pDemoApp->m_pMissileNotReadyVoice->Start();
          pDemoApp->m_pMissileNotReadyVoice->SubmitSourceBuffer(&pDemoApp->m_MissileNotReadyBuffer);
        }
        result = 0;
        wasHandled = true;
      break;
      
      case WM_KEYDOWN :
        switch (wParam) {
        case VK_F2 :
          // Mute toggle
          pDemoApp->m_IsMuted = !pDemoApp->m_IsMuted;
        break;

        case VK_F3 :
          // Volume down
          if (pDemoApp->m_VolumeLog <= 0) {
            pDemoApp->m_VolumeLog = 0;
          } else {
            --pDemoApp->m_VolumeLog;
          }
        break;

        case VK_F4 :
          // Volume up
          if (pDemoApp->m_VolumeLog >= 10) {
            pDemoApp->m_VolumeLog = 10;
          }
          else {
            ++pDemoApp->m_VolumeLog;
          }
        break;
        }
        pDemoApp->m_pMasterVoice->SetVolume(pDemoApp->GetVolumeLinear());
        result = 0;
        wasHandled = true;
      break;

      case WM_DESTROY :
        PostQuitMessage(0);
        result = 1;
        wasHandled = true;
      break;
      }
    }

    if (!wasHandled) {
      result = DefWindowProc(hwnd, message, wParam, lParam);
    }
  }

  return result;
}

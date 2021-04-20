#include "main.h"
// Student Hedaer File
#include <vector>
#include "student.h"
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")

/* 최대 상자, 학생 수 */
#define MAX_RECTANGLE 10
#define MAX_STUDENT 10

/* 현재 윈도우 창의 크기 */
FLOAT winWidth;
FLOAT winHeight;

/* 왼쪽,오른쪽 마우스 클릭중인지 확인 */
BOOL leftMouseClicking = false;

/* 왼쪽, 현재 마우스 DOWN,UP 확인 포인트 */
D2D_POINT_2F leftMouseDownPosition, leftMouseUpPosition;
D2D_POINT_2F currentMousePosition;

/* 드래그중인 상자의 크기 비율, 회전 각도 */
FLOAT scale = 1.0f;
FLOAT rotate = 0.0f;

/* 학생 구조체 배열 */
std::vector<Student> studentV;

// Application entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
  if (SUCCEEDED(CoInitialize(NULL))) {
    {
      DemoApp app;

      if (SUCCEEDED(app.Initialize(hInstance))) {
        app.RunMessageLoop();
      }
    }
    CoUninitialize();
  }

  return 0;
}

// DemoApp constructor. Initialize members.
DemoApp::DemoApp()
    : m_hwnd(NULL),
      m_pD2DFactory(NULL),
      m_pRenderTarget(NULL),
      m_pStrokeStyleDash(NULL),
      m_pGridPatternBitmapBrush(NULL),
      m_pDarkBlueShapeBrush(NULL),
      m_pBlackShapeBrush(NULL),
      m_pFillLightGrayBrush(NULL),
      m_pFillGrayBrush(NULL),
      m_pTextBrush(NULL),
      m_pDWriteFactory(NULL),
      m_pTextFormat(NULL),
      m_smallRect(D2D1::Rect(475.0f, 50.0f, 525.0f, 100.0f)) {}

// DemoApp destructor. Releases the application's resources.
DemoApp::~DemoApp() {
  // 장치 독립 자원들을 반납
  SAFE_RELEASE(m_pD2DFactory);
  // 장치 의존 자원들을 반납
  SAFE_RELEASE(m_pRenderTarget);
  SAFE_RELEASE(m_pStrokeStyleDash);
  SAFE_RELEASE(m_pGridPatternBitmapBrush);
  SAFE_RELEASE(m_pBlackShapeBrush);
  SAFE_RELEASE(m_pDarkBlueShapeBrush);
  SAFE_RELEASE(m_pFillLightGrayBrush);
  SAFE_RELEASE(m_pFillGrayBrush);
  SAFE_RELEASE(m_pFilllRainbowBrush[0]);
  SAFE_RELEASE(m_pFilllRainbowBrush[1]);
  SAFE_RELEASE(m_pFilllRainbowBrush[2]);
  SAFE_RELEASE(m_pFilllRainbowBrush[3]);
  SAFE_RELEASE(m_pFilllRainbowBrush[4]);
  SAFE_RELEASE(m_pFilllRainbowBrush[5]);
  SAFE_RELEASE(m_pFilllRainbowBrush[6]);
  SAFE_RELEASE(m_pFilllRainbowBrush[7]);
  SAFE_RELEASE(m_pFilllRainbowBrush[8]);
  SAFE_RELEASE(m_pFilllRainbowBrush[9]);
  SAFE_RELEASE(m_pTextBrush);
  SAFE_RELEASE(m_pDWriteFactory);
  SAFE_RELEASE(m_pTextFormat);
}

// Creates the application window and device-independent resources.
HRESULT DemoApp::Initialize(HINSTANCE hInstance) {
  HRESULT hr;

  // Initialize device-indpendent resources, such as the Direct2D factory.
  hr = CreateDeviceIndependentResources();
  if (SUCCEEDED(hr)) {
    // Register the window class.
    WNDCLASSEX wcex = {sizeof(WNDCLASSEX)};
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

    // Create the application window.
    m_hwnd = CreateWindow(L"D2DDemoApp", L"d190425_D2dRectStack",
                          WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                          1000, 700, NULL, NULL, hInstance, this);
    hr = m_hwnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
      ShowWindow(m_hwnd, SW_SHOWNORMAL);
      UpdateWindow(m_hwnd);
    }
  }

  return hr;
}

// This method is used to create resources which are not bound to any device.
// Their lifetime effectively extends for the duration of the app.
HRESULT DemoApp::CreateDeviceIndependentResources() {
  HRESULT hr = S_OK;

  // Create D2D factory
  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

  if (SUCCEEDED(hr)) {
    // Create a shared DirectWrite factory
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                             __uuidof(IDWriteFactory),
                             reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
  }

  if (SUCCEEDED(hr)) {
    // Create a DirectWrite text format object.
    hr = m_pDWriteFactory->CreateTextFormat(
        L"Verdana",  // The font family name.
        NULL,        // The font collection (NULL sets it to use the system font
                     // collection).
        DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 13.0f, L"en-us", &m_pTextFormat);
  }

  if (SUCCEEDED(hr)) {
    // Create a dashed stroke style.
    float dashes[] = {10.0f, 2.0f};
    hr = m_pD2DFactory->CreateStrokeStyle(
        D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT,
                                    D2D1_CAP_STYLE_FLAT, D2D1_LINE_JOIN_MITER,
                                    10.0f, D2D1_DASH_STYLE_CUSTOM, 0.0f),
        dashes, ARRAYSIZE(dashes), &m_pStrokeStyleDash);
  }

  return hr;
}

// This method creates resources which are bound to a particular D3D device.
// It's all centralized here, in case the resources need to be recreated in case
// of D3D device loss (eg. display change, remoting, removal of video card,
// etc).
HRESULT DemoApp::CreateDeviceResources() {
  HRESULT hr = S_OK;

  if (!m_pRenderTarget) {
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    // Create a Direct2D render target.
    hr = m_pD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

    if (SUCCEEDED(hr)) {
      // Create a brush for virtual shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::DarkBlue), &m_pDarkBlueShapeBrush);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for outlining the pre-transform shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackShapeBrush);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush to fill the area of an element.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::LightGray), &m_pFillLightGrayBrush);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush to fill the area of an element.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Gray), &m_pFillGrayBrush);
    }

    // test
    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[0] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Red), &m_pFilllRainbowBrush[0]);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[1] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Orange), &m_pFilllRainbowBrush[1]);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[2] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Yellow), &m_pFilllRainbowBrush[2]);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[3] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::YellowGreen), &m_pFilllRainbowBrush[3]);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[4] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Green), &m_pFilllRainbowBrush[4]);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[5] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::LightBlue), &m_pFilllRainbowBrush[5]);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[6] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Blue), &m_pFilllRainbowBrush[6]);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[7] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Navy), &m_pFilllRainbowBrush[7]);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[8] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Violet), &m_pFilllRainbowBrush[8]);
    }

    if (SUCCEEDED(hr)) {
      // Create a brush for Rainbow[9] shape.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::DarkViolet), &m_pFilllRainbowBrush[9]);
    }

    if (SUCCEEDED(hr)) {
      // Create a solid color brush for writing text.
      hr = m_pRenderTarget->CreateSolidColorBrush(
          D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrush);
    }

    if (SUCCEEDED(hr)) {
      hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
    }
  }

  return hr;
}

HRESULT DemoApp::CreateGridPatternBrush(ID2D1RenderTarget *pRenderTarget,
                                        ID2D1BitmapBrush **ppBitmapBrush) {
  HRESULT hr = S_OK;

  // Create a compatible render target.
  ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
  hr = pRenderTarget->CreateCompatibleRenderTarget(D2D1::SizeF(10.0f, 10.0f),
                                                   &pCompatibleRenderTarget);

  if (SUCCEEDED(hr)) {
    // Draw a pattern.
    ID2D1SolidColorBrush *pGridBrush = NULL;
    hr = pCompatibleRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f), &pGridBrush);

    if (SUCCEEDED(hr)) {
      pCompatibleRenderTarget->BeginDraw();
      pCompatibleRenderTarget->FillRectangle(
          D2D1::RectF(0.0f, 0.0f, 10.0f, 1.0f), pGridBrush);
      pCompatibleRenderTarget->FillRectangle(
          D2D1::RectF(0.0f, 0.0f, 1.0f, 10.0f), pGridBrush);
      hr = pCompatibleRenderTarget->EndDraw();

      if (hr == D2DERR_RECREATE_TARGET) {
        // Discard all of the domain resources on this error.
        DiscardDeviceResources();
      }
      if (SUCCEEDED(hr)) {
        // Retrieve the bitmap from the render target.
        ID2D1Bitmap *pGridBitmap = NULL;
        hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);

        if (SUCCEEDED(hr)) {
          // Create the bitmap brush.
          hr = m_pRenderTarget->CreateBitmapBrush(
              pGridBitmap,
              D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP,
                                          D2D1_EXTEND_MODE_WRAP),
              ppBitmapBrush);

          pGridBitmap->Release();
        }
      }

      pGridBrush->Release();
    }

    pCompatibleRenderTarget->Release();
  }

  return hr;
}

// Discard device-specific resources which need to be recreated when a Direct3D
// device is lost.
void DemoApp::DiscardDeviceResources() {
  SAFE_RELEASE(m_pRenderTarget);
  SAFE_RELEASE(m_pDarkBlueShapeBrush);
  SAFE_RELEASE(m_pBlackShapeBrush);
  SAFE_RELEASE(m_pFillLightGrayBrush);
  SAFE_RELEASE(m_pFillGrayBrush);
  SAFE_RELEASE(m_pFilllRainbowBrush[0]);
  SAFE_RELEASE(m_pFilllRainbowBrush[1]);
  SAFE_RELEASE(m_pFilllRainbowBrush[2]);
  SAFE_RELEASE(m_pFilllRainbowBrush[3]);
  SAFE_RELEASE(m_pFilllRainbowBrush[4]);
  SAFE_RELEASE(m_pFilllRainbowBrush[5]);
  SAFE_RELEASE(m_pFilllRainbowBrush[6]);
  SAFE_RELEASE(m_pFilllRainbowBrush[7]);
  SAFE_RELEASE(m_pFilllRainbowBrush[8]);
  SAFE_RELEASE(m_pFilllRainbowBrush[9]);
  SAFE_RELEASE(m_pTextBrush);
  SAFE_RELEASE(m_pGridPatternBitmapBrush);
}

// Main window message loop
void DemoApp::RunMessageLoop() {
  MSG msg;

  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

// Called whenever the application needs to display the client window.
// Note that this function will automatically discard device-specific resources
// if the Direct3D device disappears during execution, and will recreate the
// resources the next time it's invoked.
HRESULT DemoApp::OnRender() {
  HRESULT hr = CreateDeviceResources();

  if (SUCCEEDED(hr)) {
    // Get ready to draw.
    m_pRenderTarget->BeginDraw();

    // Reset to the identity transform.
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // Clear the render target contents.
    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

    m_pRenderTarget->FillRectangle(
        D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height),
        m_pGridPatternBitmapBrush);

    m_pFillLightGrayBrush->SetOpacity(0.5f);

    WriteActionInfo();      // Write mouse cursor position, rectangle rotation &
                            // resize, etc...
    DrawSmallRectangle();   // Create a Small Rectangle, on top
    DrawMovingRectangle();  // Create a Moving Rectangle
    DrawRealRectangle();    // Create a Real Rectangle, from bottom to top
    WriteStudentInfo();     // Write student name, grade
    DrawVirtualRectangle();  // Create a Vitual Rectangle

    m_pRenderTarget->EndDraw();  // Commit the drawing operations.
  }

  if (hr == D2DERR_RECREATE_TARGET) {
    hr = S_OK;
    DiscardDeviceResources();
  }

  return hr;
}

// mouse cursor position, rectangle rotation & resize
void DemoApp::WriteActionInfo() {
  m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

  WCHAR szText[250];
  swprintf_s(
      szText,
      L"마우스x:%f\n마우스y:%f\n-----------------------\n마우스왼쪽 눌림 : "
      L"%d\n눌린x:%f\n눌린y:%f\n-----------------------\n상자의 개수 : "
      L"%d\n-----------------------\n크기비율:%f\nSizex:%f\nSizeY:%f\n---------"
      L"--------------\n회전각:%f\n",
      currentMousePosition.x, currentMousePosition.y, leftMouseClicking,
      leftMouseDownPosition.x, leftMouseDownPosition.y, studentV.size(), scale,
      (100 * scale), (25 * scale), rotate);

  m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat,
                            D2D1::RectF(10.0f, 10.0f, 150.0f, 150.0f),
                            m_pTextBrush);

  m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

// Draw small rectangle on top of screen
void DemoApp::DrawSmallRectangle() {
  if (!leftMouseClicking && studentV.size() < MAX_RECTANGLE) {
    m_pDarkBlueShapeBrush->SetOpacity(0.4);

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // Fill the rectangle.
    m_pRenderTarget->FillRectangle(GetSmallRect(), m_pFillLightGrayBrush);

    // Draw the rectangle.
    m_pRenderTarget->DrawRectangle(GetSmallRect(), m_pDarkBlueShapeBrush, 1.0f,
                                   m_pStrokeStyleDash);
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
  } else if (leftMouseClicking && studentV.size() < MAX_RECTANGLE &&
             !IsIn(leftMouseDownPosition, GetSmallRect())) {
    m_pDarkBlueShapeBrush->SetOpacity(0.4);

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // Fill the rectangle.
    m_pRenderTarget->FillRectangle(GetSmallRect(), m_pFillLightGrayBrush);

    // Draw the rectangle.
    m_pRenderTarget->DrawRectangle(GetSmallRect(), m_pDarkBlueShapeBrush, 1.0f,
                                   m_pStrokeStyleDash);
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
  }
}

// Draw virtual rectangle
void DemoApp::DrawVirtualRectangle() {
  if (studentV.size() < MAX_RECTANGLE &&
      !(leftMouseClicking && IsIn(leftMouseDownPosition, GetVirtualRect()))) {
    m_pDarkBlueShapeBrush->SetOpacity(0.3);
    m_pFillLightGrayBrush->SetOpacity(0.1);

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // Fill the rectangle.
    m_pRenderTarget->FillRectangle(GetVirtualRect(), m_pFillLightGrayBrush);

    // Draw the rectangle.
    m_pRenderTarget->DrawRectangle(GetVirtualRect(), m_pDarkBlueShapeBrush,
                                   1.0f, m_pStrokeStyleDash);
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
  }
}

// Draw Moving rectangle
void DemoApp::DrawMovingRectangle() {
  auto smallRectMiddle = (GetSmallRect().top + GetSmallRect().bottom) / 2;
  auto virtualRectMiddle = (GetVirtualRect().top + GetVirtualRect().bottom) / 2;
  auto topRectMiddle = (GetTopRect().top + GetTopRect().bottom) / 2;


  // 상자 추가를 위해 드래그 할때의 움직임
  if (leftMouseClicking && studentV.size() < 10 &&
      IsIn(leftMouseDownPosition, GetSmallRect())) {
    // 상자가 vitualbox 안으로 들어가면 종료
    if (IsIn(currentMousePosition, GetVirtualRect())) {
      return;
    }

    // 크기 변환
    scale = 3 *
            (currentMousePosition.y - smallRectMiddle +
             (virtualRectMiddle - smallRectMiddle) / 3) /
            (virtualRectMiddle - smallRectMiddle);
    // 회전 변환
    rotate = 360.0f * (currentMousePosition.y - smallRectMiddle) /
             (virtualRectMiddle - smallRectMiddle);

    m_pDarkBlueShapeBrush->SetOpacity(0.3);
    m_pFilllRainbowBrush[studentV.size()]->SetOpacity(0.1);

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // Create a rectangle.
    D2D1_RECT_F rectangle = D2D1::Rect(
        currentMousePosition.x - 25.0F, currentMousePosition.y - 25.0F,
        currentMousePosition.x + 25.0F, currentMousePosition.y + 25.0F);

    // Apply the scale transform to the render target.
    m_pRenderTarget->SetTransform(
        D2D1::Matrix3x2F::Scale(D2D1::Size(scale, 1.0F), currentMousePosition) *
        D2D1::Matrix3x2F::Rotation(rotate, currentMousePosition));

    // Fill the rectangle.
    m_pRenderTarget->FillRectangle(rectangle,
                                   m_pFilllRainbowBrush[studentV.size()]);

    // Draw the rectangle.
    m_pRenderTarget->DrawRectangle(rectangle, m_pDarkBlueShapeBrush, 1.0f,
                                   m_pStrokeStyleDash);
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
  }
  // 상자 삭제를 위해 드래그 할때의 움직임
  else if (leftMouseClicking && studentV.size() > 0 &&
           IsIn(leftMouseDownPosition, GetTopRect())) {
    // Small Rect로 드래그하면 종료
    if (IsIn(currentMousePosition, GetSmallRect())) {
      studentV.pop_back();

      leftMouseClicking = false;
      rotate = 0.0f;
      return;
    }

    // 크기 변환
    scale = 3 *
            (currentMousePosition.y - smallRectMiddle +
             (topRectMiddle - smallRectMiddle) / 3) /
            (topRectMiddle - smallRectMiddle);
    // 회전 변환
    rotate = 360.0f * (currentMousePosition.y - smallRectMiddle) /
             (topRectMiddle - smallRectMiddle);

    m_pFilllRainbowBrush[studentV.size() - 1]->SetOpacity(1.0);
    m_pBlackShapeBrush->SetOpacity(0.5);

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // Create a rectangle.
    D2D1_RECT_F rectangle = D2D1::Rect(
        currentMousePosition.x - 25.0f, currentMousePosition.y - 25.0f,
        currentMousePosition.x + 25.0f, currentMousePosition.y + 25.0f);

    // Apply the scale transform to the render target.
    m_pRenderTarget->SetTransform(
        D2D1::Matrix3x2F::Scale(D2D1::Size(scale, 1.0F), currentMousePosition) *
        D2D1::Matrix3x2F::Rotation(rotate, currentMousePosition));

    // Fill the rectangle.
    m_pRenderTarget->FillRectangle(rectangle,
                                   m_pFilllRainbowBrush[studentV.size() - 1]);

    // Draw the rectangle.
    m_pRenderTarget->DrawRectangle(rectangle, m_pBlackShapeBrush, 1.0f);
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
  }
}

// Draw real rectangle on bottom of screen
void DemoApp::DrawRealRectangle() {
  D2D1_RECT_F rectangle;

  // 원래 있던것들 그리기
  for (int i = 0; i < studentV.size(); i++) {
    // 맨 윗상자가 눌렸을 경우 제외
    if (leftMouseClicking && i == (studentV.size() - 1) &&
        IsIn(leftMouseDownPosition, GetTopRect())) {
      break;
    }

    m_pBlackShapeBrush->SetOpacity(1.0);
    m_pFilllRainbowBrush[i]->SetOpacity(1.0);
    // m_pFillGrayBrush->SetOpacity(1.0);

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // Create a rectangle.
    rectangle = D2D1::Rect(400.0f, winHeight - (50.0f * (i + 1)), 600.0f,
                           winHeight - (50.0f * i));

    // Fill the rectangle.
    // m_pRenderTarget->FillRectangle(rectangle, m_pFillGrayBrush);
    m_pRenderTarget->FillRectangle(rectangle, m_pFilllRainbowBrush[i]);

    // Draw the rectangle.
    m_pRenderTarget->DrawRectangle(rectangle, m_pBlackShapeBrush, 1.0f);
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
  }

  // 새로운거 그리기
  if (studentV.size() < MAX_RECTANGLE && leftMouseClicking &&
      IsIn(currentMousePosition, GetVirtualRect()) &&
      IsIn(leftMouseDownPosition, GetSmallRect())) {
    m_pBlackShapeBrush->SetOpacity(1.0);
    m_pFilllRainbowBrush[studentV.size()]->SetOpacity(1.0);

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // Fill the rectangle.
    m_pRenderTarget->FillRectangle(GetVirtualRect(),
                                   m_pFilllRainbowBrush[studentV.size()]);

    // Draw the rectangle.
    m_pRenderTarget->DrawRectangle(GetVirtualRect(), m_pBlackShapeBrush, 1.0f);
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    MakeStudent();

    leftMouseClicking = false;
    leftMouseDownPosition.x = 0.0f;
    leftMouseDownPosition.y = 0.0f;
  }
}

// Make student struct
void DemoApp::MakeStudent() { studentV.push_back(::MakeStudent()); }

// Write Student Info (grade, name)
void DemoApp::WriteStudentInfo() {
  if (studentV.size() != 0) {
    for (int i = 0; i < studentV.size(); i++) {
      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

      Student tmp = studentV.at(i);

      WCHAR szText[200];

      const char *multiByte = tmp.name.c_str();
      TCHAR UniCode[15];
      memset(UniCode, 0, sizeof(UniCode));
      MultiByteToWideChar(CP_ACP, MB_COMPOSITE, multiByte, -1, UniCode, 15);

      swprintf_s(szText, L"%s  %d\n", UniCode, tmp.grade);

      // 맨위의 상자가 눌렸을 경우
      if (leftMouseClicking && i == (studentV.size() - 1) &&
          IsIn(leftMouseDownPosition, GetTopRect())) {
        // Apply the rotation transform to the render target.
        m_pRenderTarget->SetTransform(
            D2D1::Matrix3x2F::Rotation(rotate, currentMousePosition));

        m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat,
                                  D2D1::RectF(currentMousePosition.x - 40,
                                              currentMousePosition.y - 10,
                                              currentMousePosition.x + 100,
                                              currentMousePosition.y + 25),
                                  m_pTextBrush);
      }
      // 맨위의 상자가 눌려있지 않은 경우
      else {
        m_pRenderTarget->DrawText(
            szText, wcslen(szText), m_pTextFormat,
            D2D1::RectF(460, winHeight - (50 * (i + 1)) + 15, 600,
                        winHeight - (50 * i)),
            m_pTextBrush);
      }

      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    }
  }
}

// If the application receives a WM_SIZE message, this method resizes the render
// target appropriately.
void DemoApp::OnResize(UINT width, UINT height) {
  if (m_pRenderTarget) {
    // Note: This method can fail, but it's okay to ignore the error here -- the
    // error will be repeated on the next call to EndDraw.
    m_pRenderTarget->Resize(D2D1::SizeU(width, height));
  }
}

// Window message handler                                         *
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
  if (message == WM_CREATE) {
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
    DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

    ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

    return 1;
  }

  DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(
      static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

  if (pDemoApp) {
    switch (message) {
      case WM_SIZE: {
        winWidth = LOWORD(lParam);
        winHeight = HIWORD(lParam);
        pDemoApp->OnResize(winWidth, winHeight);
      }
        return 0;

      // 윈도우 창 크기 변환 막기
      case WM_GETMINMAXINFO: {
        MINMAXINFO *mmi = (MINMAXINFO *)lParam;
        mmi->ptMinTrackSize.x = 1000;
        mmi->ptMinTrackSize.y = 0;
        mmi->ptMaxTrackSize.x = 1000;
        mmi->ptMaxTrackSize.y = 800;
      }
        return 0;

      case WM_DISPLAYCHANGE: {
        InvalidateRect(hwnd, NULL, FALSE);
      }
        return 0;

      case WM_MOUSEMOVE: {
        if (!leftMouseClicking) {
          leftMouseDownPosition.x = 0;
          leftMouseDownPosition.y = 0;
        }

        currentMousePosition.x = LOWORD(lParam);
        currentMousePosition.y = HIWORD(lParam);
        InvalidateRect(hwnd, NULL, FALSE);
      }
        return 0;

      case WM_LBUTTONDOWN: {
        leftMouseUpPosition.x = 0.0f;
        leftMouseUpPosition.y = 0.0f;

        leftMouseClicking = true;
        leftMouseDownPosition.x = LOWORD(lParam);
        leftMouseDownPosition.y = HIWORD(lParam);

        InvalidateRect(hwnd, NULL, FALSE);
      }
        return 0;

      case WM_LBUTTONUP: {
        leftMouseUpPosition.x = LOWORD(lParam);
        leftMouseUpPosition.y = HIWORD(lParam);
        leftMouseClicking = false;
        scale = 1.0f;

        InvalidateRect(hwnd, NULL, FALSE);
      }
        return 0;

      case WM_PAINT: {
        pDemoApp->OnRender();
        ValidateRect(hwnd, NULL);
      }
        return 0;

      case WM_DESTROY: {
        PostQuitMessage(0);
      }
        return 1;
    }
  }
  return DefWindowProc(hwnd, message, wParam, lParam);
}

// ClientRect(그리기 영역의 위치와 크기)로부터 i(순서)에 따라 box의 위치와
// 크기를 결정한다
void DemoApp::GetRect(D2D1_RECT_F &boxRect, int i) const {
  FLOAT boxHeight = 50.0F;
  //(clientRect.bottom - clientRect.top) / (2 * nameScoreStack.maxSize());
  FLOAT gap = 0.0F;    // boxHeight;
  boxRect.left = 400;  // clientRect.left + 100;
  boxRect.top = winHeight - (i + 1) * (gap + boxHeight);
  boxRect.right = 600;  // clientRect.right - 100;
  boxRect.bottom = winHeight - gap - i * (gap + boxHeight);
}

// x, y 좌표가 rect 영역 안에 위치하는지 확인한다
bool DemoApp::IsIn(const D2D1_POINT_2F &point, const D2D1_RECT_F &rect) const {
  return rect.left <= point.x && point.x <= rect.right && rect.top <= point.y &&
         point.y <= rect.bottom;
}

const D2D1_RECT_F &DemoApp::GetSmallRect() const { return m_smallRect; }

const D2D1_RECT_F &DemoApp::GetVirtualRect() const {
  static D2D1_RECT_F ret;
  GetRect(ret, studentV.size());
  return ret;
}

const D2D1_RECT_F &DemoApp::GetTopRect() const {
  static D2D1_RECT_F ret;
  GetRect(ret, studentV.size() - 1);
  return ret;
}
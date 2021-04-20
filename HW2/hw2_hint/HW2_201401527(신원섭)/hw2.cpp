#include "hw2.h"
#define MAX_LOADSTRING 100
#define MAX_LAST_NAME 18
#define MAX_FIRST_NAME 21
typedef std::basic_string<TCHAR> tstring;

// 대화상자를 사용하기 위한 전역변수
HINSTANCE hInstance = NULL;      //프로그램의 인스턴스 핸들
HINSTANCE g_hInstance = NULL;
LPCTSTR lpTemplate;      //대화상자 템플릿의 ID
HWND hwnd;          //부모 윈도우의 윈도우 핸들

INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM); // 대화상자 선언

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 구조체 생성
typedef struct  Student{
	int grade;		//성적
	TCHAR* name;	//이름
};


// 스택 생성
std:: stack<Student, std::vector<Student>> temp1, mystack;
Student s;

// String을 TCHAR 형식으로 변환해주는 함수 [참고 : 티스토리 블로그]
TCHAR* StringToTCHAR(string& s)
{
	tstring tstr;
	const char* all = s.c_str();
	int len = 1 + strlen(all);
	wchar_t* t = new wchar_t[len];
	if (NULL == t) throw std::bad_alloc();
	mbstowcs(t, all, len);
	return (TCHAR*)t;
}

// 스택에 이름, 점수 넣어주는 함수
void Insert(){

	setlocale(LC_ALL, "Korean");

	char* lastnames[MAX_LAST_NAME] = { "김", "이", "박",
		"최", "강", "유",
		"원", "정", "오",
		"송", "서", "한",
		"홍", "고", "신",
		"조", "섭", "임" };		//랜덤 성씨
	char* firstnames[MAX_FIRST_NAME] = { "주", "지", "정",
		"수", "인", "철",
		"현", "석", "준",
		"은", "신", "범",
		"동", "훈", "영",
		"소", "하", "호",
		"진", "명", "윤" };	//랜덤 이름
	std::string name = "";

	//int tmp = rand() % 10001;
	srand((int)time(NULL));
	int grade = rand() % 101;
	name = lastnames[rand() % MAX_LAST_NAME];	//랜덤 성씨

	for (int j = 0; j < 2; j++) {		//성씨 제외한 이름은 2자
		name += firstnames[rand() % MAX_FIRST_NAME];
	}

	// 구조체 name에 저장된 성명을 넣는데 c_str을 이용해 String을 TCHAR 형태로 바꾸어 준다.
	s.name = StringToTCHAR(name);
	s.grade = grade;

	// 저장된 구조체를 스택에 넣는다.
	mystack.push(s);
}

/******************************************************************
*                                                                 *
*  WinMain                                                        *
*                                                                 *
*  Application entry point                                        *
*                                                                 *
******************************************************************/
/*왼쪽,오른쪽 마우스 클릭중인지 확인*/
BOOL L_inputting = false;
BOOL R_inputting = false;

/*왼쪽, 오른쪽, 현재마우스 DOWN,UP확인 포인트*/
D2D_POINT_2F L_p0, L_p1;
D2D_POINT_2F R_p0, R_p1;
D2D_POINT_2F current_p;

/*타원의 각도와 크기, 휠 정보를 저장하는 변수들*/
FLOAT Angle = 0.0f, oldAngle = 0.0f;
FLOAT Size = 1.0f, oldSize = 1.0f;
FLOAT wheel = 0.0f;

INT topbox = false;
// 박스의 위치 값
#define BOX_LEFT 250
#define BOX_TOP 790
#define BOX_RIGHT 450
#define BOX_BOTTOM 840

int stacksize = 0;

int WINAPI WinMain(
	HINSTANCE /* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */
	)
{
	// Ignore the return value because we want to run the program even in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
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

/******************************************************************
*                                                                 *
*  DemoApp::DemoApp constructor                                   *
*                                                                 *
*  Initialize member data                                         *
*                                                                 *
******************************************************************/

DemoApp::DemoApp() :
m_hwnd(NULL),
m_pD2DFactory(NULL),
m_pRenderTarget(NULL),
m_pStrokeStyleDash(NULL),
m_pGridPatternBitmapBrush(NULL),
m_pgrayBrush(NULL),
m_pOriginalShapeBrush(NULL),
m_pFillBrush(NULL),
m_pFillBrush2(NULL),
m_pFillBrush11(NULL),
m_pFillBrush12(NULL),
m_pFillBrush13(NULL),
m_pFillBrush14(NULL),
m_pFillBrush15(NULL),
m_pFillBrush16(NULL),
m_pFillBrush17(NULL),
m_pFillBrush18(NULL),
m_pFillBrush19(NULL),
m_pFillBrush20(NULL),
m_pTextBrush(NULL),
m_pDWriteFactory(NULL),
m_pTextFormat(NULL)
{
}

/******************************************************************
*                                                                 *
*  DemoApp::~DemoApp destructor                                   *
*                                                                 *
*  Release resources                                              *
*                                                                 *
******************************************************************/

DemoApp::~DemoApp()
{
	SafeRelease(&m_pgrayBrush);
	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pStrokeStyleDash);
	SafeRelease(&m_pGridPatternBitmapBrush);
	SafeRelease(&m_pOriginalShapeBrush);
	SafeRelease(&m_pFillBrush);
	SafeRelease(&m_pFillBrush2);
	SafeRelease(&m_pTextBrush);
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pTextFormat);
}

/******************************************************************
*                                                                 *
*  DemoApp::Initialize                                            *
*                                                                 *
*  Create application window and device-independent resources     *
*                                                                 *
******************************************************************/

HRESULT DemoApp::Initialize()
{
	HRESULT hr;

	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
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
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"신원섭 201401527";

		RegisterClassEx(&wcex);

		// Create the application window.
		m_hwnd = CreateWindow(
			L"신원섭 201401527",
			L"신원섭 201401527",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			700,
			900,
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
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

/******************************************************************
*                                                                 *
*  DemoApp::CreateDeviceIndependentResources                      *
*                                                                 *
*  This method is used to create resources which are not bound    *
*  to any device. Their lifetime effectively extends for the      *
*  duration of the app.                                           *
*                                                                 *
******************************************************************/

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// Create D2D factory
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&m_pD2DFactory
		);

	if (SUCCEEDED(hr))
	{
		// Create a shared DirectWrite factory
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
			);
	}

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",     // The font family name.
			NULL,           // The font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			13.0f,
			L"en-us",
			&m_pTextFormat
			);
	}

	// 점선으로 사용할 m_pStrokeStyleDash 생성
	if (SUCCEEDED(hr))
	{
		// Create a dashed stroke style.
		float dashes[] = { 10.0f, 2.0f };
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




/******************************************************************
*                                                                 *
*  DemoApp::CreateDeviceResources                                 *
*                                                                 *
*  This method creates resources which are bound to a particular  *
*  D3D device. It's all centralized here, in case the resources   *
*  need to be recreated in case of D3D device loss (eg. display   *
*  change, remoting, removal of video card, etc).                 *
*                                                                 *
******************************************************************/

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
			);

		// Create a Direct2D render target.
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
			);
		if (SUCCEEDED(hr))
		{
			// Create a brush for outlining the pre-transform shape.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Gray),
				&m_pgrayBrush
			);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush for outlining the pre-transform shape.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::DarkBlue),
				&m_pOriginalShapeBrush
				);
		}


		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Red),
				&m_pFillBrush11
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Orange),
				&m_pFillBrush12
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::YellowGreen),
				&m_pFillBrush13
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Green),
				&m_pFillBrush14
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Blue),
				&m_pFillBrush15
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::DarkCyan),
				&m_pFillBrush16
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::MediumVioletRed),
				&m_pFillBrush17
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(9050202),
				&m_pFillBrush18
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Violet),
				&m_pFillBrush19
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::DarkViolet),
				&m_pFillBrush20
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(50.0f, 0.0f, 0.0f, 0.69f),
				&m_pFillBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(0.5f, 0.5f, 0.5f, 0.4f),
				&m_pFillBrush2
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a solid color brush for writing text.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pTextBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a solid color brush for writing text.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::White),
				&m_pTextBrush2
				);
		}

		if (SUCCEEDED(hr))
		{
			hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
		}
	}

	return hr;
}

HRESULT DemoApp::CreateGridPatternBrush( //격자 모양 만들기
	ID2D1RenderTarget *pRenderTarget,
	ID2D1BitmapBrush **ppBitmapBrush
	)
{
	HRESULT hr = S_OK;

	// Create a compatible render target.
	ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
	hr = pRenderTarget->CreateCompatibleRenderTarget(
		D2D1::SizeF(10.0f, 10.0f),
		&pCompatibleRenderTarget
		);

	if (SUCCEEDED(hr))
	{
		// Draw a pattern.
		ID2D1SolidColorBrush *pGridBrush = NULL;
		hr = pCompatibleRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f),
			&pGridBrush
			);

		if (SUCCEEDED(hr))
		{
			pCompatibleRenderTarget->BeginDraw();
			pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 10.0f, 1.0f), pGridBrush);
			pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 1.0f, 10.0f), pGridBrush);
			hr = pCompatibleRenderTarget->EndDraw();

			if (hr == D2DERR_RECREATE_TARGET)
			{
				// Discard all of the domain resources on this error.
				DiscardDeviceResources();
			}
			if (SUCCEEDED(hr))
			{
				// Retrieve the bitmap from the render target.
				ID2D1Bitmap *pGridBitmap = NULL;
				hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);

				if (SUCCEEDED(hr))
				{
					// Create the bitmap brush.
					hr = m_pRenderTarget->CreateBitmapBrush(
						pGridBitmap,
						D2D1::BitmapBrushProperties(
						D2D1_EXTEND_MODE_WRAP,
						D2D1_EXTEND_MODE_WRAP),
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

/******************************************************************
*                                                                 *
*  DemoApp::DiscardDeviceResources                                *
*                                                                 *
*  Discard device-specific resources which need to be recreated   *
*  when a Direct3D device is lost.                                *
*                                                                 *
******************************************************************/

void DemoApp::DiscardDeviceResources()
{
	SafeRelease(&m_pgrayBrush);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pFillBrush);
	SafeRelease(&m_pFillBrush2);
	SafeRelease(&m_pFillBrush11);
	SafeRelease(&m_pFillBrush12);
	SafeRelease(&m_pFillBrush13);
	SafeRelease(&m_pFillBrush14);
	SafeRelease(&m_pFillBrush15);
	SafeRelease(&m_pFillBrush16);
	SafeRelease(&m_pFillBrush17);
	SafeRelease(&m_pFillBrush18);
	SafeRelease(&m_pFillBrush19);
	SafeRelease(&m_pFillBrush20);
	SafeRelease(&m_pOriginalShapeBrush);
	SafeRelease(&m_pTextBrush);
	SafeRelease(&m_pTextBrush2);
	SafeRelease(&m_pGridPatternBitmapBrush);
}

/******************************************************************
*                                                                 *
*  DemoApp::RunMessageLoop                                        *
*                                                                 *
*  Main window message loop                                       *
*                                                                 *
******************************************************************/

void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/******************************************************************
*                                                                 *
*  DemoApp::OnRender                                              *
*                                                                 *
*  Called whenever the application needs to display the client    *
*  window.                                                        *
*                                                                 *
*  Note that this function will automatically discard             *
*  device-specific resources if the Direct3D device disappears    *
*  during execution, and will recreate the resources the          *
*  next time it's invoked.                                        *
*                                                                 *
******************************************************************/

HRESULT DemoApp::OnRender()
{
	HRESULT hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		// Get ready to draw.
		m_pRenderTarget->BeginDraw();

		// Reset to the identity transform.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// Clear the render target contents.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		m_pRenderTarget->FillRectangle(
			D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height),
			m_pGridPatternBitmapBrush
			);

		m_pFillBrush->SetOpacity(0.2f);

		/* 사각형을 마우스로 조절하는 나만의 사각형그리기 함수 */
		MyDrawRect();
		if (stacksize != 0){
			write();
		}

		// Commit the drawing operations.
		m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}


void DemoApp::MyDrawRect()
{

	/* 윈도우의 센터 cX,cY */
	D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
	int cX = rtSize.width / 2;
	int cY = rtSize.height / 2;
	FLOAT current_Angle, current_Size;	// 그려지고 있는 타원의 회전각도와 크기

	/* 상자의 시작 위치 */
	int GAP = 800;

	/* 상자 그리기 */
	for (int i = 0; i < stacksize; i++){ // 상자 별로 색을 나누기 위함
		if (i == 0){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush11); }		//상자 너비 240, 높이 60
		if (i == 1){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush12); }
		if (i == 2){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush13); }
		if (i == 3){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush14); }
		if (i == 4){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush15); }
		if (i == 5){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush16); }
		if (i == 6){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush17); }
		if (i == 7){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush18); }
		if (i == 8){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush19); }
		if (i == 9){ m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush20); }
		m_pRenderTarget->DrawRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pOriginalShapeBrush);
		/* 상자 위치 바꾸기*/
		GAP -= 60;
	}
	/* 다음 상자가 들어갈 위치 표시 */
	if (R_inputting)
	{
		m_pRenderTarget->DrawRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pgrayBrush, 1.5f, m_pStrokeStyleDash);
		m_pRenderTarget->FillRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pFillBrush2);
	}

	/* 크기 공식 */
	/* (R_p1.y - R_p0.y) : rtSize.height/2 = current_Size : 1  */
	if (R_inputting)
		current_Size = 3 * (R_p1.y - R_p0.y) / (GAP - 45);				//상자가 들어갈 자리에 원래상자의 4배가 되도록 설정
	else
		current_Size = 0.75 * (L_p1.y - L_p0.y) / (GAP - 45);			//상자가 들어갈 자리에 원래상자의 1/4배가 되도록 설정
	Size = (1.0f + current_Size);	

	/* 각도 공식 */
	/* (L_p1.x - L_p0.x) : rtSize.width/2 = current_Angle : 360 */
	if (R_inputting) 
		current_Angle = 360 * (R_p1.y - R_p0.y) / (GAP - 45);
	else  
		current_Angle = 360 * (L_p1.y - L_p0.y) / (GAP - 45);
	Angle = (int)(oldAngle + current_Angle) % 360;	// 이전 타원각도 + 현재 타원각도
	

	if (L_inputting){ //밑의 상자 중 제일 위에 있는 상자를 클릭 했을 때
		//D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(Angle, D2D1::Point2F(350, cY - 80));
		D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(Size, 1.0f), D2D1::Point2F(350, 45)); //드래그 시 상자 사이즈 변경
		D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(Angle, D2D1::Point2F(350, 800-60*stacksize));
		D2D1_MATRIX_3X2_F move = D2D1::Matrix3x2F::Translation(L_p1.x - L_p0.x, L_p1.y - L_p0.y);

		m_pRenderTarget->SetTransform(scale*rotation*move);
			if (stacksize == 0) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 770, 470, 830), m_pFillBrush11); }		//상자 너비 240, 높이 60
			else if (stacksize == 1) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 710, 470, 770), m_pFillBrush12); }
			else if (stacksize == 2) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 650, 470, 710), m_pFillBrush13); }
			else if (stacksize == 3) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 590, 470, 650), m_pFillBrush14); }
			else if (stacksize == 4) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 530, 470, 590), m_pFillBrush15); }
			else if (stacksize == 5) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 470, 470, 530), m_pFillBrush16); }
			else if (stacksize == 6) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 410, 470, 470), m_pFillBrush17); }
			else if (stacksize == 7) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 350, 470, 410), m_pFillBrush18); }
			else if (stacksize == 8) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 290, 470, 350), m_pFillBrush19); }
			else if (stacksize == 9) { m_pRenderTarget->FillRectangle(D2D1::Rect(230, 230, 470, 290), m_pFillBrush20); }
			m_pRenderTarget->DrawRectangle(D2D1::Rect(230, GAP - 30, 470, GAP + 30), m_pgrayBrush);
	}

	/*타원의 크기와 각도 조절*/
	/*순서중요: Scale → Rotation -> Move */
	D2D1_MATRIX_3X2_F scale2 = D2D1::Matrix3x2F::Scale(D2D1::Size(Size, 1.0f), D2D1::Point2F(350, 45));												//드래그 시 상자 사이즈 변경
	D2D1_MATRIX_3X2_F rotation2 = D2D1::Matrix3x2F::Rotation(Angle, D2D1::Point2F(350, 45));
	D2D1_MATRIX_3X2_F move2 = D2D1::Matrix3x2F::Translation(R_p1.x - R_p0.x, R_p1.y - R_p0.y);

	
	// 마우스 움직일 때 같이 움직이게 함
	m_pRenderTarget->SetTransform(scale2*rotation2*move2);

	// 상자 생성 시작할 수 있는 작은 상자 부분
	if (R_inputting){
		m_pRenderTarget->DrawRectangle(D2D1::Rect(320, 15, 380, 75), m_pgrayBrush);
		for (int i = -1; i < stacksize; i++){
			//드로우 해서 움직이는 동안 가상 상자 색 변경
			if (i == -1){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush11); }
			if (i == 0){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush12); }
			if (i == 1){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush13); }
			if (i == 2){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush14); }
			if (i == 3){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush15); }
			if (i == 4){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush16); }
			if (i == 5){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush17); }
			if (i == 6){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush18); }
			if (i == 7){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush19); }
			if (i == 8){ m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush20); }
		}
	}
	else{//정사각형 희미한 상자
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->DrawRectangle(D2D1::Rect(320, 15, 380, 75), m_pgrayBrush, 1.5f, m_pStrokeStyleDash);
		m_pRenderTarget->FillRectangle(D2D1::Rect(320, 15, 380, 75), m_pFillBrush2);
	}


	/* 타원 인자 정보들 */
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	WCHAR szText[100];
	_swprintf(szText, L"201401527 신원섭\n마우스x:%f\n마우스y:%f\n회전각:%f\nSize:%f\n상자의 수:%d", current_p.x, current_p.y, Angle, Size, stacksize);

	m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, D2D1::RectF(10.0f, 10.0f, 150.0f, 150.0f), m_pTextBrush);


}

void DemoApp::write(){

	std::stack<Student,std::vector<Student>> temp = mystack;
	int GAP = 790;
	int height = 60;
	for (int loop = stacksize-1; loop >=0; loop--){
		m_pRenderTarget->DrawText(temp.top().name, lstrlen(temp.top().name), m_pTextFormat, D2D1::RectF(310.0f, GAP - (height*loop), 450.0f, GAP - (height*loop)), m_pTextBrush2);
		WCHAR szText[100];
		_swprintf(szText, L"%d", temp.top().grade);
		m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, D2D1::RectF(380.0f, GAP - (height*loop), 450.0f, GAP - (height*loop)), m_pTextBrush2);
		//GAP += 60;
		temp.pop();
	}

}



/******************************************************************
*                                                                 *
*  DemoApp::OnResize                                              *
*                                                                 *
*  If the application receives a WM_SIZE message, this method     *
*  resizes the render target appropriately.                       *
*                                                                 *
******************************************************************/

void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// Note: This method can fail, but it's okay to ignore the
		// error here -- the error will be repeated on the next call to
		// EndDraw.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

/******************************************************************
*                                                                 *
*  DemoApp::WndProc                                               *
*                                                                 *
*  Window message handler                                         *
*                                                                 *
******************************************************************/

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if (message == WM_CREATE)
	{

		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pDemoApp)
			);



		return 1;
	}



	DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
		::GetWindowLongPtrW(
		hwnd,
		GWLP_USERDATA
		)));

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


		case WM_GETMINMAXINFO:
		{
			//최대화, 최소화 사이즈 지정
			MINMAXINFO* mmi = (MINMAXINFO*)lParam;  //윈도우의 창크기
			mmi->ptMinTrackSize.x = 700;
			mmi->ptMinTrackSize.y = 900;
			mmi->ptMaxTrackSize.x = 700;
			mmi->ptMaxTrackSize.y = 900;
			return 0;
		}


		case WM_LBUTTONDOWN:
		{
			// 상자 회전 및 삭제를 위한 부분
			L_p0.x = L_p1.x = LOWORD(lParam);
			L_p0.y = L_p1.y = HIWORD(lParam);
			topbox = false;			//희미한 상자를 클릭 후 상자가 쌓이는 곳에 바로 클릭시 상자가 생기는 현상 방지
			// 클릭 위치가 최 상단 위치이면 스택사이즈를 줄인다.
			if (stacksize > 0) {
				if (L_p0.x > 230 && L_p0.x<470 && L_p0.y>(830 - (stacksize * 60)) && L_p0.y < (890 - (stacksize * 60))) { //쌓여있는 상자 중 가장 위의 상자 클릭
					L_inputting = true;
					stacksize--;
					temp1 = mystack;
					mystack.pop();
				}
			}

			// 상자 이동, 스케일을 위한 부분
			R_p0.x = R_p1.x = LOWORD(lParam);
			R_p0.y = R_p1.y = HIWORD(lParam);
			if (R_p0.x > 320 && R_p0.x < 380 && R_p0.y>15 && R_p0.y < 75) { //희미한 상자 클릭
				R_inputting = true;
				topbox = true;
			}
		}
			break;
		

		case WM_LBUTTONUP:
		{
			L_p0.x = L_p1.x = LOWORD(lParam);
			L_p0.y = L_p1.y = HIWORD(lParam);
			if (L_inputting) {				//직사각형 모양중 제일 위쪽 상자에서 클릭 했을 시
				if (L_p0.x > 320 && L_p0.x < 380 && L_p0.y>15 && L_p0.y < 75) {//상자가중 제일 위의 상자가 희미한 정사각형에 들어갔을때 삭제하기
					//정사각형 모양에 들어가면 아무것도 하지 않는다.(버튼을 눌렀을때 이미 스택크기를 줄이고pop실행을 했기 때문에)
				}
				else {//정사각형 모양에 들어가지 않으면 buttondown에서 했던 스택 사이즈를 다시 늘리고 top정보를 복원해 준다.
					stacksize++;
					mystack = temp1;
				}
			}
	
			L_inputting = false;
			L_p1.x = L_p0.x = 0.0f; // 각도의 변동이 없게하기 위함
			L_p1.y = L_p0.y = 0.0f;
			InvalidateRect(hwnd, NULL, FALSE);

			if (R_p1.x>230 && R_p1.x<470 && R_p1.y>770 - (stacksize * 60) && R_p1.y < 830 - (stacksize * 60)){  //상자를 밑에있는 희미한 상자에 놓았을때 상자 추가
				if (stacksize < 10){
					if (topbox == true){
						stacksize++;
						Insert();
						topbox = false;
					}
				}
				else
				{
					if (topbox == true){
						DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, About);
						topbox = false;
					}
				}
			}
			R_inputting = false;
			//oldSize = Size; // 전 타원의 크기를 저장
			R_p1.y = R_p0.y = 0.0f; // 크기의 변동이 없게하기 위함
			R_p1.x = R_p0.x = 0.0f;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;

		case WM_MOUSEMOVE:
			current_p.x = LOWORD(lParam);
			current_p.y = HIWORD(lParam);
			InvalidateRect(hwnd, NULL, FALSE);
			if (L_inputting)
			{
				L_p1.x = LOWORD(lParam);
				L_p1.y = HIWORD(lParam);
				InvalidateRect(hwnd, NULL, FALSE);
			}

			if (R_inputting)
			{
				R_p1.x = LOWORD(lParam);
				R_p1.y = HIWORD(lParam);
				InvalidateRect(hwnd, NULL, FALSE);
			}

			break;



		case WM_MOUSEWHEEL:
		{
			if ((SHORT)HIWORD(wParam) > 0)	//축소
				wheel -= 0.1f;
			else							//확대
				wheel += 0.1f;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;

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

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

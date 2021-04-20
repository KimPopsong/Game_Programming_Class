#include "HW2.h"

#define MAX_LOADSTRING 100
#define MAX_LAST_NAME 10
#define MAX_FIRST_NAME 15
#define MAX_STACK 6

#define WIDTH 200
#define HEIGHT 100
//상자 크기


//상자 생성, 삭제 제어 변수
BOOL Make_box = false;
BOOL Moving_box = false;
BOOL Remove_box = false;


/*현재 마우스 확인 포인트*/
D2D_POINT_2F current_p;
D2D_POINT_2F moving_p;


/*타원의 각도와 크기, 휠 정보를 저장하는 변수들*/
FLOAT Angle = 0.0f;
FLOAT Size = 1.0f;


D2D1_SIZE_F rtSize;
UINT winWidth;
UINT winHeight;

//학생정보를 관리할 스택 생성 및 이름 설정
Student tempStudent;
std::string name[10] = { "곽용준", "우세훈", "김광표", "이영진" , "전정호", "김승윤", "한정훈", "김규형","안준혁" , "김민석" };
std::vector<Student> stack;
std::vector<Student>::iterator it;
int sp = -1;
int boxs = 0;

//스택 함수 설정
bool isFull() {
	return sp == MAX_STACK - 1;
}

bool isEmpty() {
	return sp == -1;
}

float width_rate;
float height_rate;
//Direct2d 가로, 세로 비율 

float move_rate = 0;
//가로, 세로 길이 제어 변수

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

DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pD2DFactory(NULL),
	m_pRenderTarget(NULL),
	m_pStrokeStyleDash(NULL),
	m_pGridPatternBitmapBrush(NULL),
	m_pFillBrush(NULL),
	m_pTextBrush(NULL),
	m_pTextBrush2(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL) 
{

}

DemoApp::~DemoApp()
{
	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pStrokeStyleDash);
	SafeRelease(&m_pGridPatternBitmapBrush);

	SafeRelease(&m_pFillBrush);
	SafeRelease(&m_pTextBrush);
	SafeRelease(&m_pTextBrush2);
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pTextFormat);
	for (int i = 0; i < 10; i++)
		SafeRelease(&m_pOriginalShapeBrush[i]);
}

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
		wcex.lpszClassName = L"HW2_201401414(곽용준)";

		RegisterClassEx(&wcex);

		// Create the application window.
		m_hwnd = CreateWindow(
			L"HW2_201401414(곽용준)",
			L"HW2_201401414(곽용준)",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			2200,
			1400,
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

	srand((unsigned int)time(NULL));

	return hr;
}

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
			L"ko-kr",
			&m_pTextFormat
		);
	}

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

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;
	srand((unsigned int)time(NULL));

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

		float temp[3]; //색상제어변수

		for (int i = 0; i < 10; i++) {
			temp[0] = rand() % 101 / 100.0;
			temp[1] = rand() % 101 / 100.0;
			temp[2] = rand() % 101 / 100.0;
			if (SUCCEEDED(hr))
			{
				// Create a brush for outlining the pre-transform shape.
				hr = m_pRenderTarget->CreateSolidColorBrush(
					D2D1::ColorF(temp[0], temp[1], temp[2], 1),
					&m_pOriginalShapeBrush[i]
				);
			}
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of Top Box
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Gray),
				&m_pTopBrush
			);
			m_pTopBrush->SetOpacity(0.2f);
		}

		if (SUCCEEDED(hr))
		{
			// Create a brush to fill the area of an element.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Blue),
				&m_pFillBrush
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

HRESULT DemoApp::CreateGridPatternBrush(
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

void DemoApp::DiscardDeviceResources()
{
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pFillBrush);
	SafeRelease(&m_pTextBrush);
	SafeRelease(&m_pGridPatternBitmapBrush);
	for (int i = 0; i < 10; i++)
		SafeRelease(&m_pOriginalShapeBrush[i]);
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
		// Get ready to draw.
		m_pRenderTarget->BeginDraw();

		// Reset to the identity transform.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// Clear the render target contents.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		RECT temp;
		GetClientRect(m_hwnd, &temp);
		D2D1_SIZE_F real_size = m_pRenderTarget->GetSize();

		width_rate = real_size.width / (temp.right - temp.left);
		height_rate = real_size.height / (temp.bottom - temp.top);

		m_pRenderTarget->FillRectangle(
			D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height),
			m_pGridPatternBitmapBrush
		);

		m_pFillBrush->SetOpacity(0.2f);		

		/* 상자 만들기 함수 호출 */
		MyDrawEllipse();

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
void DemoApp::MyDrawEllipse()
{
	RECT temp;
	GetClientRect(m_hwnd, &temp); 
	
	rtSize = m_pRenderTarget->GetSize(); //윈도우 사이즈

	float width_rate = rtSize.width / (temp.right - temp.left);
	float height_rate = rtSize.height / (temp.bottom - temp.top);
	//Direct2d Window 비율 구하기

	D2D1_RECT_F rectangle1 = D2D1::RectF(
		rtSize.width / 2 - 60.0f*width_rate, 10 * height_rate,
		rtSize.width / 2 + 60.0f*width_rate, 50 * height_rate);
	m_pRenderTarget->FillRectangle(&rectangle1, m_pTopBrush);
	//Top Box 생성

	D2D1_RECT_F rectangle[11];
	
	if (!isEmpty()) {
		it = stack.begin(); //iterator 초기화
	}

	boxs = sp + 1; //놓여지는 박스의 수
	if (Remove_box)
		boxs--;

	if ((!isFull())&&(!Remove_box)) {				//놓여질 상자의 위치 
		rectangle[sp+1] = D2D1::RectF(
			rtSize.width / 2 - WIDTH * width_rate, rtSize.height - ((sp + 2) * HEIGHT * height_rate),
			rtSize.width / 2 + WIDTH * width_rate, rtSize.height - ((sp + 1) * HEIGHT * height_rate));
		m_pRenderTarget->FillRectangle(&rectangle[sp + 1], m_pFillBrush);
	}

	
	for (int i = 0; i < boxs; i++) { //만들어진 상자 생성
		
		rectangle[i] = D2D1::RectF(
		rtSize.width / 2 - WIDTH * width_rate, rtSize.height - ((i) *HEIGHT*height_rate),
		rtSize.width / 2 + WIDTH * width_rate, rtSize.height - (((i + 1) * HEIGHT)*height_rate));

		WCHAR tempText[200];
		const char* multiByte = it->name.c_str();
		TCHAR UniCode[15];
		memset(UniCode, 0, sizeof(UniCode));
		MultiByteToWideChar(CP_ACP, MB_COMPOSITE, multiByte, -1, UniCode, 15);

		swprintf_s(tempText, L"%s  %d\n", UniCode, it->grade); //상자 내부에 쓰여지는 글자 생성
		it++;

		m_pRenderTarget->FillRectangle(&rectangle[i], m_pOriginalShapeBrush[i]); //상자 그리기

		m_pRenderTarget->DrawText(
			tempText,
			wcslen(tempText),
			m_pTextFormat,
			D2D1::RectF(rtSize.width / 2 - WIDTH * width_rate, rtSize.height - ((i)*HEIGHT*height_rate), 
    			rtSize.width / 2 + WIDTH * width_rate, rtSize.height - (((i + 1) * HEIGHT)*height_rate)),
			m_pTextBrush2
		); //글자 쓰기
	}//for 문  / 상자 그리기 종료

	/* 상자 인자 정보들 */
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	WCHAR szText[100];
	swprintf_s(szText, L"마우스x:%f\n마우스y:%f\n회전각:%f\n크기조정:%f\n", current_p.x, current_p.y, Angle, move_rate);

	m_pRenderTarget->DrawText(
		szText,
		wcslen(szText),
		m_pTextFormat,
		D2D1::RectF(10.0f, 10.0f, 150.0f, 150.0f),
		m_pTextBrush
	); //좌상단에 쓰여지는 정보 쓰기

	//드래깅 상자 move_rate 핵심 만들어질 상가자 세로가 가로의 1/4
	if (Moving_box) {
		D2D1_RECT_F temprectangle = D2D1::RectF(
			current_p.x*width_rate - move_rate * width_rate, current_p.y*height_rate - move_rate * 0.25*height_rate,
			current_p.x*width_rate + move_rate * width_rate, current_p.y*height_rate + move_rate * 0.25*height_rate);
		//드래깅되는 상자의 현재 마우스 위치에 따른 위치 계산
		if (Make_box) {
			Angle = - ((current_p.y * height_rate - 30 * height_rate)
				/ (30 * height_rate - (rtSize.height - ((0.5 * HEIGHT * height_rate) * (2*sp + 3))))) * 360;
		} //상자 생성시에 회전각 계산
		else if (Remove_box) {
			Angle = - ((current_p.y * height_rate - 30 * height_rate)
				/ (30 * height_rate - (rtSize.height - ((0.5 * HEIGHT * height_rate) * (2*sp + 1))))) * 360;
		} //상자 삭제시에 회전각 계산

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(Angle, current_p)); //상자 회전
		m_pRenderTarget->FillRectangle(&temprectangle, m_pOriginalShapeBrush[boxs]); //상자 그리기

		WCHAR movingText[200];

		const char* movingByte = tempStudent.name.c_str();
		TCHAR movingCode[15];
		memset(movingCode, 0, sizeof(movingCode));
		MultiByteToWideChar(CP_ACP, MB_COMPOSITE, movingByte, -1, movingCode, 15);

		swprintf_s(movingText, L"%s  %d\n", movingCode, tempStudent.grade);
		
		m_pRenderTarget->DrawText(
			movingText,
			wcslen(movingText),
			m_pTextFormat,
			temprectangle,
			m_pTextBrush2
		);
	}

}

void DemoApp::OnResize(UINT width, UINT height) {
	if (m_pRenderTarget)
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x, y;
	
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
		case WM_SIZE: //윈도우 사이즈 변경시 호출 메세지
		{
			winWidth = LOWORD(lParam);
			winHeight = HIWORD(lParam);
			pDemoApp->OnResize(winWidth, winHeight);
		}
		return 0;

		case WM_DISPLAYCHANGE:
		{
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

		case WM_LBUTTONDOWN:
		{
			current_p.x = LOWORD(lParam);
			current_p.y = HIWORD(lParam);

			if (!isEmpty()) {
				if (current_p.x*width_rate >= (rtSize.width / 2 - WIDTH * width_rate) && 
					current_p.x*width_rate <= rtSize.width / 2 + WIDTH * width_rate && 
					current_p.y*height_rate <= rtSize.height - sp * 100 * height_rate && 
					current_p.y*height_rate >= rtSize.height - (sp + 1) * 100 * height_rate) {
					//가장 위의 상자 클릭했을 때

					
					it = stack.begin();
					for (int i = 0; i < sp; i++) {
						it++;
					}
					tempStudent.name = it->name;
					tempStudent.grade = it->grade; //임시 Student객체에 가장 상단의 값 넣어두기
					
					Moving_box = true;
					Remove_box = true;
				}
			}
		
			if (current_p.x*width_rate >= (rtSize.width / 2 - 60 * width_rate) && 
				current_p.x*width_rate <= rtSize.width / 2 + 60 * width_rate && 
				current_p.y*height_rate >= 10 * height_rate && 
				current_p.y*height_rate <= 50 * height_rate) {
				//상자 만들기
				if (isFull()) {
					MessageBoxA(hwnd, "데이터가 6개입니다 꽉 찼습니다.", "Message", MB_OK);
					//꽉 찬 경우
				}
				else {
					tempStudent.name = name[rand() % 10];
					tempStudent.grade = rand() % 30 + 20;
					//랜덤값 임시 객체에 넣어두기
					Moving_box = true;
					Make_box = true;
				}
			}
		}

		break;

		case WM_LBUTTONUP:
		{
			Make_box = false;
			Moving_box = false;
			Remove_box = false;
			Angle = 0;
			move_rate = 0;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;

		case WM_MOUSEMOVE:

			current_p.x = LOWORD(lParam);
			current_p.y = HIWORD(lParam);

			
			if (Make_box && Moving_box) {
				if (current_p.x*width_rate >= (rtSize.width / 2 - WIDTH * width_rate) && 
					current_p.x*width_rate <= rtSize.width / 2 + WIDTH * width_rate && 
					current_p.y*height_rate <= rtSize.height - (sp + 1) * 100 * height_rate && 
					current_p.y*height_rate >= rtSize.height - (sp + 2) * 100 * height_rate) {
					
					stack.push_back(tempStudent);
					sp++;
					//학생정보 스택에 삽입 후 스택포인터 증가

					winHeight -= 100;
					//삽입할 때 마다 윈도우사이즈 조정
					Make_box = false;
					Moving_box = false;
				}
			}

			if (Remove_box && Moving_box) {
				if (current_p.x*width_rate >= (rtSize.width / 2 - 60 * width_rate) &&
					current_p.x*width_rate <= rtSize.width / 2 + 60 * width_rate &&
					current_p.y*height_rate >= 10 * height_rate &&
					current_p.y*height_rate <= 50 * height_rate) {
					
					winHeight += 100;
					//윈도우 사이즈 조정

					stack.pop_back();
					sp--;
					//스택에서 상자 한 개 제거
					Moving_box = false;
					Remove_box = false;
				}
			}//Remove 끝

			if (Moving_box) {
				UINT tempHeight = winHeight;
				if (Remove_box)
					tempHeight += 100;
				move_rate = WIDTH / ((tempHeight - 50)*height_rate)*current_p.y*height_rate;
			}
			//상자 갯수에 따른 크기 비율 조절
			InvalidateRect(hwnd, NULL, FALSE);
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
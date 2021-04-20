#include "Hw2.h"

/* 마우스 좌클릭 중인지 아닌지 확인 */
BOOL leftClicking = false;

/* 왼쪽 마우스 현재좌표 및 현재 좌표 */
D2D_POINT_2F leftDownAxis;            // 마우스 좌클릭 시작 지점의 좌표
D2D_POINT_2F leftUpAxis;              // 마우스 좌클릭 후 손을 뗀 지점의 좌표
D2D_POINT_2F currentAxis;             // 현재 마우스가 클릭되고있는 지점의 좌표

/* 사각형의 좌측 상, 우측 하 좌표 */
D2D_POINT_2F miniLeftTopAxis;                //상단의 미니사각형의 왼쪽상단 좌표값
D2D_POINT_2F miniRightBottomAxis;            //상단의 미니사각형의 오른쪽하단 좌표값

D2D_POINT_2F leftTopAxis;                    //생성되는 사각형의 왼쪽상단 좌표값
D2D_POINT_2F rightBottomAxis;                //생성되는 사각형의 오른쪽하단 좌표값


/* 박스 회전 각도 및 크기조정비율 */
DOUBLE rotateRate = 0.0f;                     //박스 삭제시 박스 회전각도    
DOUBLE scaleRate = 1.0f;               //박스 생성시 크기조정비율

/* 박스 갯수 */
int numBox = 0;
float boxHeight = 0;

std::vector<Student> studentV(0);

std::string last[10] = { "김","이","박","최","정","한","윤","우","강","임" };
std::string first[15] = { "윤","요","환","원","창","연","성","선","호","인","소","신","병","구","회" };


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
	m_pRenderTarget(NULL),
	m_pStrokeStyleDash(NULL),
	m_pGridPatternBitmapBrush(NULL),
	m_pOriginalShapeBrush(NULL),
	m_pTransformedShapeBrush(NULL),
	m_pFillBrush(NULL),
	m_pTextBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL),
	m_bitmapBrush(NULL)

{
}


DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pStrokeStyleDash);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
	SAFE_RELEASE(m_pOriginalShapeBrush);
	SAFE_RELEASE(m_pTransformedShapeBrush);
	SAFE_RELEASE(m_pFillBrush);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_bitmapBrush);///
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
}

// Create application window and device-independent resources.
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
		wcex.hInstance = hInstance;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"D2DDemoApp";
		RegisterClassEx(&wcex);

		// Create the application window.
		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"201301625 정병선",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			1280, 800, NULL, NULL, hInstance, this);
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

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

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

	if (SUCCEEDED(hr))
	{
		
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

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &m_pOriginalShapeBrush);
		}

		if (SUCCEEDED(hr))
		{
			
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::YellowGreen), &m_pFillBrush);
		}
		if (SUCCEEDED(hr))
		{
			
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &m_pFillBrush1);
		}
	


		if (SUCCEEDED(hr))
		{

			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrush);
		}

		if (SUCCEEDED(hr))
		{

			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &m_pTransformedShapeBrush);
		}

		if (SUCCEEDED(hr))
		{

			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue), &m_pTransformedShapeBrush1);
		}
		if (SUCCEEDED(hr))
		{

			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &m_pTransformedShapeBrush2);
		}

		if (SUCCEEDED(hr))
		{
			hr = CreateGridPatternBrush(m_pRenderTarget, &m_bitmapBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
		}
	}

	return hr;
}

HRESULT DemoApp::CreateGridPatternBrush(ID2D1RenderTarget *pRenderTarget, ID2D1BitmapBrush **ppBitmapBrush)
{
	HRESULT hr = S_OK;

	// Create a compatible render target.
	ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
	hr = pRenderTarget->CreateCompatibleRenderTarget(D2D1::SizeF(10.0f, 10.0f), &pCompatibleRenderTarget);

	if (SUCCEEDED(hr))
	{
		// Draw a pattern.
		ID2D1SolidColorBrush *pGridBrush = NULL;
		hr = pCompatibleRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f), &pGridBrush);

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
						D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP),
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

// Discard device-specific resources which need to be recreated when a Direct3D device is lost.
void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pTransformedShapeBrush);
	SAFE_RELEASE(m_pFillBrush);
	SAFE_RELEASE(m_pOriginalShapeBrush);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
}

// Main window message loop
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

	srand((unsigned)time(NULL));

	if (SUCCEEDED(hr))
	{
		// Get ready to draw.
		m_pRenderTarget->BeginDraw();

		// Reset to the identity transform.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// Clear the render target contents.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		m_pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height), m_pGridPatternBitmapBrush);

		m_pOriginalShapeBrush->SetOpacity(0.1);
		m_pFillBrush->SetOpacity(0.1f);



		// Show the effects of rotation and then translation.

		dragBox();
		stackBox();
		predictBox();
		showBoxInfo();
		StudentInfo();
		

		// Commit the drawing operations.
		hr = m_pRenderTarget->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}
	}

	return hr;
}
void DemoApp::showBoxInfo()
{
	WCHAR szText[100];

	_swprintf(szText, L"마우스x:%f\n마우스y:%f\n회전각:%f\n크기비율:%f\n박스갯수:%d\n", currentAxis.x, currentAxis.y, rotateRate, scaleRate, numBox);

	m_pRenderTarget->DrawText(
		szText,
		wcslen(szText),
		m_pTextFormat,
		D2D1::RectF(10.0f, 10.0f, 150.0f, 150.0f),
		m_pTextBrush
	);


}
void DemoApp::dragBox()
{

	miniLeftTopAxis.x = 610.0f;
	miniLeftTopAxis.y = 40.0f;

	miniRightBottomAxis.x = 670.0f;
	miniRightBottomAxis.y = 100.0f;

	boxHeight = 60.0f;

	static const WCHAR szText[] = L"DRAG BOX";

	m_pRenderTarget->DrawText(szText, ARRAYSIZE(szText) - 1, m_pTextFormat, D2D1::RectF(610.0f, 10.0f, 700.0f, 40.0f), m_pTextBrush);


	D2D1_RECT_F Rectangle = D2D1::RectF(miniLeftTopAxis.x, miniLeftTopAxis.y, miniRightBottomAxis.x, miniRightBottomAxis.y);


	m_pRenderTarget->DrawRectangle(Rectangle, m_pTransformedShapeBrush2, 1, m_pStrokeStyleDash);

	m_pRenderTarget->DrawRectangle(D2D1::RectF(520.0f, 750.0f, 760.0f, 755.0f), m_pTransformedShapeBrush, 1, m_pStrokeStyleDash);



	if (numBox < 8 && 610 < leftDownAxis.x && leftDownAxis.x < 670 && 40 < leftDownAxis.y && leftDownAxis.y < 100)
	{
		rotateRate = 360 * (float(currentAxis.y) / (float(leftTopAxis.y) - (boxHeight*numBox)));
		scaleRate = (4.0f * float(currentAxis.y) / (float(leftTopAxis.y) - (boxHeight*numBox)));

		D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(currentAxis.x-640, currentAxis.y-40);
		D2D1_MATRIX_3X2_F scaling = D2D1::Matrix3x2F::Scale(scaleRate, 1.0f,D2D1::Point2F(currentAxis.x, currentAxis.y));
		D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(rotateRate, D2D1::Point2F(currentAxis.x , currentAxis.y));


		m_pRenderTarget->SetTransform(translation * scaling * rotation);
		m_pRenderTarget->DrawRectangle(D2D1::RectF(610.0f, 40.0f, 670.0f, 100.0f), m_pOriginalShapeBrush);
		m_pRenderTarget->FillRectangle(D2D1::RectF(610.0f, 40.0f, 670.0f, 100.0f), m_pFillBrush);

		if (510 < leftUpAxis.x && leftUpAxis.x < 770 && (750-(boxHeight*(numBox+1)))< leftUpAxis.y && leftUpAxis.y < (750 - (boxHeight*numBox)) && leftClicking == false)
		{
			numBox++;
			
			Student s;
			std::string name = "";
			name = last[rand() % 10];	//랜덤으로 성
			s.grade = 20 + rand() % 30;						//성적 20-50
			for (int j = 0; j < (rand()%2)+1; j++) {		//성 제외한 이름
				name += first[rand() % 15];
			}
			s.name = name.c_str();
			studentV.push_back(s);
		}


	}
	else
	{
		scaleRate = 1.0f;
	}

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());



}
void DemoApp::predictBox() {
	leftTopAxis.x = 520.0f;
	leftTopAxis.y = 690.0f;

	rightBottomAxis.x = 760.0f;
	rightBottomAxis.y = 750.0f;
	if (numBox < 8) {
		D2D1_RECT_F rectangle = D2D1::RectF(520, (750 - (boxHeight*(numBox + 1))), 760, (750 - (boxHeight*numBox)));
		m_pRenderTarget->DrawRectangle(rectangle, m_pTransformedShapeBrush1,1, m_pStrokeStyleDash);

	}
}
void DemoApp::stackBox()
{

	leftTopAxis.x = 520.0f;
	leftTopAxis.y = 690.0f;

	rightBottomAxis.x = 760.0f;
	rightBottomAxis.y = 750.0f;


	for (int i = 0; i < numBox; i++)
	{
		D2D1_RECT_F rectangle = D2D1::RectF(leftTopAxis.x, leftTopAxis.y, rightBottomAxis.x, rightBottomAxis.y);

		m_pRenderTarget->DrawRectangle(rectangle, m_pTransformedShapeBrush);
		m_pRenderTarget->DrawRectangle(rectangle, m_pFillBrush);

		rightBottomAxis.y = leftTopAxis.y;
		leftTopAxis.y -= 60;

	}

	leftTopAxis.y = rightBottomAxis.y;
	rightBottomAxis.y += 60;

	int rotationValue;

	

	if (numBox > 0 && leftTopAxis.x < leftDownAxis.x && leftDownAxis.x < rightBottomAxis.x && leftTopAxis.y < leftDownAxis.y && leftDownAxis.y < rightBottomAxis.y && 750 > currentAxis.y)
	{

		rotateRate = 360 * (float(currentAxis.y) / (750-(boxHeight*numBox)));
		scaleRate = (4.0f * float(currentAxis.y) / (750-(boxHeight*numBox)));

		D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(currentAxis.x-640, currentAxis.y-40);
		D2D1_MATRIX_3X2_F scaling = D2D1::Matrix3x2F::Scale(scaleRate, 1.0f, D2D1::Point2F(currentAxis.x, currentAxis.y));
		D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(rotateRate, D2D1::Point2F(currentAxis.x, currentAxis.y));


		m_pRenderTarget->SetTransform(translation * scaling * rotation);
		m_pRenderTarget->DrawRectangle(D2D1::RectF(610.0f, 40.0f, 670.0f, 100.0f), m_pOriginalShapeBrush);
		m_pRenderTarget->FillRectangle(D2D1::RectF(610.0f, 40.0f, 670.0f, 100.0f), m_pFillBrush1);

		if (610 < leftUpAxis.x && leftUpAxis.x < 670 && 40 < leftUpAxis.y && leftUpAxis.y < 100 && leftClicking == false)
		{
			
			numBox--;
			studentV.pop_back();

		}
		
	}



	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());



}
void DemoApp::StudentInfo()
{
	if (studentV.size() != 0) {
		for (int i = 0; i < numBox; i++) {
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			Student tmp = studentV.at(i);

			WCHAR szText[200];

			const char* multiByte = tmp.name.c_str();
			TCHAR UniCode[15];
			memset(UniCode, 0, sizeof(UniCode));
			MultiByteToWideChar(CP_ACP, MB_COMPOSITE, multiByte, -1, UniCode, 15);

			swprintf_s(szText, L"%s  %d\n", UniCode, tmp.grade);

			
				m_pRenderTarget->DrawText(
					szText,
					wcslen(szText),
					m_pTextFormat,
					D2D1::RectF(520, (750 - (boxHeight*i)), 760, (750 - (boxHeight*(i+1)))),
					m_pTextBrush
				);

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		}
	}
}
// If the application receives a WM_SIZE message, this method resizes the render target appropriately.
void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// Note: This method can fail, but it's okay to ignore the error here -- the error will be repeated on the next call to EndDraw.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

// Window message handler
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		return 1;
	}

	DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

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

		case WM_LBUTTONDOWN:
		{
			leftDownAxis.x = LOWORD(lParam);
			leftDownAxis.y = HIWORD(lParam);
			leftClicking = true;
		}

		break;

		case WM_LBUTTONUP:
		{
			leftUpAxis.x = LOWORD(lParam);
			leftUpAxis.y = HIWORD(lParam);
			leftClicking = false;
			InvalidateRect(hwnd, NULL, FALSE);

		}
		break;

		case WM_MOUSEMOVE:
			//currentAxis.x = LOWORD(lParam);
			//currentAxis.y = HIWORD(lParam);

			//InvalidateRect(hwnd, NULL, FALSE);

			if (leftClicking)
			{
				currentAxis.x = LOWORD(lParam);
				currentAxis.y = HIWORD(lParam);
				InvalidateRect(hwnd, NULL, FALSE);
			}

			break;

		case WM_DISPLAYCHANGE:
		{
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

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
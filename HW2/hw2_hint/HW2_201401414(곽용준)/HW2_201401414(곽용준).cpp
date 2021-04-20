#include "HW2.h"

#define MAX_LOADSTRING 100
#define MAX_LAST_NAME 10
#define MAX_FIRST_NAME 15
#define MAX_STACK 6

#define WIDTH 200
#define HEIGHT 100
//���� ũ��


//���� ����, ���� ���� ����
BOOL Make_box = false;
BOOL Moving_box = false;
BOOL Remove_box = false;


/*���� ���콺 Ȯ�� ����Ʈ*/
D2D_POINT_2F current_p;
D2D_POINT_2F moving_p;


/*Ÿ���� ������ ũ��, �� ������ �����ϴ� ������*/
FLOAT Angle = 0.0f;
FLOAT Size = 1.0f;


D2D1_SIZE_F rtSize;
UINT winWidth;
UINT winHeight;

//�л������� ������ ���� ���� �� �̸� ����
Student tempStudent;
std::string name[10] = { "������", "�켼��", "�豤ǥ", "�̿���" , "����ȣ", "�����", "������", "�����","������" , "��μ�" };
std::vector<Student> stack;
std::vector<Student>::iterator it;
int sp = -1;
int boxs = 0;

//���� �Լ� ����
bool isFull() {
	return sp == MAX_STACK - 1;
}

bool isEmpty() {
	return sp == -1;
}

float width_rate;
float height_rate;
//Direct2d ����, ���� ���� 

float move_rate = 0;
//����, ���� ���� ���� ����

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
		wcex.lpszClassName = L"HW2_201401414(������)";

		RegisterClassEx(&wcex);

		// Create the application window.
		m_hwnd = CreateWindow(
			L"HW2_201401414(������)",
			L"HW2_201401414(������)",
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

		float temp[3]; //���������

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

		/* ���� ����� �Լ� ȣ�� */
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
	
	rtSize = m_pRenderTarget->GetSize(); //������ ������

	float width_rate = rtSize.width / (temp.right - temp.left);
	float height_rate = rtSize.height / (temp.bottom - temp.top);
	//Direct2d Window ���� ���ϱ�

	D2D1_RECT_F rectangle1 = D2D1::RectF(
		rtSize.width / 2 - 60.0f*width_rate, 10 * height_rate,
		rtSize.width / 2 + 60.0f*width_rate, 50 * height_rate);
	m_pRenderTarget->FillRectangle(&rectangle1, m_pTopBrush);
	//Top Box ����

	D2D1_RECT_F rectangle[11];
	
	if (!isEmpty()) {
		it = stack.begin(); //iterator �ʱ�ȭ
	}

	boxs = sp + 1; //�������� �ڽ��� ��
	if (Remove_box)
		boxs--;

	if ((!isFull())&&(!Remove_box)) {				//������ ������ ��ġ 
		rectangle[sp+1] = D2D1::RectF(
			rtSize.width / 2 - WIDTH * width_rate, rtSize.height - ((sp + 2) * HEIGHT * height_rate),
			rtSize.width / 2 + WIDTH * width_rate, rtSize.height - ((sp + 1) * HEIGHT * height_rate));
		m_pRenderTarget->FillRectangle(&rectangle[sp + 1], m_pFillBrush);
	}

	
	for (int i = 0; i < boxs; i++) { //������� ���� ����
		
		rectangle[i] = D2D1::RectF(
		rtSize.width / 2 - WIDTH * width_rate, rtSize.height - ((i) *HEIGHT*height_rate),
		rtSize.width / 2 + WIDTH * width_rate, rtSize.height - (((i + 1) * HEIGHT)*height_rate));

		WCHAR tempText[200];
		const char* multiByte = it->name.c_str();
		TCHAR UniCode[15];
		memset(UniCode, 0, sizeof(UniCode));
		MultiByteToWideChar(CP_ACP, MB_COMPOSITE, multiByte, -1, UniCode, 15);

		swprintf_s(tempText, L"%s  %d\n", UniCode, it->grade); //���� ���ο� �������� ���� ����
		it++;

		m_pRenderTarget->FillRectangle(&rectangle[i], m_pOriginalShapeBrush[i]); //���� �׸���

		m_pRenderTarget->DrawText(
			tempText,
			wcslen(tempText),
			m_pTextFormat,
			D2D1::RectF(rtSize.width / 2 - WIDTH * width_rate, rtSize.height - ((i)*HEIGHT*height_rate), 
    			rtSize.width / 2 + WIDTH * width_rate, rtSize.height - (((i + 1) * HEIGHT)*height_rate)),
			m_pTextBrush2
		); //���� ����
	}//for ��  / ���� �׸��� ����

	/* ���� ���� ������ */
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	WCHAR szText[100];
	swprintf_s(szText, L"���콺x:%f\n���콺y:%f\nȸ����:%f\nũ������:%f\n", current_p.x, current_p.y, Angle, move_rate);

	m_pRenderTarget->DrawText(
		szText,
		wcslen(szText),
		m_pTextFormat,
		D2D1::RectF(10.0f, 10.0f, 150.0f, 150.0f),
		m_pTextBrush
	); //�»�ܿ� �������� ���� ����

	//�巡�� ���� move_rate �ٽ� ������� ���� ���ΰ� ������ 1/4
	if (Moving_box) {
		D2D1_RECT_F temprectangle = D2D1::RectF(
			current_p.x*width_rate - move_rate * width_rate, current_p.y*height_rate - move_rate * 0.25*height_rate,
			current_p.x*width_rate + move_rate * width_rate, current_p.y*height_rate + move_rate * 0.25*height_rate);
		//�巡��Ǵ� ������ ���� ���콺 ��ġ�� ���� ��ġ ���
		if (Make_box) {
			Angle = - ((current_p.y * height_rate - 30 * height_rate)
				/ (30 * height_rate - (rtSize.height - ((0.5 * HEIGHT * height_rate) * (2*sp + 3))))) * 360;
		} //���� �����ÿ� ȸ���� ���
		else if (Remove_box) {
			Angle = - ((current_p.y * height_rate - 30 * height_rate)
				/ (30 * height_rate - (rtSize.height - ((0.5 * HEIGHT * height_rate) * (2*sp + 1))))) * 360;
		} //���� �����ÿ� ȸ���� ���

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(Angle, current_p)); //���� ȸ��
		m_pRenderTarget->FillRectangle(&temprectangle, m_pOriginalShapeBrush[boxs]); //���� �׸���

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
		case WM_SIZE: //������ ������ ����� ȣ�� �޼���
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
					//���� ���� ���� Ŭ������ ��

					
					it = stack.begin();
					for (int i = 0; i < sp; i++) {
						it++;
					}
					tempStudent.name = it->name;
					tempStudent.grade = it->grade; //�ӽ� Student��ü�� ���� ����� �� �־�α�
					
					Moving_box = true;
					Remove_box = true;
				}
			}
		
			if (current_p.x*width_rate >= (rtSize.width / 2 - 60 * width_rate) && 
				current_p.x*width_rate <= rtSize.width / 2 + 60 * width_rate && 
				current_p.y*height_rate >= 10 * height_rate && 
				current_p.y*height_rate <= 50 * height_rate) {
				//���� �����
				if (isFull()) {
					MessageBoxA(hwnd, "�����Ͱ� 6���Դϴ� �� á���ϴ�.", "Message", MB_OK);
					//�� �� ���
				}
				else {
					tempStudent.name = name[rand() % 10];
					tempStudent.grade = rand() % 30 + 20;
					//������ �ӽ� ��ü�� �־�α�
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
					//�л����� ���ÿ� ���� �� ���������� ����

					winHeight -= 100;
					//������ �� ���� ����������� ����
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
					//������ ������ ����

					stack.pop_back();
					sp--;
					//���ÿ��� ���� �� �� ����
					Moving_box = false;
					Remove_box = false;
				}
			}//Remove ��

			if (Moving_box) {
				UINT tempHeight = winHeight;
				if (Remove_box)
					tempHeight += 100;
				move_rate = WIDTH / ((tempHeight - 50)*height_rate)*current_p.y*height_rate;
			}
			//���� ������ ���� ũ�� ���� ����
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
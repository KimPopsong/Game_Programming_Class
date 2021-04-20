#include "HW2.h"
using namespace std;
struct tempbox
{
	int left;
	int top;
	int right;
	int bottom;
}tbx,tbx2;

typedef struct student
{
	WCHAR name[4];
	UINT score;
}stu;
stu student;
vector <stu> v;
WCHAR lastname[] = L"���̹ڽ���ä������������";
WCHAR firstname[] = L"���������������������������������α���";
FLOAT cur_h;
FLOAT info_s, info_r;
FLOAT dist;
FLOAT win_height;
FLOAT win_width;
D2D_RECT_F cur_rec;
D2D_POINT_2F current_p;
BOOL upper_rect_check = false;
BOOL delet_rect_check = false;
BOOL inBox = false;
UINT recNum = 0;


#if defined(_DEBUG)&&defined(WIN32)&&!defined(_AFX)&&!defined(_AFXDLL)
#define TRACE TRACE_WIN32
#pragma warning(disable: 4996)
void TRACE_WIN32(LPCTSTR lpszFormat, ...) {
	TCHAR lpszBuffer[0x160]; //buffer size
	va_list fmtList;
	va_start(fmtList, lpszFormat);
	_vstprintf(lpszBuffer, lpszFormat, fmtList);
	va_end(fmtList);
	OutputDebugString(lpszBuffer);
}
#endif

// ���� ���α׷��� ������ �Լ�.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			srand(time(NULL));
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

// ������. ��� �������� �ʱ�ȭ��.
DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL),
	m_pLightSlateGrayBrush(NULL),
	m_pCornflowerBlueBrush(NULL),
	m_pTransformedShapeBrush(NULL),
	m_pFillBrush{ NULL ,},
	m_pTextBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL)
{
}

// �Ҹ���. ���� ���α׷��� �ڿ��� �ݳ���.
DemoApp::~DemoApp()
{
	DiscardDeviceResources();
	SAFE_RELEASE(m_pDirect2dFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pTransformedShapeBrush);
	for (int i = 0; i < 10; i++)
		SAFE_RELEASE(m_pFillBrush[i]);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);

}

// ���� ���α׷��� �����츦 ������. ��ġ ������ �ڿ��� ������.
HRESULT DemoApp::Initialize(HINSTANCE hInstance)
{
	HRESULT hr;
	// ��ġ ������ �ڿ��� ������.
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		// ������ Ŭ������ �����..
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = hInstance;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"D2DDemoApp";
		RegisterClassEx(&wcex);

		// �����츦 ������.
		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"Direct2D Demo Application",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			640, 480, NULL, NULL, hInstance, this
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

// ��ġ ������ �ڿ����� ������. �̵� �ڿ��� ������ ���� ���α׷��� ����Ǳ� ������ ��ȿ��.
HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// D2D ���丮�� ������.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

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
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			13.0f,
			L"en-us",
			&m_pTextFormat
		);
	}

	return hr;
}


// ��ġ ������ �ڿ����� ������. ��ġ�� �ҽǵǴ� ��쿡�� �̵� �ڿ��� �ٽ� �����ؾ� ��.
HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		cur_h = size.height;
		dist = size.height;
		win_width = size.width;
		win_height = size.height;
		// D2D ����Ÿ���� ������.
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			// ȸ�� ���� ������.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray), &m_pLightSlateGrayBrush);
		}
		if (SUCCEEDED(hr))
		{
			// �Ķ��� ���� ������.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
		}
		if (SUCCEEDED(hr))
		{
			// ��ȯ �� ����� �ܰ����� ���� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &m_pTransformedShapeBrush);
		}
		float tmp[3];
		for (int i = 0; i < 10; ++i) {
			tmp[0] = rand() % 101 / 100.0;
			tmp[1] = rand() % 101 / 100.0;
			tmp[2] = rand() % 101 / 100.0;
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateSolidColorBrush(
					D2D1::ColorF(tmp[0], tmp[1], tmp[2], 0.5),
					&m_pFillBrush[i]
				);
			}
		}
		if (SUCCEEDED(hr))
		{
			// �ؽ�Ʈ�� ���� ���� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrush);
		}
	}

	return hr;
}

// ��ġ ������ �ڿ����� �ݳ���. ��ġ�� �ҽǵǸ� �̵� �ڿ��� �ٽ� �����ؾ� ��.
void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pLightSlateGrayBrush);
	SAFE_RELEASE(m_pCornflowerBlueBrush);
	for (int i = 0; i < 10; i++)
		SAFE_RELEASE(m_pFillBrush[i]);
}

// ���� ������ �޽��� ������ ������.
void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

// Ŭ���̾�Ʈ ������ ȭ���� �׸�.
// ����: �� �Լ��� ����Ǵ� ���ȿ� ��ġ�� �ҽǵǸ� ��ġ ������ �ڿ����� �ݳ���. �� ���� ȣ�� �ÿ� �� �ڿ����� �ٽ� ������.
HRESULT DemoApp::OnRender()
{
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		// ���� ����� �׸�.
		int width = static_cast<int>(rtSize.width);
		int height = static_cast<int>(rtSize.height);

		for (int x = 0; x < width; x += 10)
		{
			m_pRenderTarget->DrawLine(
				D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
				D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
				m_pLightSlateGrayBrush, 0.5f
			);
		}

		for (int y = 0; y < height; y += 10)
		{
			m_pRenderTarget->DrawLine(
				D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
				D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
				m_pLightSlateGrayBrush, 0.5f
			);
		}
		WCHAR szText[100];
		_swprintf(szText, L"x:%.2f\ny:%.2f\nrotate:%.2f\nscale:%.2f\n", current_p.x, current_p.y, info_r, info_s);
		D2D1_RECT_F info_rec = D2D1::RectF(0, 0, 100, 70);
		m_pRenderTarget->DrawRectangle(&info_rec, m_pCornflowerBlueBrush);
		m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, info_rec, m_pTextBrush);
		// �� ���� �簢���� �׸�.
		if (upper_rect_check||delet_rect_check) {
			MakeChasingRect(current_p,cur_h);
			if (upper_rect_check) { //�Ʒ� ������ ����� �ӽ� �簢��.
				cur_rec = D2D1::RectF(
					rtSize.width / 2 - 80.0f, cur_h - 40.0f, rtSize.width / 2 + 80.0f, cur_h);
			}
			else if (delet_rect_check) {
				cur_rec = D2D1::RectF(
					rtSize.width / 2 - 80.0f, cur_h, rtSize.width / 2 + 80.0f, cur_h+40.0f);
			}
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->DrawRectangle(&cur_rec, m_pCornflowerBlueBrush);
		}
		if (recNum > 0) {
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			for (int i = 1; i <= recNum; ++i) {
				D2D1_RECT_F bottom_rect = D2D1::RectF(
					rtSize.width / 2 - 80.0f, rtSize.height - 40 * i,
					rtSize.width / 2 + 80.0f, rtSize.height - 40 * (i - 1));
				_swprintf(szText, L"%s\n%d", v[i-1].name,v[i-1].score);
				m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, bottom_rect, m_pTextBrush);
				m_pRenderTarget->DrawRectangle(&bottom_rect, m_pTransformedShapeBrush);
				m_pRenderTarget->FillRectangle(&bottom_rect, m_pFillBrush[i-1]);
			}
			
		}
		m_pLightSlateGrayBrush->SetOpacity(0.2f);
		D2D1_RECT_F upper_fix_rectangle = D2D1::RectF(
			rtSize.width / 2 - 20.0f, 0,
			rtSize.width / 2 + 20.0f, 40.0f
		);
		tbx.left = rtSize.width / 2 - 20.0f; tbx.top = 0; tbx.right = rtSize.width / 2 + 20.0f; tbx.bottom = 40.0f;
		tbx2.left = rtSize.width / 2 - 80.0f; tbx2.top = cur_h; tbx2.right = rtSize.width / 2 + 80.0f; tbx2.bottom = cur_h + 40.0f;
		// ä���� �簢���� �׸�.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->FillRectangle(&upper_fix_rectangle, m_pLightSlateGrayBrush);


		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}

stu GenerateData(void)
{
	stu temp;

	if (recNum < 10) {
		temp.score = (rand()%30)+20;
		TRACE(L"%i", temp.score);
		temp.name[0] = lastname[rand() % 12];
		for (int i = 1; i < 3; ++i) {
			temp.name[i] = firstname[rand() % 20];
		}
		temp.name[3] = '\0';

	}

	return temp;
}

D2D1_RECT_F DemoApp::MakeChasingRect(D2D_POINT_2F cur,FLOAT h)
{
	FLOAT scale_ratio,rotate_degree,alpha1,beta1,alpha2,beta2,temp_cur_h;
	int a;
	////////////////////scale ��ȯ ������ ���ϴ� ��
	if (cur.y < 20) {
		scale_ratio = 1;
		rotate_degree = 0;
	}
	else {
		temp_cur_h = cur_h - 20;
		alpha1 = 3 / (float)(temp_cur_h - 20);
		alpha2 = 360 / (float)(temp_cur_h - 20);
		beta1 = 1 - 20 * alpha1;
		beta2 = -20 * alpha2;
		scale_ratio = alpha1 * cur.y + beta1;
		rotate_degree = (int)(alpha2 * cur.y + beta2)%360;
	}
	if (scale_ratio > 4)
		scale_ratio = 4.0f;
	////////////////////////////////////////

	D2D1_RECT_F F=D2D1::RectF(
		current_p.x - 20.0f, current_p.y - 20.0f,
		current_p.x + 20.0f, current_p.y + 20.0f);
	
	// ũ������ ��ȯ�� ����Ÿ�ٿ� �����ϰ� �簢���� ä�� �׸�.
	D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(scale_ratio, 1.0f), D2D1::Point2F(cur.x, cur.y));
	D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(rotate_degree, D2D1::Point2F(cur.x,cur.y));
	
	m_pRenderTarget->SetTransform(scale*rotation);
	WCHAR szText[100];
	_swprintf(szText, L"%s\n%d", student.name, student.score);
	
	m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, F, m_pTextBrush);
	m_pRenderTarget->DrawRectangle(F, m_pTransformedShapeBrush);
	if (delet_rect_check)
		a = recNum - 1;
	else
		a = recNum;
	m_pRenderTarget->FillRectangle(F, m_pFillBrush[a]);
	TRACE(L"cur_h = %f ----- cur_y = %f ------ ratio = %f ------------degree=%f \n", cur_h,cur.y,scale_ratio,rotate_degree);
	info_s = scale_ratio; info_r = rotate_degree;
	return F;
}


// ���� ���α׷����� WM_SIZE �޽����� �߻��Ǹ� �� �Լ��� ����Ÿ���� ũ�⸦ �ٽ� ������.
void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// ����: �Ʒ��� �Լ��� ���� �ÿ� ������ ���� ������, ���⿡�� ���� ������ üũ���� �ʰ� �Ѿ�� ��. ������ EndDraw �Լ��� ȣ��� ���� ������ Ȯ�ε� ���̹Ƿ� �׶� ó���ϸ� ��.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

// ������ �޽����� ó��.
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	LPMINMAXINFO mmi;

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
			case WM_LBUTTONDOWN:
			{
				
				current_p.x = LOWORD(lParam);
				current_p.y = HIWORD(lParam);
				if (current_p.x >= tbx.left&&current_p.x <= tbx.right) {//tbx���� �簢���� ����Ŵ.
					if (current_p.y >= tbx.top&&current_p.y <= tbx.bottom) {
						if (recNum > 9) {
							MessageBoxW(hwnd, L"������ �Ѿ���.", L"error", 0);
							return 0;
						}
						upper_rect_check = true;
						student = GenerateData();
					}
				}
				if (!upper_rect_check) {
					if (current_p.x >= tbx2.left&&current_p.x <= tbx2.right)
						if (current_p.y >= tbx2.top&&current_p.y <= tbx2.bottom) {
							delet_rect_check = true;
							student = v[v.size() - 1];
							TRACE(L"DELETE\n");
						}
				}
			}
			break;
			case WM_LBUTTONUP:
			{
				current_p.x = LOWORD(lParam);
				current_p.y = HIWORD(lParam);
				if (upper_rect_check) {
					if (current_p.x >= cur_rec.left&&current_p.x <= cur_rec.right)
						if (current_p.y >= cur_rec.top&&current_p.y <= cur_rec.bottom) {
							recNum++;
							inBox = true;
							cur_h -= 40;
							v.push_back(student);
						}
				}
				else if (delet_rect_check) {
					if (current_p.x >= tbx.left&&current_p.x <= tbx.right) //tbx���� �簢���� ����Ŵ.
						if (current_p.y >= tbx.top&&current_p.y <= tbx.bottom) {
							if (recNum > 0) {
								recNum--;
								cur_h += 40;
								v.pop_back();
							}
						}
				}
				TRACE(L"asdf\n");
				upper_rect_check = false;
				delet_rect_check = false;
				info_r = 0.0f; info_s = 0.0f;
				InvalidateRect(hwnd, NULL, false);
			}
			break;
			case WM_MOUSEMOVE:
			{
				current_p.x = LOWORD(lParam);
				current_p.y = HIWORD(lParam);
				InvalidateRect(hwnd, NULL, FALSE);
			}
			break;
			case WM_PAINT:
			{
				pDemoApp->OnRender();
				ValidateRect(hwnd, NULL);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_GETMINMAXINFO:
				mmi = (LPMINMAXINFO)lParam;
				mmi->ptMinTrackSize.x = 640;
				mmi->ptMinTrackSize.y = 480;
				mmi->ptMaxTrackSize.x = 640;
				mmi->ptMaxTrackSize.y = 480;
				mmi->ptMaxPosition.x = 10;
				mmi->ptMaxPosition.y = 10;
				mmi->ptMaxSize.x = 640;
				mmi->ptMaxSize.y = 480;
				return 0;

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

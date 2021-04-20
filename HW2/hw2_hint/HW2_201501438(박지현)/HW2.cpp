#define _CRT_SECURE_NO_WARNINGS
#define gap 30
#define maxsize 20
#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <time.h>
#include "HW2.h"

using namespace std;

int centerx, centery =  0; //��ü ��ũ���� �߽�x,y��ǥ 

D2D_POINT_2F lastdown; // ���� �ֱٿ� ������ ��ǥ
D2D_POINT_2F current; // ���� ���콺 ��ǥ 

vector <student> v; // �л� �����͵��� ��� ���� v
 
float Angle = 0; // ���� ���� ����
float Size = 0; // ������ ���� ���� 
int stacksize = 0; // ���� �л����� �������� ������ 

boolean inserttingflag = false; // ������ �ǰ� �ִ� ������ ��Ÿ��
boolean inserttedflag = false; // ������ ��ٴ� ���� ��Ÿ��
boolean deletedflag = false; // ������ �ǰ� �ִ� ������ ��Ÿ��
boolean deletingflag = false; // ������ ��ٴ� ���� ��Ÿ��

D2D1_RECT_F insertrectangle;


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
	m_boundaryBrush(NULL),
	m_RectangleBrush(NULL),
	m_pFillBrush(NULL),
	m_pTextBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL)
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pStrokeStyleDash);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
	SAFE_RELEASE(m_pOriginalShapeBrush);
	SAFE_RELEASE(m_boundaryBrush);
	SAFE_RELEASE(m_RectangleBrush);
	SAFE_RELEASE(m_pFillBrush);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
}

HRESULT DemoApp::Initialize(HINSTANCE hInstance)
{
	HRESULT hr;

	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = hInstance;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"HW2_201501438(������)";
		RegisterClassEx(&wcex);

		m_hwnd = CreateWindow(
			L"HW2_201501438(������)", L" HW2_201501438(������)",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			800, 900, NULL, NULL, hInstance, this
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

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

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

	if (SUCCEEDED(hr))
	{
		// ���� ȹ ��Ÿ�� ��ü�� ������.
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

		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
		);

		if (SUCCEEDED(hr))
		{
			// ��ȯ �� ����� �ܰ����� ���� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &m_pOriginalShapeBrush);
		}

		if (SUCCEEDED(hr))
		{
			// ��ȯ �� ������ ä��� �뵵�� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pFillBrush);
		}

		if (SUCCEEDED(hr))
		{
			// �ؽ�Ʈ�� ���� ���� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrush);
		}

		if (SUCCEEDED(hr))
		{
			// ��ȯ �� ����� �ܰ����� ���� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Navy), &m_boundaryBrush);
		}
		if (SUCCEEDED(hr))
		{
			// ��ȯ �� ����� �ܰ����� ���� ���� ����.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSkyBlue), &m_RectangleBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
		}
	}

	return hr;
}

HRESULT DemoApp::CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, ID2D1BitmapBrush** ppBitmapBrush)
{
	HRESULT hr = S_OK;

	// ȣȯ ����Ÿ���� ����.
	ID2D1BitmapRenderTarget* pCompatibleRenderTarget = NULL;
	hr = pRenderTarget->CreateCompatibleRenderTarget(D2D1::SizeF(10.0f, 10.0f), &pCompatibleRenderTarget);

	if (SUCCEEDED(hr))
	{
		// ������ �׸�.
		ID2D1SolidColorBrush* pGridBrush = NULL;
		hr = pCompatibleRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f), &pGridBrush);

		if (SUCCEEDED(hr))
		{
			pCompatibleRenderTarget->BeginDraw();
			pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 10.0f, 1.0f), pGridBrush);
			pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 1.0f, 10.0f), pGridBrush);
			hr = pCompatibleRenderTarget->EndDraw();

			if (hr == D2DERR_RECREATE_TARGET)
			{
				// ��� ��ġ ������ �ڿ����� �ݳ�.
				DiscardDeviceResources();
			}
			if (SUCCEEDED(hr))
			{
				// ����Ÿ�����κ��� ��Ʈ���� ����.
				ID2D1Bitmap* pGridBitmap = NULL;
				hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);

				if (SUCCEEDED(hr))
				{
					// ��Ʈ�� ���� ������.
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

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_boundaryBrush);
	SAFE_RELEASE(m_pFillBrush);
	SAFE_RELEASE(m_pOriginalShapeBrush);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pGridPatternBitmapBrush);
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

void dataPush() { // data�� vector v�� push���� 
	srand(time(NULL));
	int grade = rand() % 100; // ������ 0~100�� ������ �� 
	string name = randumsung[rand() % 10]; // �̸��� �����ϰ� ���� 
	for (int i = 0; i < rand() % 3 + 1; i++) { // �̸��� 2~4���� �̹Ƿ�
		name = name + randname[rand() % 24];
	}
	v.push_back(student(name, grade));
}
D2D1_RECT_F getNextRectangle(int stackSize) { // stacksize�� ������ ������ �׷��� �簢�� ��ǥ�� �� 
	float top = 870 - 30 * (stackSize + 2);
	return D2D1::RectF(centerx - 60, top, centerx + 60, top + 30);
}
void string2wstring(wstring &dest, const string &src)
{
	dest.resize(src.size());
	for (unsigned int i = 0; i < src.size(); i++)
		dest[i] = static_cast<unsigned char>(src[i]);
}

HRESULT DemoApp::OnRender() 
{
	HRESULT hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		// �׸��⸦ ������.
		m_pRenderTarget->BeginDraw(); 
	
		// ����Ÿ�� ��ȯ�� �׵� ��ȯ���� ������.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); 
		// ����Ÿ�� ������ Ŭ������.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		m_pRenderTarget->FillRectangle(
			D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height),
			m_pGridPatternBitmapBrush);

		if (deletingflag == true || deletedflag == true) { //deletingflag�� deletedflag�� true�̸� ��� stacksize�� �ٿ��ش�. 
			stacksize--;
		}
		for (int i = 0; i < stacksize; i++) { // v�� ����� ������ �׷��ش�. 
			m_pRenderTarget->FillRectangle(getNextRectangle(i), m_RectangleBrush); // �״��� �׷��� ���� ��ġ�� ���� �簢���� �׷��ش�. 
			const char* multiByte = v[i].name.c_str(); // v[i].name�� const char*������ �ٲ۴�. 
			TCHAR temp[15]; // const char*�� TCHAR������ �ٲ�� �ϱ� ������ temp������ �����Ѵ�. 
			memset(temp, 0, sizeof(temp)); 
			//memset(str, 0, sizeof(str));
			MultiByteToWideChar(CP_ACP, MB_COMPOSITE, multiByte, -1, temp, 15);
			static WCHAR szText[100]; // name�� ������ �ѹ��� �־�� ��Ƿ� WCHAR �迭�� ������ �����Ѵ�. 
			swprintf_s(szText, L"%s %d\n", temp, v[i].score);
			m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, getNextRectangle(i), m_pTextBrush); // Text�� �׷��ش�.
			m_pRenderTarget->DrawRectangle(getNextRectangle(i), m_boundaryBrush); // ���� �׵θ��� �׷��ش�. 
		}
		//m_pFillBrush->SetOpacity(0.5f);
		if (deletingflag == true || deletedflag == true) { // �ٿ����� stacksize�� �ٽ� �÷��ش�.
			stacksize++;
		}
		drawInit();
		if (inserttingflag == true) { // ���� �����ϰ� �ִ°����̶��
			drawTempInsertingRectangle(); // �巡���ϴ� ������ ���ڸ� �׷��ش�. 
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->DrawRectangle(getNextRectangle(stacksize), m_pOriginalShapeBrush); // ���� ��ġ�� ǥ�����ش�. 
		}
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		if (deletingflag == true) {
			drawTempdeletingRectangle();

		}
		//DrawAndTranslateARectangle();
		if (inserttedflag == true) { // ���� �Ϸ������ 
	/*		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->FillRectangle(getNextRectangle(stacksize), m_pOriginalShapeBrush);*/
		
			stacksize++; // ���û���� �÷��ش�. 
			dataPush(); // ���Ϳ� data�� �־��ش�. 
			inserttedflag = false; 
		}
		if (deletedflag == true) { // �����Ǿ�����
			stacksize--; //stacksize�� �ٿ��ش�.
			v.pop_back(); // ���Ϳ��� ���� ���´�. 
			deletedflag = false; 
		}
		// �׸��� ������� ������.
		m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}



void DemoApp::drawTempInsertingRectangle() {
	D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(current.x - lastdown.x, current.y - lastdown.y); // �ֱ� ������ ��ǥ���� ���� ��ǥ���� �Ÿ���ŭ �̵������ش�. 
	Size = ((current.y - lastdown.y) / (getNextRectangle(stacksize).top - 30)) * 3 + 1; // ��ǥ �������� �󸶸�ŭ �Դ��� ������ ���ؼ� Size�� �����ݴϴ�. 
	D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(Size, 1.0f), D2D1::Point2F(insertrectangle.left , insertrectangle.top)); 
	Angle = (current.y - lastdown.y) / (getNextRectangle(stacksize).top - 30) * 360; // Size�� ����ϰ� ��ǥ �������� �󸶸�ŭ �Դ��� ������ ���ؼ� Angle�� �����ݴϴ�. 
	D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(Angle, D2D1::Point2F((insertrectangle.right + insertrectangle.left)/2, (insertrectangle.top + insertrectangle.bottom)/2));
	m_pRenderTarget->SetTransform(scale * rotation * translation); // scale->rotation->translation ��
	
	m_pRenderTarget->FillRectangle(insertrectangle, m_pOriginalShapeBrush); //insertrectangled�� ��ȯ�Ͽ� �׷��ݴϴ�.

}

void DemoApp::drawTempdeletingRectangle() {
	stacksize--;
	D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(current.x - lastdown.x, current.y - lastdown.y); // �ֱ� ������ ��ǥ���� ���� ��ǥ���� �Ÿ���ŭ �̵������ش�.
	if (current.y <= lastdown.y) // ���� ���� �巡�� �Ѵٸ�
		Size = (getNextRectangle(stacksize).top - 30) / ((lastdown.y - current.y) * 3 + (getNextRectangle(stacksize).top - 30)); // ��ǥ �������� �󸶸�ŭ �Դ��� ������ ���ؼ� Size�� �����ݴϴ�. 
	else //���� �Ʒ��� �巡�� �Ѵٸ� 
		Size = ((current.y - lastdown.y) / (getNextRectangle(stacksize).top - 30)) * 3 + 1; // ���� �Ҷ��� ������ ���� 
	D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(Size, 1.0f), D2D1::Point2F(lastdown.x, lastdown.y)); 
	Angle = (lastdown.y - current.y) / (getNextRectangle(stacksize).top - 30) * 360 * -1; //Size�� ����ϰ� ��ǥ �������� �󸶸�ŭ �Դ��� ������ ������ 
	//D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(Angle, D2D1::Point2F((getNextRectangle(stacksize).right + getNextRectangle(stacksize).left) / 2, (getNextRectangle(stacksize - 1).top + getNextRectangle(stacksize - 1).bottom) / 2));
	D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(Angle, D2D1::Point2F(lastdown.x, lastdown.y));
	m_pRenderTarget->SetTransform(scale*rotation * translation);
	m_pRenderTarget->FillRectangle(getNextRectangle(stacksize), m_pOriginalShapeBrush); // �ֱ� �Էµ� ������ �������� ��ȯ�Ͽ� �׷��ݴϴ�.
	stacksize++;

}

// �̵� ��ȯ�� ����Ÿ�ٿ� �����ϰ� �׸�.
void DemoApp::drawInit()
{
	m_pOriginalShapeBrush->SetOpacity(0.3); // brsuh�� ������ �������ش�.
	D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize(); // rendertarget�� size�� �̿��Ͽ� 
	centerx = rtSize.width / 2; //��ü ��ũ���� �߾� ���� ���س���.
	centery = rtSize.height / 2;

	// ����Ÿ�� ��ȯ�� �׵� ��ȯ���� ������. �簢���� �׸�.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	insertrectangle = D2D1::Rect(centerx - 15, gap, centerx + 15, gap + 30); // ��� ��ܿ�
	m_pRenderTarget->FillRectangle(insertrectangle, m_pOriginalShapeBrush); // ������ ���� ���׸��� �簢���� �׷��� 

	// ĸ�� �ؽ�Ʈ�� ǥ����.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	WCHAR szText[100];
	_swprintf(szText, L"���콺 X:%.2f\n���콺 Y:%.2f\nȸ������:%.2f\nũ������ ���� :%.2f\n�ڽ����� : %d\n",
		current.x, current.y, Angle, Size, v.size());
	m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat,
		D2D1::RectF(10.0f, 10.5f, 236.0f, 190.5f), m_pTextBrush);
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
	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		return 1;
	}

	DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

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
		case WM_LBUTTONUP: // ���� �巡�װ� �������� 
		{
			inserttingflag = false; // inserttingflag�� deletingflag�� ��� false�� ���ش�. 
			deletingflag = false;
			Angle = 0; // Angle,Size�� �ʱ�ȭ
			Size = 0;
			break;

		}
		case WM_LBUTTONDOWN:
		{
			
			if (LOWORD(lParam) >= insertrectangle.left && LOWORD(lParam) <= insertrectangle.right
				&& HIWORD(lParam) >= insertrectangle.top && HIWORD(lParam) <= insertrectangle.bottom) { // ���� ����� ��� �簢���� ���������� 
				inserttingflag = true; // insertting��带 ���ش�.
				lastdown.x = LOWORD(lParam);
				lastdown.y = HIWORD(lParam);
			}
			else if (v.size() == 0)
				break;
			else {
				D2D1_RECT_F lastRectangle = getNextRectangle(stacksize - 1);
				if (LOWORD(lParam) >= lastRectangle.left && LOWORD(lParam) <= lastRectangle.right
					&& HIWORD(lParam) >= lastRectangle.top && HIWORD(lParam) <= lastRectangle.bottom) { // ���� �ֱ� ���Ե� ���ڰ� ����������
					deletingflag = true; // deleting��带 ���ش�. 
					lastdown.x = LOWORD(lParam);
					lastdown.y = HIWORD(lParam);
				}

			}
			break;
		}
		case WM_PAINT:
		{
			pDemoApp->OnRender();
			ValidateRect(hwnd, NULL);
			return 0;
		}
	
		case WM_MOUSEMOVE:
		{
			current.x = LOWORD(lParam);
			current.y = HIWORD(lParam);
			if (inserttingflag == true) {
				if (stacksize >= maxsize) { // ���� maxsize�� �ʰ��Ѵٸ�
					inserttingflag = false; // inserttingflag�� false�� ó�����ش�. 
					::MessageBox(0, L"the number of box is 20 , it's limit", L"Fatal Error", MB_OK);//�޼����� ����ش�.
					::MessageBeep(MB_OK);
					break;
				}
				D2D1_RECT_F currentstack = getNextRectangle(stacksize);
				if (LOWORD(lParam) >= currentstack.left && LOWORD(lParam) <= currentstack.right
					&& HIWORD(lParam) >= currentstack.top && HIWORD(lParam) <= currentstack.bottom) { // inserttingflag�� true�̰� ���;��� ���ÿ� ���콺 Ŀ���� ��������
					inserttedflag = true; // inserttedflag�� true�� �����ش�. 
					Angle = 0; //Angle ,Size �ʱ�ȭ 
					Size = 0;
					inserttingflag = false;
					//InvalidateRect(hwnd, NULL, false);
				}
				InvalidateRect(hwnd, NULL, false);
			}
			else if (deletingflag == true) { 
				if (LOWORD(lParam) >= insertrectangle.left && LOWORD(lParam) <= insertrectangle.right
					&& HIWORD(lParam) >= insertrectangle.top && HIWORD(lParam) <= insertrectangle.bottom) {  // deletingflag�� true�̰� insertingrectangle�� ���콺 Ŀ���� ��������
					deletedflag = true; // deletedflag�� true�� �����ش�.
					Angle = 0; //Angle,Size �ʱ�ȭ 
					Size = 0;
					deletingflag = false;
				}
				InvalidateRect(hwnd, NULL, false);
			}
			else {
				RECT rect;
				rect.left = 10; rect.top = 10.5; rect.right = 236; rect.bottom = 190.5;
				InvalidateRect(hwnd, &rect, true);
			}

			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return 1;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


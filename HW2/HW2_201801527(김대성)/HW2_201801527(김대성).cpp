// HW2_201801527(김대성).cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "HW2_201801527(김대성).h"

bool SortByScore(const Student& st1, const Student& st2)  // sort 함수에 사용되는 함수. 내림차순으로 정렬되게 함
{
	if (st1.score > st2.score)
	{
		return true;
	}

	else
	{
		return false;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	srand((unsigned)time(NULL));

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;

			if (SUCCEEDED(app.Initialize(hInstance)))
			{  // DemoApp 클래스의 instance 초기화
				app.RunMessageLoop();  // 메시지루프 시작
			}
		}

		CoUninitialize();
	}

	return 0;
}

// 생성자에서는 클래스 변수들을 NULL로 초기화
DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL),
	m_pLightSlateGrayBrush(NULL),
	m_pCornflowerBlueBrush(NULL),
	m_pTransformedShapeBrush(NULL),
	m_pFillBrush{ NULL , },
	m_pTextBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL)
{
}

// 소멸자에서는 모든 인터페이스들을 반납
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

void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT DemoApp::Initialize(HINSTANCE hInstance)
{
	HRESULT hr;
	// 장치 독립적 자원을 생성함.
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		// 윈도우 클래스를 등록함..
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

		// 윈도우를 생성함.
		m_hwnd = CreateWindow(L"D2DDemoApp", L"Direct2D Demo Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, this);// 이유를 모르겠는데 이름을 바꾸면 실행이 안됩니다.

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

	// D2D 팩토리를 생성함.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteFactory->CreateTextFormat(L"Verdana", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 13.0f, L"en-us", &m_pTextFormat);
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
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			// 회색 붓을 생성함.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray), &m_pLightSlateGrayBrush);
		}

		if (SUCCEEDED(hr))
		{
			// 파란색 붓을 생성함.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
		}

		if (SUCCEEDED(hr))
		{
			// 변환 후 모양의 외곽선을 위한 붓을 생성.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &m_pTransformedShapeBrush);
		}

		float tmp[3];

		for (int i = 0; i < 10; ++i)
		{
			tmp[0] = rand() % 101 / 100.0;
			tmp[1] = rand() % 101 / 100.0;
			tmp[2] = rand() % 101 / 100.0;

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(tmp[0], tmp[1], tmp[2], 0.5), &m_pFillBrush[i]);
			}
		}

		if (SUCCEEDED(hr))
		{
			// 텍스트를 쓰기 위한 붓을 생성.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrush);
		}
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	// CreateDeviceResources 함수에서 생성한 모든 자원들을 반납
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pLightSlateGrayBrush);
	SAFE_RELEASE(m_pCornflowerBlueBrush);
	for (int i = 0; i < 10; i++)
		SAFE_RELEASE(m_pFillBrush[i]);
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
		LPMINMAXINFO mmi;

		bool wasHandled = false;

		if (pDemoApp)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);  // 새 창의 크기
				UINT height = HIWORD(lParam);
				pDemoApp->OnResize(width, height);  // 창의 크기를 다시 조절
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE); //WM_PAINT를 발생시킴
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_LBUTTONDOWN:  // 마우스 왼쪽 버튼 클릭
			{
				mouseClick = true;

				mousePosition.x = LOWORD(lParam);
				mousePosition.y = HIWORD(lParam);

				if (topBox.top <= mousePosition.y && mousePosition.y <= topBox.bottom)
				{
					if (topBox.left <= mousePosition.x && mousePosition.x <= topBox.right)  // 위쪽 가상 상자를 클릭하면
					{
						if (studentNumber < 8)  // 학생이 8명 미만이라면
						{
							makeBoxFlag = true;

							student.score = (rand() % 50) + 50;  // 점수는 50 ~ 99
							student.name[0] = lastName[rand() % 4];  // 성

							for (int i = 1; i < 3; ++i) 
							{
								student.name[i] = firstName[rand() % 13];  // 이름 두 개
							}

							student.name[3] = '\0';  // NULL
						}

						else
						{
							MessageBoxW(hwnd, L"Already 8 Students!", L"ERROR", 0);  // 경고문구 출력
							makeBoxFlag = false;
						}
					}

					else
						makeBoxFlag = false;
				}

				else if (studentBox.top <= mousePosition.y && mousePosition.y <= studentBox.bottom)  // 상자 제거시
				{
					if (studentBox.left <= mousePosition.x && mousePosition.x <= studentBox.right)
					{
						if (studentNumber > 0)  // 생성된 학생이 있다면
						{
							delBoxFlag = true;							
						}

						else
						{
							MessageBoxW(hwnd, L"No Student in Box!", L"ERROR", 0);  // 경고문구 출력
							delBoxFlag = false;				
						}
					}

					else
						delBoxFlag = false;
				}

				else  // 다른 곳 클릭시
				{
					makeBoxFlag = false;
					delBoxFlag = false;
				}

				InvalidateRect(hwnd, NULL, false);
			}
			break;  // break 문 다는 것을 깜빡하여 고생을 좀 많이 했습니다...ㅠㅠ

			case WM_LBUTTONUP:  // 마우스 땔 시
			{
				mouseClick = false;

				mousePosition.x = LOWORD(lParam);
				mousePosition.y = HIWORD(lParam);

				if (bottomBox.top <= mousePosition.y && mousePosition.y <= bottomBox.bottom)  // 박스를 만듬
				{
					if (bottomBox.left <= mousePosition.x && mousePosition.x <= bottomBox.right)
					{
						if (makeBoxFlag)
						{
							studentVector.push_back(student);

							studentNumber++;  // 학생의 수 1 증가

							sort(studentVector.begin(), studentVector.end(), SortByScore);  // vector를 내림차순으로 정렬
						}						
					}
				}

				else if (topBox.top <= mousePosition.y && mousePosition.y <= topBox.bottom)  // 상자 제거시, 이 부분 또한, 상하 비교가 아닌 좌우 비교로 하여 위쪽 if문에 먼저 걸리는 문제가 있었습니다.
				{
					if (topBox.left <= mousePosition.x && mousePosition.x <= topBox.right)  // 해결하는데 상당한 시간을 소요했습니다. ㅠㅠ
					{
						if (delBoxFlag)
						{
							studentVector.pop_back();

							studentNumber--;
						}
					}
				}
			
				makeBoxFlag = false;
				delBoxFlag = false;				

				InvalidateRect(hwnd, NULL, false);
			}
			break;

			case WM_MOUSEMOVE:
			{
				mousePosition.x = LOWORD(lParam);
				mousePosition.y = HIWORD(lParam);

				InvalidateRect(hwnd, NULL, FALSE);
			}
			break;

			case WM_PAINT:
			{
				pDemoApp->OnRender();  // 창을 그림
				ValidateRect(hwnd, NULL);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_GETMINMAXINFO:  // 화면 크기 조절 방지
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
			result = DefWindowProc(hwnd, message, wParam, lParam);
	}

	return result;
}

HRESULT DemoApp::OnRender()
{
	HRESULT hr = S_OK;
	WCHAR szText[100];

	hr = CreateDeviceResources(); // 항상 호출되며, 렌더타겟이 유효하면 아무일도 하지 않음.

	if (SUCCEEDED(hr))
	{ //렌더타겟이 유효함.
		m_pRenderTarget->BeginDraw(); //그리기 시작.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); //변환행렬을 항등행렬로.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White)); // 창을 클리어.

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize(); //그리기 영역의 크기를 얻음.

		// 배경 격자를 그림.
		int width = static_cast<int>(rtSize.width);
		int height = static_cast<int>(rtSize.height);

		topBox.left = rtSize.width / 2 - 40.0f;
		topBox.top = 0;
		topBox.right = rtSize.width / 2 + 40.0f;
		topBox.bottom = 40.0f;

		studentBox.left = rtSize.width / 2 - 80.0f;
		studentBox.top = 40.0f * 8;
		studentBox.right = rtSize.width / 2 + 80.0f;
		studentBox.bottom = 40.0f * 9;

		for (int x = 0; x < width; x += 10)
		{
			m_pRenderTarget->DrawLine(D2D1::Point2F(static_cast<FLOAT>(x), 0.0f), D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height), m_pLightSlateGrayBrush, 0.5f);
		}

		for (int y = 0; y < height; y += 10)
		{
			m_pRenderTarget->DrawLine(D2D1::Point2F(0.0f, static_cast<FLOAT>(y)), D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)), m_pLightSlateGrayBrush, 0.5f);
		}
		

		if (makeBoxFlag || delBoxFlag)
		{
			MakeChasingRect(mousePosition, bottomBox.bottom);
		}

		if (studentNumber < 8)  // 학생 수가 8명 미만이면 가상 박스 생성
		{
			bottomBox.left = rtSize.width / 2 - 80.0f;
			bottomBox.top = 40.0f * (8 - studentNumber);
			bottomBox.right = rtSize.width / 2 + 80.0f;
			bottomBox.bottom = 40.0f * (9 - studentNumber);

			m_pLightSlateGrayBrush->SetOpacity(0.2f);
			D2D1_RECT_F bottomGrayBox = D2D1::RectF(bottomBox.left, bottomBox.top, bottomBox.right, bottomBox.bottom);  // 가상 직사각형의 너비는 높이의 두 배
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->FillRectangle(&bottomGrayBox, m_pLightSlateGrayBrush);
		}// 데이터 직사각형은 높이와 너비가 1 : 4 비율

		if (studentNumber > 0)  // 학생 박스 생성
		{
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			for (int i = 0; i < studentNumber; ++i)   // 데이터 직사각형은 높이와 너비가 1 : 4 비율
			{
				studentBox.left = rtSize.width / 2 - 80.0f;
				studentBox.top = 40.0f * (8 - i);
				studentBox.right = rtSize.width / 2 + 80.0f;
				studentBox.bottom = 40.0f * (9 - i);

				D2D1_RECT_F studentInfoBox = D2D1::RectF(studentBox.left, studentBox.top, studentBox.right, studentBox.bottom);
				_swprintf(szText, L"%s\n%d", studentVector[i].name, studentVector[i].score);
				m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, studentInfoBox, m_pTextBrush);
				m_pRenderTarget->DrawRectangle(&studentInfoBox, m_pTransformedShapeBrush);
				m_pRenderTarget->FillRectangle(&studentInfoBox, m_pFillBrush[i]);
			}
		}

		m_pLightSlateGrayBrush->SetOpacity(0.2f);
		D2D1_RECT_F topGrayBox = D2D1::RectF(topBox.left, topBox.top, topBox.right, topBox.bottom);  // 가상 직사각형의 너비는 높이의 두 배
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->FillRectangle(&topGrayBox, m_pLightSlateGrayBrush);

		_swprintf(szText, L"Mouse Clicked : %d\nX Pos : %.2f\nY Pos : %.2f\nRotate : %.2f\nScale : %.2f\n", mouseClick, mousePosition.x, mousePosition.y, info_r, info_s);  // 마우스 눌림, 현재 마우스의 좌표, rotate 및 scale 정보 출력
		D2D1_RECT_F infoMouse = D2D1::RectF(0, 0, 120, 80);  // 좌측 정보창 사각형
		m_pRenderTarget->DrawRectangle(&infoMouse, m_pCornflowerBlueBrush);
		m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, infoMouse, m_pTextBrush);

		_swprintf(szText, L"Student Box\nLeft : %d\nTop : %d\nRight : %d\nBottom : %d\n", studentBox.left, studentBox.top, studentBox.right, studentBox.bottom);  // 학생 박스 정보 출력
		D2D1_RECT_F infoStudentBox = D2D1::RectF(0, 80, 120, 160);  // 좌측 정보창 사각형
		m_pRenderTarget->DrawRectangle(&infoStudentBox, m_pCornflowerBlueBrush);
		m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, infoStudentBox, m_pTextBrush);

		_swprintf(szText, L"Top Box\nLeft : %d\nTop : %d\nRight : %d\nBottom : %d\n", topBox.left, topBox.top, topBox.right, topBox.bottom);  // 상단 회색 박스 정보 출력
		D2D1_RECT_F infoMakeBox = D2D1::RectF(0, 160, 120, 240);  // 좌측 정보창 사각형
		m_pRenderTarget->DrawRectangle(&infoMakeBox, m_pCornflowerBlueBrush);
		m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, infoMakeBox, m_pTextBrush);

		_swprintf(szText, L"Bottom Box\nLeft : %d\nTop : %d\nRight : %d\nBottom : %d\n", bottomBox.left, bottomBox.top, bottomBox.right, bottomBox.bottom);  // 하단 가상 박스 정보 출력
		D2D1_RECT_F infoBottomBox = D2D1::RectF(0, 240, 120, 320);  // 좌측 정보창 사각형
		m_pRenderTarget->DrawRectangle(&infoBottomBox, m_pCornflowerBlueBrush);
		m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, infoBottomBox, m_pTextBrush);

		hr = m_pRenderTarget->EndDraw();  //그리기를 수행함. 성공하면 S_OK를 리턴함.
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{ // 렌더타겟을 재생성해야 함.
		// 실행중에 D3D 장치가 소실된 경우,
		hr = S_OK;
		DiscardDeviceResources(); // 장치 의존 자원들을 반납함.
		// 다음번 OnRender 함수가 호출될 때에 재생성함.
	}

	return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
	// 창이 resize될 경우에 렌더타겟도 이에 맞도록 resize함.
	if (m_pRenderTarget)
	{
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
		// Resize함수가 실패할 수도 있으나, 예외처리하지 않았음.
		// 왜냐하면, 실패하는  경우에, 다음번 EndDraw 호출에서도 실패할 것이므로.
	}
}

D2D1_RECT_F DemoApp::MakeChasingRect(D2D_POINT_2F mousePos, FLOAT h)
{
	FLOAT scale_ratio, rotate_degree, alpha1, beta1, alpha2, beta2, temp_cur_h;
	WCHAR szText[100];
	int a;

	////////////////////scale 변환 비율을 구하는 식
	if (mousePos.y < 40)
	{
		scale_ratio = 1;
		rotate_degree = 0;
	}

	else
	{
		temp_cur_h = bottomBox.bottom - 20;
		alpha1 = 3 / (float)(temp_cur_h - 20);
		alpha2 = 360 / (float)(temp_cur_h - 20);
		beta1 = 1 - 20 * alpha1;
		beta2 = -20 * alpha2;
		scale_ratio = alpha1 * mousePos.y + beta1;
		rotate_degree = 360 - (int)(alpha2 * mousePos.y + beta2) % 360;  // 기존 코드 살짝 변경. 360에서 기존 값을 빼주면 됌
	}

	if (scale_ratio > 2)
		scale_ratio = 2.0f;
	////////////////////////////////////////

	D2D1_RECT_F F = D2D1::RectF(mousePosition.x - 40.0f, mousePosition.y - 20.0f, mousePosition.x + 40.0f, mousePosition.y + 20.0f);

	// 크기조정 변환을 렌더타겟에 지정하고 사각형을 채워 그림.
	D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(scale_ratio, 1.0f), D2D1::Point2F(mousePos.x, mousePos.y));
	D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(rotate_degree, D2D1::Point2F(mousePos.x, mousePos.y));

	if (delBoxFlag)
	{
		a = studentNumber - 1;
	}
		
	else
	{
		a = studentNumber;
	}

	if (studentNumber != 0 && delBoxFlag)
	{
		_swprintf(szText, L"%s\n%d", studentVector[studentNumber - 1].name, studentVector[studentNumber - 1].score);
	}

	else
	{
		_swprintf(szText, L"%s\n%d", student.name, student.score);
	}
	
	m_pRenderTarget->SetTransform(scale * rotation);
	m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, F, m_pTextBrush);
	m_pRenderTarget->DrawRectangle(F, m_pTransformedShapeBrush);
		
	m_pRenderTarget->FillRectangle(F, m_pFillBrush[a]);

	info_s = scale_ratio;
	info_r = rotate_degree;

	return F;
}

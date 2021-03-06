// Hw1.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Hw1.h"

#define MAX_LOADSTRING 100
#define MAX_STACK 6

int Height;
std::stack<Student, std::vector<Student>>stack;
std::stack<Student, std::vector<Student>> tempstack;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.


std::string last[5] = { "김","박","이","최","정" };
std::string first[8] = { "인천","광주","대구","창원","서울","대전","부산","울산" };

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	srand((unsigned)time(NULL));
    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HW1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HW1));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HW1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HW1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x, y; //마우스 좌표
	HBRUSH BG, color; //채우기 속성을 위한 Brush의 핸들
	int bot;
	int stack_size;
	char grd[10];
	Student tempStudent;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_LBUTTONDOWN:  //마우스 왼클릭 
	{
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		stack_size = stack.size();


		if (0 < x && 400 > x && (Height - 80 * stack_size) < y && y < (Height - 80 * (stack_size - 1))) {//x가 0과 400사이 
			stack.pop();
			InvalidateRect(hWnd, NULL, TRUE); //지금 그려저 있는 그림을 전부 무효화하고 다시그림
			return 0;
			//상자 클릭
		}
		if (stack_size == MAX_STACK) {
			MessageBoxA(hWnd, "Full", "Message", MB_OK);
			return 0;
			//스택 꽉참
		}
		InvalidateRect(hWnd, NULL, TRUE);
		Student s;
		std::string name = "";
		name = last[rand() % 5];	//랜덤으로 성
		s.grade = 20 + rand() % 30;						//성적 20-50
		for (int j = 0; j < 1; j++) {		//성 제외한 이름
			name += first[rand() % 8];
		}
		s.name = name;
		stack.push(s);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		bot = Height;
		stack_size = stack.size();			//스택 사이즈
		for (int j = 0; j < stack_size; j++) {

			BG = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255)); //랜던함 색을 갖는 브러쉬 객체 생성
			color = (HBRUSH)SelectObject(hdc, BG);
			_itoa_s(stack.top().grade, grd, 10);
			Rectangle(hdc, 0, bot - 80 * stack_size, 400, bot - 80 * stack_size + 80);
			TextOutA(hdc, 20, bot - 80 * stack_size + 20, grd, strlen(grd));
			TextOutA(hdc, 20, bot - 80 * stack_size + 40, stack.top().name.c_str(), stack.top().name.length());
			bot = bot + 80;
			tempStudent = stack.top();
			tempstack.push(tempStudent);
			stack.pop();
		}
		stack = tempstack;
		for (int i = 0; i < stack_size; i++)	
			tempstack.pop();
		EndPaint(hWnd, &ps);
	}
	break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_SIZE:
	{
		Height = HIWORD(lParam);
		break;
	}
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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

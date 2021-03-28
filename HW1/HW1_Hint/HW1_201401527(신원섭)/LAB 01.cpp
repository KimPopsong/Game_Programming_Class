// LAB 01.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "LAB 01.h"
#include <stack>
#include <string> // 문자열
#include <iostream>
#include <vector>
#include <fstream>
#include <windows.h>  // 이 헤더파일은 Win32 API 관련 구조와 함수 선언들을 포함함.
#include <Windowsx.h> // SetDlgMsgResult... window message crackers
#include <functional> //greater
#include "resource.h" // 리소스에 대한 디파인 값들이 리소스 헤더에 정의되어 있기 때문에 추가해준다.
#define MAX_LAST_NAME 18
#define MAX_FIRST_NAME 21
#define STACK_SIZE 6
using namespace std;

#define MAX_LOADSTRING 100

// 박스의 위치 값
#define BOX_LEFT 130
#define BOX_TOP 560
#define BOX_RIGHT 380
#define BOX_BOTTOM 630

// 새로운 박스를 위한 간격
#define GAP 80

int top=BOX_TOP,bottom=BOX_BOTTOM;

// 상자
RECT rect;

// 대화상자를 사용하기 위한 전역변수
HINSTANCE hInstance =NULL;      //프로그램의 인스턴스 핸들
HINSTANCE g_hInstance = NULL;
LPCTSTR lpTemplate;      //대화상자 템플릿의 ID
HWND hwnd;          //부모 윈도우의 윈도우 핸들
DLGPROC DlgProc;      //대화상자의 메세지를 처리해줄 프로시저
// 멤버 추가 부분 (DialogBox)
HWND input_Name;
HWND input_Grade;
// 박스의 개수
int boxnum = 0;

// 칼라지정 지정을 위한 COLORREF
COLORREF color(int i){
	COLORREF color;
	if (i == 0) color = 0x000081;
	else if (i == 1) color = 0x2019A5;
	else if (i == 2) color = 0x3030c9;	
	else if (i == 3) color = 0x5042ed;
	else if (i == 4) color = 0x7266ff;
	else if (i == 5) color = 0x9478ff;
	return color;
}

// 구조체 생성
typedef struct {
	int grade;		//성적
	TCHAR name[20];	//이름
} Student;


std::stack<Student, std::vector<Student>> mystack;
Student s;

// 큐 사이즈
int stacksize = 0;
void Insert(){

	char* lastnames[MAX_LAST_NAME] = { 
		"김", "이", "박",
		"최", "강", "유",
		"노", "정", "오",
		"송", "서", "한",
		"홍", "고", "신",
		"조", "장", "임" };		//랜덤 성씨
	char* firstnames[MAX_FIRST_NAME] = { 
		"섭", "지", "정",
		"수", "인", "철",
		"현", "석", "준",
		"은", "연", "범",
		"동", "원", "영",
		"소", "하", "호",
		"진", "명", "윤" };	//랜덤 이름
	std::string name = "";
	


		Student s;
		s.grade = rand() % 31+20;					//성적은 20~50점

		name = lastnames[rand() % MAX_LAST_NAME];	//랜덤 성씨

		for (int j = 0; j < 2; j++) {		//성씨 제외한 이름은 3자
			name += firstnames[rand() % MAX_FIRST_NAME];
		}
		
		// 구조체 name에 저장된 성명을 넣는데 c_str을 이용해 String을 TCHAR 형태로 바꾸어 준다.
		strcpy_s(s.name, name.c_str());
		
		// 저장된 구조체를 큐에 넣는다.
		mystack.push(s);

}


// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK	DIALOG(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInstance = hInstance; // 운영체제가 보내준 현재 프로그램의 인스턴스 객체를 전역변수로 사용하도록 받아옵니다.

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;
	srand((unsigned)time(NULL));

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY201401527, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY201401527));

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
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
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY201401527));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MY201401527);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//   함수: OnPaint(HWND hWnd, int top, int bottom) 
//
//   목적: 상자를 그립니다.
//
//   설명: 상자 그리기 (
//
LRESULT OnPaint(HWND hWnd, int top, int bottom)
{
	HDC hdc = GetDC(hWnd); // 화면 영역 값을 위해서 필요한 dc값
	HDC memdc; // 비트맵을 하나씩 불러오면 너무 느리기 때문에 미리 메모리에 올려놓고 불러오는 방법을 사용한다.

	PAINTSTRUCT ps; // 영역에 대한 상세 정보
	static HBITMAP hBitMap_Char; // 비트맵을 불러오기 위해서 필요한 비트맵 핸들 변수를 만들어준다.

	hdc = BeginPaint(hWnd, &ps);
	// 아래 부분에 그리기 코드를 쓰면 된다.
	// 배경 비트맵 그리기
	hBitMap_Char = (HBITMAP)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP_BOX)); // 만들어준 비트맵 아이디 값을 줍니다.
	memdc = CreateCompatibleDC(hdc); // hdc 영역에 대한 메모리 영역을 memdc에 받아줍니다.
	SelectObject(memdc, hBitMap_Char); // 이렇게 memdc에 비트맵을 올려줍니다.
	BitBlt(hdc, 0, 0, 1040, 780, memdc, 0, 0, SRCCOPY); //memdc에 있는 걸 hdc로 옮겨줍니다. SRCCOPY 영역만큼 복사해줍니다.
	DeleteObject(memdc);

	//stack모양 그려주기
	HPEN spen = ::CreatePen(PS_SOLID, 10, RGB(0, 0, 0));
	HPEN soldPen = (HPEN)::SelectObject(ps.hdc, spen);
	MoveToEx(ps.hdc, BOX_LEFT-5, top - (GAP * 5), NULL);
	LineTo(ps.hdc, BOX_LEFT-5, BOX_BOTTOM+5);
	LineTo(ps.hdc, BOX_RIGHT+5, BOX_BOTTOM+5);
	LineTo(ps.hdc, BOX_RIGHT+5, top - (GAP * 5));
	DeleteObject(spen);
	TCHAR count[3];
	
	// 박스 개수 찍어주기
	
	
	TextOut(hdc, 10, 10, "현재 박스의 개수 : ", 19);
	wsprintf(count, TEXT("%d"), stacksize);
	TextOut(hdc, 150, 10, count, lstrlen(count));

	CHAR buf[10];

	GetClientRect(hWnd, &rect);

	int loop = 0;

	// 스택의 저장을 위해 temp 스택를 만든다.
	std::stack<Student, std::vector<Student>> temp = mystack;
	if (stacksize != 0){
		for (loop = stacksize - 1; loop >= 0; loop--){
			// 펜과 브러쉬로 상자 색상을 지정해준다.
			HPEN pen = ::CreatePen(PS_SOLID, 1, color(loop));
			HPEN oldPen = (HPEN)::SelectObject(ps.hdc, pen);
			HBRUSH MyBrush, OldBrush;
			sprintf_s(buf, "%d", temp.top().grade);
			MyBrush = (HBRUSH)CreateSolidBrush(color(loop));
			OldBrush = (HBRUSH)SelectObject(ps.hdc, MyBrush);
			SetBkColor(ps.hdc, color(loop));
			Rectangle(ps.hdc, BOX_LEFT, top - (GAP*loop), BOX_RIGHT, bottom - (GAP*loop)); // 사각형을 그림
			SetBkMode(ps.hdc, TRANSPARENT);
			SetTextColor(ps.hdc, RGB(255, 255, 255));
			TextOut(ps.hdc, 200, top + 25 - (GAP*loop), temp.top().name, strlen(temp.top().name)); // Member 구조체 배열을 출력한다.
			TextOut(ps.hdc, 290, top + 25 - (GAP*loop), buf, strlen(buf)); // Member 구조체 배열을 출력한다.
			SelectObject(ps.hdc, OldBrush);
			// top을 찍어준 후 pop을 하여 다음 것을 top 할 준비를 한다.
			temp.pop();
			DeleteObject(MyBrush);

			// 원래 객체를 복원함
		}
	}
	
	
	EndPaint(hWnd,&ps); // 그리기 코드가 끝날 경우 어디까지가 그리기 코드인지 알기 위해서 EndPaint를 해줍니다.
	return 0;
}


//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//

// 윈도우로 보내지는 메시지들을 처리하는 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	
	
	HDC hdc = GetDC(hWnd); // 화면 영역 값을 위해서 필요한 dc값
	static HBITMAP hBitMap_Char; // 비트맵을 불러오기 위해서 필요한 비트맵 핸들 변수를 만들어준다.

	static int x;
	static int y;
	switch (message)
	{
	case WM_CREATE: // 현재 창의 초기화 메시지
		hBitMap_Char = (HBITMAP)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP_BOX)); // 만들어준 비트맵 아이디 값을 줍니다.
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// 메뉴 선택을 구문 분석합니다.
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
		break;

		// WM_PAINT 란 화면이 지워진 상태에서 작동하는 것으로 다시 그려주는 역할을 한다.
	case WM_PAINT:

			// 스택 사이즈가 6이 넘지 않으면 상자를 그려줍니다.
			if (stacksize <= STACK_SIZE){
			OnPaint(hWnd, top, bottom);
			}
			//// 스택 사이즈가 6이면 더 이상 그릴 수 없으며, 경고 메시지를 띄워줍니다.
			//if (stacksize == STACK_SIZE){
			//	DialogBox(hInst, MAKEINTRESOURCE(IDD_ALERT), hWnd, About);}
		
		break;
	case WM_GETMINMAXINFO:
	{
		//최소 사이즈 지정
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;  //윈도우의 창크기
		mmi->ptMinTrackSize.x = 500;
		mmi->ptMinTrackSize.y = 700;
		mmi->ptMaxTrackSize.x = 500;
		mmi->ptMaxTrackSize.y = 700;
		return 0;
	}

	case WM_LBUTTONDOWN:
		
		// 클릭 좌표값을 받아옵니다.
		x = LOWORD(lParam);
		y = HIWORD(lParam);

		// 스택사이즈가 0이고 첫 번째 박스의 위치를 클릭하면 더 이상 삭제할 수 없다는 창을 띄워줍니다.
		if (stacksize == 0 && BOX_LEFT <x && BOX_RIGHT > x && top< y && bottom> y)
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ALERT1), hWnd, About);
		
		// 맨 위 사각형 눌렸을 때, 사각형을 지운다.
		if (BOX_LEFT <x && BOX_RIGHT > x && top - (GAP*(stacksize - 1))< y && bottom - (GAP*(stacksize - 1))> y)
		{
			stacksize--;
			mystack.pop();
			InvalidateRect(hWnd, NULL, TRUE);		//배경을 지우고 다시그린다
		}
		else{
			// 그 외는 스택에 값을 넣어주고 스택 사이즈를 증가시킵니다.
			if (stacksize < STACK_SIZE && !(BOX_LEFT <x && BOX_RIGHT > x && top< y && bottom> y)){
				Insert();
				stacksize++;
				InvalidateRect(hWnd, NULL, false);		//배경을 놔두고 다시그린다
			}
			else if (stacksize == STACK_SIZE)
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ALERT), hWnd, About);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
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




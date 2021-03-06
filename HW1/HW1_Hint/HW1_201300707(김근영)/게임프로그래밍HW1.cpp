// 게임프로그래밍HW1.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "게임프로그래밍HW1.h"
#include <vector>
#include <stack>
#include <string>
#include <stdlib.h>
#include <time.h>

#define MAX_LOADSTRING 100
using namespace std;
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
stack <pair <LPCWSTR,int>> s;
stack <pair <LPCWSTR, int>> s2;
int idx;
struct student
{
	WCHAR name[4];
	int score;
};
WCHAR lastname[] = L"김이박신최채장정주진유연";
WCHAR firstname[] = L"근종영순준재유원진인희성은서도지연민기현";
student t[6];

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 디버그 메시지를 표시하기 
#if defined(_DEBUG)&&defined(WIN32)&&!defined(_AFX)&&!defined(_AFXDLL)
#define TRACE	TRACE_WIN32
#pragma warning(disable: 4996)
void TRACE_WIN32(LPCTSTR lpszFormat, ...) {
	TCHAR lpszBuffer[0x160]; //buffer size
	va_list fmtList;
	va_start(fmtList, lpszFormat);
	_vstprintf(lpszBuffer, lpszFormat, fmtList);
	va_end(fmtList);
	::OutputDebugString(lpszBuffer);
}
#endif


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	srand(time(NULL));

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HW1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HW1));

    MSG msg;

    // 기본 메시지 루프입니다.
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
//  목적: 창 클래스를 등록합니다.
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
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 1024, 600, nullptr, nullptr, hInstance, nullptr); //4~7번째 매개변수를 변경하여 윈도우의 크기를 설정

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT OnPaint(HWND hWnd) 
{
	HDC hdc;
	PAINTSTRUCT ps;
	WCHAR buf[5];
	int ssize = s.size();
	hdc = BeginPaint(hWnd, &ps);

	for (int i = 0; i < s.size(); ++i) {
		HBRUSH Mybrush = CreateSolidBrush(RGB(255-i*4, 255-i*7, 255-t[i].score));
		HGDIOBJ hOldBrush = SelectObject(ps.hdc, Mybrush);
		RECT rt = { 400,430 + 20-(i*70),600,500+(i*70) };
		Rectangle(ps.hdc, 400, 500 - 70*(i+1), 400 + 200, 500-(70*i));
		DrawText(hdc, t[i].name , -1, &rt, DT_CENTER);
		RECT rt2 = { 400,430 + 40 - (i * 70),600,500 + (i * 70) };
		DrawText(hdc, _itow(t[i].score,buf,10), -1, &rt2, DT_CENTER);
		//TRACE(_T("%s"), Data[i].first);
		SelectObject(ps.hdc, hOldBrush);
		DeleteObject(Mybrush);
	}
	EndPaint(hWnd, &ps);
	return 0;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	WCHAR a[4];
	pair <LPCWSTR, int> p;
	
    switch (message)
    {
	case WM_LBUTTONDOWN:
		int param;
		// 왼쪽 마우스 버튼이 눌림.
		TRACE(_T("왼쪽 마우스 버튼이 눌러졌습니다! 좌표 = (%d, %d)\n"), LOWORD(lParam), HIWORD(lParam));
		param = s.empty() ? 0 : 1;
		if ((400* param <= LOWORD(lParam) && LOWORD(lParam) <= 600*param) && (500-(s.size()*70)) <= HIWORD(lParam) && HIWORD(lParam) <= (500-((s.size()-1)*70))) {
			if (s.empty()) {
				MessageBoxW(hWnd, L"이미 스택이 비어있습니다.", L"Empty Stack", 0);
			}
			else {
				TRACE(_T("맨위의 상자가 눌렸습니다.\n"));
				s.pop();
				InvalidateRect(hWnd, NULL, TRUE);
				--idx;
			}
		}
		else {
			if (s.size()<6){
				t[idx].score=(rand() % 30) + 20;
				t[idx].name[0] = lastname[rand() % 12];
				for (int i = 1; i < 3; ++i) {
					t[idx].name[i] = firstname[rand() % 20];
				}
				t[idx].name[3] = '\0';
				p = make_pair(t[idx].name, t[idx].score);
				//TRACE(_T("%s"), p.first);
				s.push(p);
				
				InvalidateRect(hWnd, NULL, TRUE);
				++idx;
			}
			else {
				MessageBoxW(hWnd, L"스택이 꽉찼습니다.", L"FULL STACK", 0);
			}
		}
		return 0;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
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
        }
        break;
    case WM_PAINT:
		OnPaint(hWnd);
        return 0;
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

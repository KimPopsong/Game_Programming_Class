﻿// HW1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "HW1.h"
#include <vector>
#include <stack>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <windowsx.h>

#define MAX_LOADSTRING 100

using namespace std;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

int left = 200;
int bottom = 600;
int right = 600;
int top = 700;

typedef struct {
	WCHAR name[4];
	int score;
}Student;

WCHAR lastname[] = L"김나박이";
WCHAR firstname[] = L"대성수연정민수한종혁석준식";

Student student[8];
vector<Student> studentVector;

int studentIndex;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

bool SortByScore(const Student& st1, const Student& st2)
{
	if (st1.score < st2.score)
	{
		return true;
	}

	else
	{
		return false;
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_HW1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
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

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HW1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_HW1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

LRESULT OnPaint(HWND hWnd)  // 사각형을 그리는 함수 From PPT p.27 and hint1 "김근영" 코드 참조
{
	HDC hdc;
	PAINTSTRUCT ps;
	WCHAR buf[5];

	hdc = BeginPaint(hWnd, &ps);

	for (int i = 0; i < studentVector.size(); ++i)
	{
		HBRUSH Mybrush = CreateSolidBrush(RGB(255 - i * 4, 255 - i * 7, 255 - student[i].score));
		HGDIOBJ hOldBrush = SelectObject(ps.hdc, Mybrush);
		RECT rt = { left, bottom + 50 - (i * 70), right, top + (i * 70) };  // left, ?, right, ?

		Rectangle(ps.hdc, left, bottom + 100 - 70 * (i + 1), right, top - (70 * i));
		DrawText(hdc, student[i].name, -1, &rt, DT_CENTER);

		RECT rt2 = { left, bottom + 70 - (i * 70), right, top + (i * 70) };
		DrawText(hdc, _itow(student[i].score, buf, 10), -1, &rt2, DT_CENTER);

		SelectObject(ps.hdc, hOldBrush);
		DeleteObject(Mybrush);
	}

	EndPaint(hWnd, &ps);

	return 0;
}


//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	pair <LPCWSTR, int> p;  // 뭘까??????????????????????????????????????????????????????????????????????????

	switch (message)
	{
	case WM_LBUTTONDOWN:  // 왼쪽 마우스 버튼이 눌렸을 때
	{
		int xpos = GET_X_LPARAM(lParam);  // 마우스의 x좌표
		int ypos = GET_Y_LPARAM(lParam);  // 마우스의 y좌표

		int param = studentVector.empty() ? 0 : 1;
		
		if ((left * param <= xpos && xpos <= right * param) && (bottom - ((studentVector.size()) * 70)) <= ypos && ypos <= (top - ((studentVector.size()) * 70))) ///////////수정 제발 수정해 ^^ㅣ발 제발
		{
			if (studentVector.empty())
			{
				MessageBoxW(hWnd, L"Stack Is Empty!", L"Empty Stack", 0);
			}

			else
			{
				studentVector.pop_back();
				InvalidateRect(hWnd, NULL, TRUE);
				studentIndex--;
			}
		}

		else  // 학생 생성
		{
			if (studentVector.size() < 8)  // 학생이 8명 미만이라면
			{
				student[studentIndex].score = (rand() % 50) + 50;  // 점수는 0 ~ 49 + 50 -> 50 ~ 99점 까지
				student[studentIndex].name[0] = lastname[rand() % 4];  // 무작위 성 배치

				for (int i = 1; i < 3; ++i) {
					student[studentIndex].name[i] = firstname[rand() % 13];  // 무작위 이름 두개 배치
				}

				student[studentIndex].name[3] = '\0';  // NULL

				studentVector.push_back(student[studentIndex]);  // vector에 저장

				InvalidateRect(hWnd, NULL, TRUE);

				studentIndex++;  // 학생의 수 1 증가

				sort(studentVector.begin(), studentVector.end(), SortByScore);  // vector를 내림차순으로 정렬, ERROR......
			}

			else
			{
				MessageBox(hWnd, L"Stack Is FULL!", L"FULL STACK", 0);
			}
		}

		InvalidateRect(hWnd, NULL, TRUE);
		OnPaint(hWnd);

		return 0;
	}

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

	case WM_PAINT:
	{
		OnPaint(hWnd);
		/*
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);
		*/
		return 0;
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

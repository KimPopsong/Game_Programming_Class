// LAB 01.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "LAB 01.h"
#include <stack>
#include <string> // ���ڿ�
#include <iostream>
#include <vector>
#include <fstream>
#include <windows.h>  // �� ��������� Win32 API ���� ������ �Լ� ������� ������.
#include <Windowsx.h> // SetDlgMsgResult... window message crackers
#include <functional> //greater
#include "resource.h" // ���ҽ��� ���� ������ ������ ���ҽ� ����� ���ǵǾ� �ֱ� ������ �߰����ش�.
#define MAX_LAST_NAME 18
#define MAX_FIRST_NAME 21
#define STACK_SIZE 6
using namespace std;

#define MAX_LOADSTRING 100

// �ڽ��� ��ġ ��
#define BOX_LEFT 130
#define BOX_TOP 560
#define BOX_RIGHT 380
#define BOX_BOTTOM 630

// ���ο� �ڽ��� ���� ����
#define GAP 80

int top=BOX_TOP,bottom=BOX_BOTTOM;

// ����
RECT rect;

// ��ȭ���ڸ� ����ϱ� ���� ��������
HINSTANCE hInstance =NULL;      //���α׷��� �ν��Ͻ� �ڵ�
HINSTANCE g_hInstance = NULL;
LPCTSTR lpTemplate;      //��ȭ���� ���ø��� ID
HWND hwnd;          //�θ� �������� ������ �ڵ�
DLGPROC DlgProc;      //��ȭ������ �޼����� ó������ ���ν���
// ��� �߰� �κ� (DialogBox)
HWND input_Name;
HWND input_Grade;
// �ڽ��� ����
int boxnum = 0;

// Į������ ������ ���� COLORREF
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

// ����ü ����
typedef struct {
	int grade;		//����
	TCHAR name[20];	//�̸�
} Student;


std::stack<Student, std::vector<Student>> mystack;
Student s;

// ť ������
int stacksize = 0;
void Insert(){

	char* lastnames[MAX_LAST_NAME] = { 
		"��", "��", "��",
		"��", "��", "��",
		"��", "��", "��",
		"��", "��", "��",
		"ȫ", "��", "��",
		"��", "��", "��" };		//���� ����
	char* firstnames[MAX_FIRST_NAME] = { 
		"��", "��", "��",
		"��", "��", "ö",
		"��", "��", "��",
		"��", "��", "��",
		"��", "��", "��",
		"��", "��", "ȣ",
		"��", "��", "��" };	//���� �̸�
	std::string name = "";
	


		Student s;
		s.grade = rand() % 31+20;					//������ 20~50��

		name = lastnames[rand() % MAX_LAST_NAME];	//���� ����

		for (int j = 0; j < 2; j++) {		//���� ������ �̸��� 3��
			name += firstnames[rand() % MAX_FIRST_NAME];
		}
		
		// ����ü name�� ����� ������ �ִµ� c_str�� �̿��� String�� TCHAR ���·� �ٲپ� �ش�.
		strcpy_s(s.name, name.c_str());
		
		// ����� ����ü�� ť�� �ִ´�.
		mystack.push(s);

}


// ���� ����:
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

	g_hInstance = hInstance; // �ü���� ������ ���� ���α׷��� �ν��Ͻ� ��ü�� ���������� ����ϵ��� �޾ƿɴϴ�.

 	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	HACCEL hAccelTable;
	srand((unsigned)time(NULL));

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY201401527, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY201401527));

	// �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//   �Լ�: OnPaint(HWND hWnd, int top, int bottom) 
//
//   ����: ���ڸ� �׸��ϴ�.
//
//   ����: ���� �׸��� (
//
LRESULT OnPaint(HWND hWnd, int top, int bottom)
{
	HDC hdc = GetDC(hWnd); // ȭ�� ���� ���� ���ؼ� �ʿ��� dc��
	HDC memdc; // ��Ʈ���� �ϳ��� �ҷ����� �ʹ� ������ ������ �̸� �޸𸮿� �÷����� �ҷ����� ����� ����Ѵ�.

	PAINTSTRUCT ps; // ������ ���� �� ����
	static HBITMAP hBitMap_Char; // ��Ʈ���� �ҷ����� ���ؼ� �ʿ��� ��Ʈ�� �ڵ� ������ ������ش�.

	hdc = BeginPaint(hWnd, &ps);
	// �Ʒ� �κп� �׸��� �ڵ带 ���� �ȴ�.
	// ��� ��Ʈ�� �׸���
	hBitMap_Char = (HBITMAP)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP_BOX)); // ������� ��Ʈ�� ���̵� ���� �ݴϴ�.
	memdc = CreateCompatibleDC(hdc); // hdc ������ ���� �޸� ������ memdc�� �޾��ݴϴ�.
	SelectObject(memdc, hBitMap_Char); // �̷��� memdc�� ��Ʈ���� �÷��ݴϴ�.
	BitBlt(hdc, 0, 0, 1040, 780, memdc, 0, 0, SRCCOPY); //memdc�� �ִ� �� hdc�� �Ű��ݴϴ�. SRCCOPY ������ŭ �������ݴϴ�.
	DeleteObject(memdc);

	//stack��� �׷��ֱ�
	HPEN spen = ::CreatePen(PS_SOLID, 10, RGB(0, 0, 0));
	HPEN soldPen = (HPEN)::SelectObject(ps.hdc, spen);
	MoveToEx(ps.hdc, BOX_LEFT-5, top - (GAP * 5), NULL);
	LineTo(ps.hdc, BOX_LEFT-5, BOX_BOTTOM+5);
	LineTo(ps.hdc, BOX_RIGHT+5, BOX_BOTTOM+5);
	LineTo(ps.hdc, BOX_RIGHT+5, top - (GAP * 5));
	DeleteObject(spen);
	TCHAR count[3];
	
	// �ڽ� ���� ����ֱ�
	
	
	TextOut(hdc, 10, 10, "���� �ڽ��� ���� : ", 19);
	wsprintf(count, TEXT("%d"), stacksize);
	TextOut(hdc, 150, 10, count, lstrlen(count));

	CHAR buf[10];

	GetClientRect(hWnd, &rect);

	int loop = 0;

	// ������ ������ ���� temp ���ø� �����.
	std::stack<Student, std::vector<Student>> temp = mystack;
	if (stacksize != 0){
		for (loop = stacksize - 1; loop >= 0; loop--){
			// ��� �귯���� ���� ������ �������ش�.
			HPEN pen = ::CreatePen(PS_SOLID, 1, color(loop));
			HPEN oldPen = (HPEN)::SelectObject(ps.hdc, pen);
			HBRUSH MyBrush, OldBrush;
			sprintf_s(buf, "%d", temp.top().grade);
			MyBrush = (HBRUSH)CreateSolidBrush(color(loop));
			OldBrush = (HBRUSH)SelectObject(ps.hdc, MyBrush);
			SetBkColor(ps.hdc, color(loop));
			Rectangle(ps.hdc, BOX_LEFT, top - (GAP*loop), BOX_RIGHT, bottom - (GAP*loop)); // �簢���� �׸�
			SetBkMode(ps.hdc, TRANSPARENT);
			SetTextColor(ps.hdc, RGB(255, 255, 255));
			TextOut(ps.hdc, 200, top + 25 - (GAP*loop), temp.top().name, strlen(temp.top().name)); // Member ����ü �迭�� ����Ѵ�.
			TextOut(ps.hdc, 290, top + 25 - (GAP*loop), buf, strlen(buf)); // Member ����ü �迭�� ����Ѵ�.
			SelectObject(ps.hdc, OldBrush);
			// top�� ����� �� pop�� �Ͽ� ���� ���� top �� �غ� �Ѵ�.
			temp.pop();
			DeleteObject(MyBrush);

			// ���� ��ü�� ������
		}
	}
	
	
	EndPaint(hWnd,&ps); // �׸��� �ڵ尡 ���� ��� �������� �׸��� �ڵ����� �˱� ���ؼ� EndPaint�� ���ݴϴ�.
	return 0;
}


//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����: �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT	- �� â�� �׸��ϴ�.
//  WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//

// ������� �������� �޽������� ó���ϴ� �Լ�
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	
	
	HDC hdc = GetDC(hWnd); // ȭ�� ���� ���� ���ؼ� �ʿ��� dc��
	static HBITMAP hBitMap_Char; // ��Ʈ���� �ҷ����� ���ؼ� �ʿ��� ��Ʈ�� �ڵ� ������ ������ش�.

	static int x;
	static int y;
	switch (message)
	{
	case WM_CREATE: // ���� â�� �ʱ�ȭ �޽���
		hBitMap_Char = (HBITMAP)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP_BOX)); // ������� ��Ʈ�� ���̵� ���� �ݴϴ�.
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// �޴� ������ ���� �м��մϴ�.
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

		// WM_PAINT �� ȭ���� ������ ���¿��� �۵��ϴ� ������ �ٽ� �׷��ִ� ������ �Ѵ�.
	case WM_PAINT:

			// ���� ����� 6�� ���� ������ ���ڸ� �׷��ݴϴ�.
			if (stacksize <= STACK_SIZE){
			OnPaint(hWnd, top, bottom);
			}
			//// ���� ����� 6�̸� �� �̻� �׸� �� ������, ��� �޽����� ����ݴϴ�.
			//if (stacksize == STACK_SIZE){
			//	DialogBox(hInst, MAKEINTRESOURCE(IDD_ALERT), hWnd, About);}
		
		break;
	case WM_GETMINMAXINFO:
	{
		//�ּ� ������ ����
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;  //�������� âũ��
		mmi->ptMinTrackSize.x = 500;
		mmi->ptMinTrackSize.y = 700;
		mmi->ptMaxTrackSize.x = 500;
		mmi->ptMaxTrackSize.y = 700;
		return 0;
	}

	case WM_LBUTTONDOWN:
		
		// Ŭ�� ��ǥ���� �޾ƿɴϴ�.
		x = LOWORD(lParam);
		y = HIWORD(lParam);

		// ���û���� 0�̰� ù ��° �ڽ��� ��ġ�� Ŭ���ϸ� �� �̻� ������ �� ���ٴ� â�� ����ݴϴ�.
		if (stacksize == 0 && BOX_LEFT <x && BOX_RIGHT > x && top< y && bottom> y)
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ALERT1), hWnd, About);
		
		// �� �� �簢�� ������ ��, �簢���� �����.
		if (BOX_LEFT <x && BOX_RIGHT > x && top - (GAP*(stacksize - 1))< y && bottom - (GAP*(stacksize - 1))> y)
		{
			stacksize--;
			mystack.pop();
			InvalidateRect(hWnd, NULL, TRUE);		//����� ����� �ٽñ׸���
		}
		else{
			// �� �ܴ� ���ÿ� ���� �־��ְ� ���� ����� ������ŵ�ϴ�.
			if (stacksize < STACK_SIZE && !(BOX_LEFT <x && BOX_RIGHT > x && top< y && bottom> y)){
				Insert();
				stacksize++;
				InvalidateRect(hWnd, NULL, false);		//����� ���ΰ� �ٽñ׸���
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

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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




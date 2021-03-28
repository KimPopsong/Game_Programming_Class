// d190409_GdiRectStack.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "d190409_GdiRectStack.h"
#include <stack>
#include "NameScoreStack.h"

#define MAX_LOADSTRING 100

const TCHAR* familyNames[] = {L"김", L"이", L"박", L"최", L"강", L"유", L"노",
                              L"정", L"오", L"송", L"서", L"한", L"홍", L"고",
                              L"신", L"조", L"장", L"임"};  //랜덤 성씨
const TCHAR* firstNames[] = {L"주", L"지", L"정", L"수", L"인", L"철",
                             L"현", L"석", L"준", L"은", L"연", L"범",
                             L"동", L"훈", L"영", L"소", L"하", L"호",
                             L"진", L"명", L"윤"};  //랜덤 이름

// Global Variables:
HINSTANCE hInst;                      // current instance
TCHAR szTitle[MAX_LOADSTRING];        // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];  // the main window class name
NameScoreStack nameScoreStack(6);

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPaint(HWND hWnd);
// ClientRect(그리기 영역의 위치와 크기)로부터 i(순서)에 따라 box의 위치와
// 크기를 결정한다
void GetBoxRect(RECT& boxRect, const RECT& clientRect, int i);
// x, y 좌표가 rect 영역 안에 위치하는지 확인한다
bool IsInRect(const RECT& rect, LONG x, LONG y);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // TODO: Place code here.
  srand(unsigned int(time(nullptr)));

  // Initialize global strings
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_D190409GDIRECTSTACK, szWindowClass,
              MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Perform application initialization:
  if (!InitInstance(hInstance, nCmdShow)) {
    return FALSE;
  }

  HACCEL hAccelTable =
      LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D190409GDIRECTSTACK));

  MSG msg;

  // Main message loop:
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D190409GDIRECTSTACK));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_D190409GDIRECTSTACK);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  hInst = hInstance;  // Store instance handle in our global variable

  HWND hWnd =
      CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                    0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

  if (!hWnd) {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  RECT r;
  switch (message) {
    case WM_LBUTTONDOWN: {
      // 클릭 좌표값을 받아옵니다.
      WORD x = LOWORD(lParam);
      WORD y = HIWORD(lParam);
      RECT clientRect;
      GetClientRect(hWnd, &clientRect);
      RECT boxRect;
      GetBoxRect(boxRect, clientRect, int(nameScoreStack.size()) - 1);

      if (nameScoreStack.full() && !IsInRect(boxRect, x, y)) {
        MessageBox(0, L"The stack is full!", L"Full!", MB_OK);
        // 맨 위 사각형 눌렸을 때, 사각형을 지운다.
      } else if (!nameScoreStack.empty() && IsInRect(boxRect, x, y)) {
        delete[] nameScoreStack.top().first;
        nameScoreStack.pop();
        InvalidateRect(hWnd, NULL, true);
        if (nameScoreStack.empty()) {
          MessageBox(0, L"You've removed all boxes!", L"Empty!", MB_OK);
        }
      } else {
        // 무작위 이름 생성 및 스택에 emplace
        TCHAR* tc = new TCHAR[22];
        lstrcpy(tc, familyNames[rand() % std::size(familyNames)]);
        lstrcat(tc, firstNames[rand() % std::size(familyNames)]);
        lstrcat(tc, firstNames[rand() % std::size(familyNames)]);
        nameScoreStack.emplace(std::pair<const TCHAR*, int>(tc, rand() % 101));
        InvalidateRect(hWnd, NULL, false);
      }
    } break;
    case WM_COMMAND: {
      int wmId = LOWORD(wParam);
      // Parse the menu selections:
      switch (wmId) {
        case IDM_ABOUT:
          DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
          break;
        case IDM_EXIT:
          DestroyWindow(hWnd);
          break;
        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
      }
    } break;
    case WM_PAINT: {
      OnPaint(hWnd);
    } break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (message) {
    case WM_INITDIALOG:
      return (INT_PTR)TRUE;

    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
      }
      break;
  }
  return (INT_PTR)FALSE;
}


LRESULT OnPaint(HWND hWnd) {
  PAINTSTRUCT ps;
  BeginPaint(hWnd, &ps);

  RECT clientRect;
  GetClientRect(hWnd, &clientRect);
  LONG boxHeight =
      (clientRect.bottom - clientRect.top) / (2 * nameScoreStack.maxSize());
  LONG gap = boxHeight;

  FillRect(ps.hdc, &clientRect, GetSysColorBrush(COLOR_HIGHLIGHT));

  HPEN hMyRedPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
  HGDIOBJ hOldPen = SelectObject(ps.hdc, hMyRedPen);

  SetTextAlign(ps.hdc, TA_CENTER);

  int i0 = 0;
  nameScoreStack.forEachFromBottom([&](std::pair<const TCHAR*, int>& element) {
    TCHAR scoreStr[9];
    wsprintf(scoreStr, L"%d", element.second);
    RECT boxRect;
    GetBoxRect(boxRect, clientRect, i0);
    LONG textY = (boxRect.top + boxRect.bottom) / 2 - 8;

    SelectObject(ps.hdc, hMyRedPen);
    Rectangle(ps.hdc, boxRect.left, boxRect.top, boxRect.right, boxRect.bottom);
    SelectObject(ps.hdc, hOldPen);
    SetBkMode(ps.hdc, TRANSPARENT);
    TextOut(ps.hdc, boxRect.left + (boxRect.right - boxRect.left) / 3, textY,
            element.first, lstrlen(element.first));
    TextOut(ps.hdc, boxRect.left + (boxRect.right - boxRect.left) * 2 / 3,
            textY, scoreStr, lstrlen(scoreStr));

    ++i0;
  });
  DeleteObject(hMyRedPen);

  EndPaint(hWnd, &ps);

  return 0;
}

// ClientRect(그리기 영역의 위치와 크기)로부터 i(순서)에 따라 box의 위치와 크기를 결정한다
void GetBoxRect(RECT& boxRect, const RECT& clientRect, int i) {
  LONG boxHeight =
      (clientRect.bottom - clientRect.top) / (2 * nameScoreStack.maxSize());
  LONG gap = boxHeight;
  boxRect.left = clientRect.left + 100;
  boxRect.top = clientRect.bottom - (i + 1) * (gap + boxHeight);
  boxRect.right = clientRect.right - 100;
  boxRect.bottom = clientRect.bottom - gap - i * (gap + boxHeight);
}

// x, y 좌표가 rect 영역 안에 위치하는지 확인한다
bool IsInRect(const RECT& rect, LONG x, LONG y) {
  return rect.left <= x && x <= rect.right && rect.top <= y && y <= rect.bottom;
}
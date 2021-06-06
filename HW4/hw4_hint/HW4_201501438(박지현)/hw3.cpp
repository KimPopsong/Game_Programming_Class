#include "hw3.h"
#include <time.h>
#include <string>
#include "SoundManager.h"
#include "Character.h"

/* 현재 윈도우 창의 크기 */
FLOAT winWidth;
FLOAT winHeight;

// 화면보기
SHORT input_str;
bool isShow = false;
bool isAiShow = false;

//CSoundManager* soundManager;

int score = 0;
/* 현재 마우스 위치 좌표 */
D2D_POINT_2F currentMousePosition;
// 라면 메뉴들 위치 
D2D1_RECT_F ramensMenuRectangle[3] = { D2D1::RectF(20.0f, 20.0f, 120.0f, 120.0f), D2D1::RectF(130.0f, 20.0f, 230.0f, 120.0f), D2D1::RectF(240.0f, 20.0f, 340.0f, 120.0f) };
D2D1_RECT_F kettleRectangle = {D2D1::RectF(450 + 170 , 250 + 180, 450 + 270 , 250 + 280)};
D2D1_RECT_F rangeRectangle = { D2D1::RectF(340 ,485, 340+190 , 485 + 70) };
D2D1_RECT_F nambiRectangle = D2D1::RectF(365, 385, 465, 485);
D2D1_RECT_F sonnimRectangle = D2D1::RectF(50, 500, 250, 700);
/* 왼쪽, 오른쪽 마우스 클릭중인지 확인 */
BOOL leftMouseClicking = false;

/* 창, 캐릭터이미지의 크기 */

/* 캐릭터 움직임을 위한 좌표 */
D2D1_POINT_2F move = { 0, 0 };



D2D1_SIZE_F rtSize;
D2D1_SIZE_U ch0Size, ch1Size;
// flag들 
BOOL isSinramen = false; BOOL isYulramen = false; BOOL isChamggaeramen = false; BOOL isKettle = false; BOOL isnambi = false;
BOOL isBurned = false;BOOL isFired = false;BOOL isRiped = false;
BOOL isOrdering = false;

time_t lastFiredTime;
clock_t  lastsonnimComeTime;
bool isMute = false;
int currentsound = 90;



enum ramen{
	Sinramen = 0,
	Yulramen = 1,
	Chamggaeramen = 2,
	nothing = 3
};

enum soundIndex {
	backgroundSound = 0,
	kitchenSound1 = 1,
	kitchenSound2 = 2,
	flySound = 3,
	gasSound = 4,
	fireSound = 5,
	ramenSound = 6,
	waterSound = 7,
	soupeSound = 8
};

std::string getramenString[4] = { "신라면", "열라면", "참깨라면" };
ramen currentramen = nothing;
ramen orderingramen = nothing;

BOOL currentwater = false;

/* 무기 위치 좌표 */
D2D_POINT_2F currentWeaponPosition;

//마우스 커서 위치 확인
using namespace D2D1;

boolean isMouseIn(D2D1_POINT_2F inputMouse, D2D1_RECT_F candidateRect) {
  if (inputMouse.x >= candidateRect.left && inputMouse.x <= candidateRect.right
    && inputMouse.y >= candidateRect.top && inputMouse.y <= candidateRect.bottom)
    return true;
  else
    return false;
}

// mouse cursor position, rectangle rotation & resize
void DemoApp::WriteActionInfo()
{
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	// debug
	RECT rc;
	GetClientRect(m_hwnd, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	WCHAR szText[250];
	swprintf_s(szText, L"마우스x : %1.f\n마우스y : %1.f\n배경사운드 : %d\n",
		currentMousePosition.x, currentMousePosition.y, currentsound);

	m_pRenderTarget->DrawText(
		szText,
		wcslen(szText),
		m_pTextFormat,
		D2D1::RectF(10.0f, 150.0f, 150.0f, 240.0f),
		m_pTextBrush
		);

  swprintf_s(szText, L"점수 : %d", score);

  m_pRenderTarget->DrawText(
    szText,
    wcslen(szText),
    m_score_TextFormat,
    D2D1::RectF(1200.0f, 15.0f, 1450.f, 240.f),
    m_pTextBrush
  );

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void DemoApp::drawKettle() { // 주전자 그리는 코드
  m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Yellow, 1.f));
  m_pRenderTarget->FillGeometry(kettleGeometry, m_pSceneBrush);
  m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
  m_pRenderTarget->DrawGeometry(kettleGeometry, m_pSceneBrush, 1.f);
}

D2D1_RECT_F getRect(int width, int height) {
	return RectF(0, 0, width, height);
}

void DemoApp::soundOn(DemoApp* pDemoApp, int i, bool trueorfalse) {
	if (isMute == false) {
		pDemoApp->soundManager->play(i, trueorfalse);
	}
}
// This method discards device-specific resources if the Direct3D device dissapears during execution and recreates the resources the next time it's invoked.
HRESULT DemoApp::OnRender()
{
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		rtSize = m_pRenderTarget->GetSize();
		ch0Size, ch1Size;

		// Get ready to draw.
		m_pRenderTarget->BeginDraw();

		// Reset to the identity transform.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		// Clear the render target contents.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		//D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
		D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, 100, 100);
		D2D1_RECT_F rcBrushRect2 = RectF(0, 0, 150, 100);
		/* 비트맵 배경 그리기 */
		m_pRenderTarget->FillRectangle(&RectF(0, 0, rtSize.width, rtSize.height), m_pBackgroundBitmapBrush);

		/* 라면 메뉴 그리기*/
		D2D1::Matrix3x2F ramen_sinramen;
		ramen_sinramen = D2D1::Matrix3x2F::Translation(20, 20);
    //ramensMenuRectangle[0] = D2D1::RectF(20.0f,120.0f, 20.0f, 120.0f);

		m_pRenderTarget->SetTransform(ramen_sinramen);
		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pRenderTarget->FillGeometry(m_pRectGeo,
			ramenTypeBrush[0], ramenTypeOpacityBrush[0]);

		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		D2D1::Matrix3x2F ramen_yulramen;
		ramen_yulramen = D2D1::Matrix3x2F::Translation(130, 20);
    //ramensMenuRectangle[1] = D2D1::RectF(130.0f,230.0f, 20.0f, 120.0f);
		m_pRenderTarget->SetTransform(ramen_yulramen);
		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pRenderTarget->FillGeometry(m_pRectGeo,
			ramenTypeBrush[1], ramenTypeOpacityBrush[1]);
		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		D2D1::Matrix3x2F ramen_chamggaeramen;
		ramen_chamggaeramen = D2D1::Matrix3x2F::Translation(240, 20);
    //ramensMenuRectangle[2] = D2D1::RectF(240.0f, 340.0f, 20.0f, 120.0f);

		m_pRenderTarget->SetTransform(ramen_chamggaeramen);
		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pRenderTarget->FillGeometry(m_pRectGeo,
			ramenTypeBrush[2], ramenTypeOpacityBrush[2]);

		m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(170, 450) * D2D1::Matrix3x2F::Scale(2, 1, D2D1::Point2F(0, 0)));
	//가스레인지 그리기
	if (isFired == true) {
		m_pRenderTarget->FillRectangle(&rcBrushRect, firegasrangeBitmapBrush);
	}
	else {
		m_pRenderTarget->FillRectangle(&rcBrushRect, gasrangeBitmapBrush);
	}
	m_pRenderTarget->SetTransform(Matrix3x2F::Identity());
//	ch1Size = m_pCharacter1_Nomal_Bitmap->GetPixelSize();
	
	// 피카츄 손님 그리기
	if (isOrdering == false) {
		lastsonnimComeTime = clock();
		srand(time(NULL));
		int i = rand() % 3;
		orderingramen = ramen(i);
		isOrdering = true;
	}

	else if ((clock() - lastsonnimComeTime) / CLOCKS_PER_SEC < 2) {
		m_pRenderTarget->SetTransform(Matrix3x2F::Translation((double)(clock() - lastsonnimComeTime) / CLOCKS_PER_SEC * 25, 500));
		m_pRenderTarget->FillRectangle(&RectF(0, 0, rtSize.width, rtSize.height), sonnim_BitmapBrush);
	}
	else {
		m_pRenderTarget->SetTransform(Matrix3x2F::Translation(50, 500));
		m_pRenderTarget->FillRectangle(&RectF(0, 0, rtSize.width, rtSize.height), sonnim_BitmapBrush);
		m_pRenderTarget->SetTransform(Matrix3x2F::Translation(50, 450));
		m_pRenderTarget->FillRectangle(&getRect(100, 100), malpungsun_BitmapBrush);
		const char* multiByte = getramenString[orderingramen].c_str();
		TCHAR temp[15]; memset(temp, 0, sizeof(temp));
		MultiByteToWideChar(CP_ACP, MB_COMPOSITE, multiByte, -1, temp, 15);
		//MultiByteToWideChar(CP_ACP, 0, multiByte, -1, temp, 15);
		WCHAR szText[250];
		swprintf_s(szText, L" %s\n 주세요", temp);
		m_pRenderTarget->DrawText(
			szText,
			wcslen(szText),
			m_pTextFormat,
			RectF(20.0f, 30.0f, 100.0f, 100.0f),
			/*D2D1::RectF(50.0f, 450.0f, 150.0f, 550.0f),*/
			m_pTextBrush
		);
	}
	
	// 냄비 그리기
	m_pRenderTarget->SetTransform(Matrix3x2F::Identity());
	m_pRenderTarget->SetTransform(Matrix3x2F::Translation(365, 385));
 
	if (isnambi == true) {
		m_pRenderTarget->SetTransform(Matrix3x2F::Identity());
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(currentMousePosition.x - 50 , currentMousePosition.y - 50 ));
		m_pRenderTarget->FillRectangle(&getRect(130, 100), nambi_BitmapBrush);
	}
	else if (isMouseIn(currentMousePosition, nambiRectangle)) {
		if (currentramen != nothing && currentwater == true && isRiped == true)
			m_pRenderTarget->FillRectangle(&rcBrushRect2, riped_ramen_nambi_BitmapBrush);
		else if (currentramen != nothing && currentwater == true)
			m_pRenderTarget->FillRectangle(&rcBrushRect2, soupe_nambi_BitmapBrush);
		else if (currentwater == true)
			m_pRenderTarget->FillRectangle(&rcBrushRect2, water_nambi_BitmapBrush);
		else if (currentramen != nothing)
			m_pRenderTarget->FillRectangle(&rcBrushRect2, ramen_nambi_BitmapBrush);
		else
		m_pRenderTarget->FillRectangle(&rcBrushRect2, notcover_nambi_BitmapBrush);
	}
	else {
		if (isFired == true && time(NULL) - lastFiredTime >= 13) {
			if (isBurned == false) {
				score = score - 30;
				isBurned = true;
			}
		}
		else if (isFired == true && time(NULL) - lastFiredTime >= 6) {
			isRiped = true;
		}
		if (isBurned == true)
			m_pRenderTarget->FillRectangle(&getRect(130, 100), burned_nambi_BitmapBrush);
		else
			m_pRenderTarget->FillRectangle(&getRect(130, 100), nambi_BitmapBrush);
	}

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // 주전자 그리기
    if (isKettle == true) {
      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(currentMousePosition.x - 200 , currentMousePosition.y - 200));
    }
    else {
      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(450,250));
    }
    drawKettle();

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    // 라면 마우스 경로를 따라서 그리기 
    if (isSinramen == true) {
     m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(currentMousePosition.x, currentMousePosition.y));
      m_pRenderTarget->FillRectangle(&rcBrushRect, ramenTypeBrush[0]);
    }
    else if (isYulramen == true) {
      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(currentMousePosition.x, currentMousePosition.y));
      m_pRenderTarget->FillRectangle(&rcBrushRect, ramenTypeBrush[1]);
    }
    else if (isChamggaeramen == true) {
      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(currentMousePosition.x, currentMousePosition.y));
      m_pRenderTarget->FillRectangle(&rcBrushRect, ramenTypeBrush[2]);
    }
    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		/* 파리 */
		static float float_time = 0.0f;

		float length = m_Animation.GetValue(float_time);


		D2D1_POINT_2F point;
		D2D1_POINT_2F tangent;
		D2D1_MATRIX_3X2_F triangleMatrix;

		hr = fly_PathGeometry->ComputePointAtLength(
			length,
			NULL,
			&point,
			&tangent);

		Assert(SUCCEEDED(hr));


		// 파리 Translation 
		ch0Size = fly_Bitmap->GetPixelSize();
		D2D1::Matrix3x2F tmp = D2D1::Matrix3x2F::Translation(
			point.x - ch0Size.width / 2,
			point.y - ch0Size.height / 2
			);
		//*/
		triangleMatrix = D2D1::Matrix3x2F(
			-tangent.x, -tangent.y,
			tangent.y, -tangent.x,
			point.x - ch0Size.width / 2,
			point.y - ch0Size.height / 2
			);

		/* 파리 그리기 */
		//m_pRenderTarget->SetTransform(triangleMatrix);
		m_pRenderTarget->SetTransform(tmp);
		m_pRenderTarget->FillRectangle(&rcBrushRect, fly_BitmapBrush);

		/* 파리 애니메이션이 끝날때 조건 */
		if (float_time >= m_Animation.GetDuration())
		{
			float_time = 0.0f;
		}
		else
		{
			float_time += static_cast<float>(fly_DwmTimingInfo.rateCompose.uiDenominator) /
				static_cast<float>(fly_DwmTimingInfo.rateCompose.uiNumerator);
		}

		drawAICharacter(isAiShow);
		// 마우스에 대한 디버깅 
	/*	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		D2D1::Matrix3x2F Trans = D2D1::Matrix3x2F::Translation(myCharacter->getDestPositon().x, myCharacter->getDestPositon().y);

*/
		// str 변수
		//TODO
		//LPWSTR str = new TCHAR[100];
		//if (Character::isAttacked == true) {
		//	if ((time(NULL) - Character::lastAttackedTime) <= 5) {
		//		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(1100, 100));
		//		//m_pRenderTarget->DrawTextW(str, wcslen(str), m_pTextFormat, D2D1::RectF(0, 0, 10, 20), m_pTextBrush);
		//		m_pRenderTarget->FillRectangle(&RectF(0, 0, 200, 200), mouseJailBitmapBrush);
		//	}
		//	else {
		//		Character::isAttacked = false;
		//		setRandomSkullPosition();
		//	}
		//}

		/*else {*/
		//	memset(str, 0, sizeof(str));
		//	for (size_t i = 0; i < character.size(); i++)
		//	{
		//		Trans = D2D1::Matrix3x2F::Translation(character[i]->getPosition().x, character[i]->getPosition().y);
		//		m_pRenderTarget->SetTransform(Trans);
		//		switch (character[i]->getColor())
		//		{
		//		case 0://Character.Black:
		//			m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
		//			swprintf(str, L"Stop");
		//			break;
		//		case 1://Character.Red:
		//			m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
		//			swprintf(str, L"attacking");
		//			break;
		//		case 2://Character.Blue:
		//			m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));
		//			swprintf(str, L"following");
		//			break;
		//		case 3: //Character.Green
		//			m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Green));
		//			swprintf(str, L"moving");
		//			break;
		//		default:
		//			m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
		//			break;
		//		}
		//		//Trans = D2D1::Matrix3x2F::Translation(character[i]->getPosition().x - 30, character[i]->getPosition().y - 15);
		//		//m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(0,0));

		//		//m_pRenderTarget->DrawRectangle(D2D1::Rect(-5, -5, 5, 5), m_pSceneBrush);

		//		m_pRenderTarget->DrawTextW(str, wcslen(str), m_pTextFormat, D2D1::RectF(-20, -25, 50, 20), m_pRedBrush);
		//		m_pRenderTarget->FillRectangle(&RectF(0, 0, 30, 20), skullBitmapBrush);
		//		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(character[i]->getPosition().x + 8, character[i]->getPosition().y + 12));


		//		// 적 바깥 주변 원 그리기 
		//		if (isAiShow == true) {
		//			m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));
		//			m_pRenderTarget->DrawGeometry(pViewRange[i], m_pSceneBrush, 0.3f);
		//			m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
		//			m_pRenderTarget->DrawGeometry(pAttackRange[i], m_pSceneBrush, 0.3f);
		//			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		//			swprintf(str, L"인공지능 적에게 attack당하면 적에게 풀리는 순간부터 5초간 마우스를 사용하지 못합니다.");
		//			m_pRenderTarget->DrawTextW(str, wcslen(str), m_pTextFormat, D2D1::RectF(300, 100, 1000, 200), m_pRedBrush);
		//		}

		//	}

		//	for (std::vector<Character*>::iterator iter = character.begin(); iter != character.end(); iter++)
		//	{
		//		Character* ch = *iter;
		//		ch->update(myCharacter, 0.01);
		//	}
		//	myCharacter->update(myCharacter, 0.01);
		/*}*/

		//토글키(t)에 대한 설정 (draw the path in Gray and Dot)
		if (isShow)
		{
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->DrawGeometry(fly_PathGeometry, fly_path_red_Brush, 2, m_pStrokeStyleDash);
		}

		WriteActionInfo(); // Write mouse cursor position, rectangle rotation & resize, etc...

		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}


// Handles window messages.
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));
		//pDemoApp->Soundinit(hwnd);
		result = 1;
	}
	else
	{
		DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

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

			// 윈도우 창 크기 변환 막기
			case WM_GETMINMAXINFO:
			{
				MINMAXINFO* mmi = (MINMAXINFO*)lParam;
				mmi->ptMinTrackSize.x = 1400;
				mmi->ptMinTrackSize.y = 800;
				mmi->ptMaxTrackSize.x = 1400;
				mmi->ptMaxTrackSize.y = 800;
			}
			break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}
			result = 0;
			wasHandled = true;
			break;

      case WM_LBUTTONUP: 
      {
		  if (Character::isAttacked == true)
			  break;
		  if ((isSinramen || isYulramen || isChamggaeramen) && isMouseIn(currentMousePosition, nambiRectangle) == true) {
			  if (isSinramen == true)
				  currentramen = Sinramen;
			  else if (isYulramen == true)
				  currentramen = Yulramen;
			  else if (isChamggaeramen == true)
				  currentramen = Chamggaeramen;
			  pDemoApp->soundOn(pDemoApp, ramenSound, FALSE);
			  pDemoApp->soundOn(pDemoApp, soupeSound, FALSE);
			  
		  }
		  else if (isKettle && isMouseIn(currentMousePosition, nambiRectangle) == true) {
			  currentwater = true;
			  //pDemoApp->soundManager->play(waterSound, FALSE);
			  pDemoApp->soundOn(pDemoApp, waterSound, FALSE);
		  }
		  else if (isnambi && isMouseIn(currentMousePosition, sonnimRectangle) == true) {
			  if (currentwater == true && currentramen == orderingramen && isBurned == false && isRiped == true) {
				  score += 100;
			  }
			  else if (currentwater == true && currentramen != orderingramen && isBurned == false && isRiped == true) {
				  score += 50;
			  }
			  else if (currentwater == true && currentramen == orderingramen && isRiped == false)
				  score += 30;

			  currentramen = nothing; orderingramen = nothing; currentwater = false;
			  isBurned = false; isRiped = false; isOrdering = false;
			  lastFiredTime = time(NULL);
		  }

		  isSinramen = false; isYulramen = false; isChamggaeramen = false; 
          isKettle = false; isnambi = false;
          InvalidateRect(hwnd, NULL, FALSE);
          break;
      }
      case WM_LBUTTONDOWN:
      {
		  if (Character::isAttacked == true)
			  break;
          currentMousePosition.x = LOWORD(lParam);
          currentMousePosition.y = HIWORD(lParam);
  
          if (isMouseIn(currentMousePosition, ramensMenuRectangle[0])== true) {
            isSinramen = true;
          }

          else if (isMouseIn(currentMousePosition, ramensMenuRectangle[1]) == true) {
            isYulramen = true;
          }
        
          else if (isMouseIn(currentMousePosition, ramensMenuRectangle[2])== true) {
            isChamggaeramen = true;
          }
          else if (isMouseIn(currentMousePosition, kettleRectangle) == true) {
            isKettle = true;
          }
		  else if (isMouseIn(currentMousePosition, nambiRectangle) == true) {
			  isnambi = true;
		  }
		  else if (isMouseIn(currentMousePosition, rangeRectangle) == true) {
			  isFired = !isFired;
			  if (isFired == true) {
				  lastFiredTime = time(NULL);
				/*  pDemoApp->soundManager->play(fireSound, FALSE);
				  pDemoApp->soundManager->play(gasSound, TRUE);*/
				  pDemoApp->soundOn(pDemoApp, fireSound, FALSE);
				  pDemoApp->soundOn(pDemoApp, gasSound, TRUE);
			  }
			  else {
				  pDemoApp->soundManager->stop(gasSound);
			  }
		  }

          InvalidateRect(hwnd, NULL, FALSE);
          break;
      }
			case WM_MOUSEMOVE:
			{
				currentMousePosition.x = LOWORD(lParam);
				currentMousePosition.y = HIWORD(lParam);
				pDemoApp->myCharacter->setDestPosition(currentMousePosition);
				InvalidateRect(hwnd, NULL, FALSE);
			    break;
			}
			

			case WM_PAINT:
			{
				pDemoApp->OnRender();

				// Do not call ValidateRect so that the window will be redrawn.
				//ValidateRect(hwnd, NULL);
			}
			result = 0;
			wasHandled = true;
			break;

			/* 토글키(t)와 스페이스를 위한 설정들 */
			case WM_KEYDOWN:
			{
				input_str = static_cast<SHORT>(wParam);
				switch (input_str)
				{

				case 't':
				case 'T':
					isShow = !isShow;
					break;

				case 's':
				case 'S': 
					{
					isMute = !isMute;
					if (isMute == true) {
						for (int i = 0; i <= 8; i++) {
							pDemoApp->soundManager->stop(i);
						}
					}
					else {
						pDemoApp->soundManager->play(backgroundSound, true); pDemoApp->soundManager->play(kitchenSound1, true);
						pDemoApp->soundManager->play(kitchenSound2, true); pDemoApp->soundManager->play(flySound, true);
					}
					break;
					}

				case 'i':
				case 'I':
					isAiShow = !isAiShow;
					break;
				case VK_LEFT: { // 좌우키로 효과음 볼륨 조정
					
				}
				case VK_RIGHT: {
					
				}
				case VK_UP: { // 위아래키로 배경음 볼륨 조정
					if (currentsound != 100)
						currentsound = currentsound + 5;
					pDemoApp->SetVolume(pDemoApp->soundManager->pDSBPrimary, currentsound);
					//pDemoApp->soundManager->pDSBPrimary->SetVolume(DSVOLUME_TO_DB(100000));
					break;
				}
				case VK_DOWN: {
					if (currentsound != 0)
						currentsound -= 5;
					pDemoApp->SetVolume(pDemoApp->soundManager->pDSBPrimary, currentsound);
					//pDemoApp->soundManager->pDSBPrimary->SetVolume(DSVOLUME_TO_DB(0));
					break;
				}
				}
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_KEYUP:
			{
			}
			result = 0;
			wasHandled = true;
			break;

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
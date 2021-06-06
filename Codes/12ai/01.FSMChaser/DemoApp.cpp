#include "d2dHelper.h"
#include "Character.h"
#include <vector>

/*
**추격자 게임을 FSM 구현해보기
○ 플레이어(1기)와 적 추격자(5기)의 행위를 FSM으로 구현한다.
○ 플레이어의 행위: 플레이어는 정지해 있다가 마우스를 클릭하면 그 위치로 이동한다.
- 이동 속도는 추격자의 이동 속도보다 약간 빠르게 설정하자.
○ 5개의 추격자의 행위: 각 추격자는 FSM에 따라서 행동하도록 하자.
- 초기 위치는 랜덤하게 배치하자.
- 정지한 상태, 움직이는 상태, 추격 상태, 공격 상태, 도망 상태를 구현하자.
○ 표시: 플레이어를 검정색 채워진 원으로 표시하자. 또한 원의 상단에는 "player"라는 텍스트를 표시하자.
○ 표시: 각 추격자를 상태에 따라서 다른 특정 색의 작은 채워진 원으로 표시하자. 또한 추격자 원의 상단에는 현재 상태를 텍스트로 표시하자.
- <상태: 채워진 원의 색, 문자열>: <정지 상태: 검정, stop>, <이동 상태: 초록, "moving">, <추격 상태: 파랑, "following">, <공격 상태: 빨강, "attacking">
○ 각 추격자의 가시 영역과 공격가능 영역을 원으로 표시하자.
- 동그라미 색: 적의 시야: 파랑, 공격 가능 거리: 빨강. (플레이어가 적의 시야에 들어가면 적이 쫓아옴. 공격 가능 거리에 들어가면 적이 공격함.)
*/


#define NUM_ENEMIES		5

HWND hWnd = NULL;
D2D1_SIZE_U size;

extern ID2D1Factory* pFactory;
extern ID2D1HwndRenderTarget* pRenderTarget;
extern ID2D1SolidColorBrush* pRedBrush;
extern ID2D1SolidColorBrush* pBlackBrush;
extern ID2D1SolidColorBrush* pBlueBrush;
extern ID2D1SolidColorBrush* pGreenBrush;
extern ID2D1SolidColorBrush* pSolidColorBrush;
extern ID2D1PathGeometry* pCharacter;
extern IDWriteFactory* pWriteFactory;
extern IDWriteTextFormat* pTextFormat;

extern ID2D1PathGeometry* pEnemy[5];
extern ID2D1PathGeometry* pViewRange[5];
extern ID2D1PathGeometry* pAttackRange[5];

std::vector<Character*> enemyCharacters;
Character* myCharacter = NULL;


HRESULT Init();
bool Render(float);
void OnPaint();
bool updateFSM(float timeDelta);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void initFSM();
void releaseFSM();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszStr, int nCmd)
{
	size.width = 640;
	size.height = 480;
	
	hWnd = d2d::initWin32(hInstance,size.width,size.height);
	if (SUCCEEDED(Init()))
	{
		d2d::doMessageLoop(Render);
	}

	d2d::releaseWin32(hInstance);
	releaseFSM();
	d2d::DiscardDeviceResources();
	d2d::DiscardDeviceIndependentResources();

	return 0;
}

HRESULT Init()
{
	srand(GetTickCount());

	HRESULT hr;
	hr = d2d::CreateDeviceIndependentResources();
	if(SUCCEEDED(hr))
	{
		hr = d2d::CreateDeviceResources(hWnd, size);
	}

	initFSM();

	return hr;
}

bool Render(float dt)
{
	OnPaint();
	updateFSM(dt);
	return true;
}

void OnPaint()
{
	HRESULT hr;
	LPWSTR str = new TCHAR[100];
	memset(str,NULL,sizeof(str));

	d2d::CreateDeviceResources(hWnd, size);

	pRenderTarget->BeginDraw();
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	//Draw Player
	D2D1::Matrix3x2F Trans = D2D1::Matrix3x2F::Translation(myCharacter->getPosition().x,myCharacter->getPosition().y);
	pRenderTarget->SetTransform(Trans);
	pRenderTarget->FillGeometry(pCharacter,pBlackBrush);
	swprintf(str,L"player");
	pRenderTarget->DrawTextW(str,wcslen(str),pTextFormat,D2D1::RectF(-20,-25,50,20),pRedBrush);
	
	for(size_t i = 0; i < enemyCharacters.size() ; i++)
	{
		Trans = D2D1::Matrix3x2F::Translation(enemyCharacters[i]->getPosition().x,enemyCharacters[i]->getPosition().y);
		pRenderTarget->SetTransform(Trans);
		switch(enemyCharacters[i]->getColor())
		{
		case 0://Character.Black:
			pSolidColorBrush = pBlackBrush;
			swprintf(str,L"Stop");
			break;
		case 1://Character.Red:
			pSolidColorBrush = pRedBrush;
			swprintf(str,L"attacking");
			break;
		case 2://Character.Blue:
			pSolidColorBrush = pBlueBrush;
			swprintf(str,L"following");
			break;
		case 3: //Character.Green
			pSolidColorBrush = pGreenBrush;
			swprintf(str,L"moving");
			break;
		default:
			pSolidColorBrush = pBlackBrush;
			break;
		}
		pRenderTarget->FillGeometry(pEnemy[i],pSolidColorBrush);
		pRenderTarget->DrawGeometry(pViewRange[i],pBlueBrush,0.3f);
		pRenderTarget->DrawGeometry(pAttackRange[i],pRedBrush,0.3f);
		pRenderTarget->DrawTextW(str,wcslen(str),pTextFormat,D2D1::RectF(-20,-25,50,20),pRedBrush);
	}

	swprintf(str,L"x: %.0f, y: %.0f\n",myCharacter->getPosition().x,myCharacter->getPosition().y);
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	pRenderTarget->DrawTextW(str,wcslen(str),pTextFormat,D2D1::RectF(20,20,640,440),pRedBrush);

	hr = pRenderTarget->EndDraw();
}

bool intersectPlane(int sx, int sy);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) 
		{
			::DestroyWindow(hWnd);
		}
		break;

	case WM_LBUTTONDOWN:
		if (intersectPlane(LOWORD(lParam),HIWORD(lParam)))
		{
		}
		break;

	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool updateFSM(float timeDelta)
{
	for (std::vector<Character*>::iterator iter = enemyCharacters.begin(); iter != enemyCharacters.end(); iter++ )	
	{
		Character* ch = *iter;
		ch->update(myCharacter, timeDelta);
	}
	// 내 위치를 갱신함.
	myCharacter->update(myCharacter, timeDelta);

	return true;
}

void initFSM()
{
	// 초기 위치들을 중복되지 않게 정한다.
	int posx[NUM_ENEMIES],posy[NUM_ENEMIES],cx,cy;
	int k=0;
	bool duplicated;
	for (int i=0; i<NUM_ENEMIES; i++) 
	{
		do {
			cx = rand() % (2*PLANE_SIZE_X) - PLANE_SIZE_X;
			cy = rand() % (2*PLANE_SIZE_Y) - PLANE_SIZE_Y;
			duplicated = false;
			if(cx <0 || cy <0 || cx >640 ||cy>480)
				duplicated = true;
			if (i != 0) 
			{
				for (int j=0; j<i; j++) 
				{
					if (sqrt((float)(cx-posx[i])*(cx-posx[i]) + (float)(cy-posy[i])*(cy-posy[i])) < 10) 
					{
						duplicated = true;
						break;
					}
				}
			}
		} while (duplicated);
		posx[k] = cx; posy[k] = cy; k++;
	}

	// 적군 캐릭터들을 생성함.
	for (int i=0; i<NUM_ENEMIES; i++) 
	{
		DWORD enemyStateTransitions[][3] = {
			{ Character::STATE_STAND, Character::EVENT_FINDTARGET, Character::STATE_FOLLOW },
			{ Character::STATE_STAND, Character::EVENT_BEATTACKED, Character::STATE_ATTACK },
			{ Character::STATE_STAND, Character::EVENT_DUBIOUS, Character::STATE_MOVE },
			{ Character::STATE_MOVE, Character::EVENT_FINDTARGET, Character::STATE_FOLLOW },
			{ Character::STATE_MOVE, Character::EVENT_STOPWALK, Character::STATE_STAND },
			{ Character::STATE_ATTACK, Character::EVENT_LOSTTARGET, Character::STATE_STAND },
			{ Character::STATE_ATTACK, Character::EVENT_HEALTHDRAINED, Character::STATE_RUNAWAY },
			{ Character::STATE_ATTACK, Character::EVENT_OUTOFATTACK, Character::STATE_FOLLOW },
			{ Character::STATE_FOLLOW, Character::EVENT_WITHINATTACK, Character::STATE_ATTACK },
			{ Character::STATE_FOLLOW, Character::EVENT_LOSTTARGET, Character::STATE_STAND },
			{ Character::STATE_RUNAWAY, Character::EVENT_LOSTTARGET, Character::STATE_STAND },
		};

		Character* enemy = new Character(Character::TYPE_AI, enemyStateTransitions, 11);
		enemy->setPosition(D2D1::Point2F((float)posx[i], (float)posy[i]));
		enemyCharacters.push_back(enemy);
	}

	// 주인공 캐릭터를 생성함.
	{
		DWORD myStateTransitions[][3] = 
		{
			{ Character::STATE_STAND, Character::EVENT_FINDTARGET, Character::STATE_MOVE },
			{ Character::STATE_MOVE, Character::EVENT_STOPWALK, Character::STATE_STAND },
		};

		myCharacter = new Character(Character::TYPE_HUMAN, myStateTransitions, 2);
		myCharacter->setPosition(D2D1::Point2F(320, 240)); //position
		myCharacter->setDestPosition(D2D1::Point2F(320, 240)); //dest
	}
}

void releaseFSM()
{
	for (std::vector <Character*>::iterator iter = enemyCharacters.begin(); iter != enemyCharacters.end(); iter++) 
	{
		Character* ch = *iter;
		SAFE_DELETE(ch);
	}
	enemyCharacters.clear();
	SAFE_DELETE(myCharacter);
}

bool intersectPlane(int sx, int sy)
{
	D2D1_POINT_2F picked = D2D1::Point2F((float)sx,(float)sy);
	myCharacter->setDestPosition(picked);
	return true;
}

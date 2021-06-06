#include "d2dHelper.h"
#include "Character.h"
#include <vector>

/*
**�߰��� ������ FSM �����غ���
�� �÷��̾�(1��)�� �� �߰���(5��)�� ������ FSM���� �����Ѵ�.
�� �÷��̾��� ����: �÷��̾�� ������ �ִٰ� ���콺�� Ŭ���ϸ� �� ��ġ�� �̵��Ѵ�.
- �̵� �ӵ��� �߰����� �̵� �ӵ����� �ణ ������ ��������.
�� 5���� �߰����� ����: �� �߰��ڴ� FSM�� ���� �ൿ�ϵ��� ����.
- �ʱ� ��ġ�� �����ϰ� ��ġ����.
- ������ ����, �����̴� ����, �߰� ����, ���� ����, ���� ���¸� ��������.
�� ǥ��: �÷��̾ ������ ä���� ������ ǥ������. ���� ���� ��ܿ��� "player"��� �ؽ�Ʈ�� ǥ������.
�� ǥ��: �� �߰��ڸ� ���¿� ���� �ٸ� Ư�� ���� ���� ä���� ������ ǥ������. ���� �߰��� ���� ��ܿ��� ���� ���¸� �ؽ�Ʈ�� ǥ������.
- <����: ä���� ���� ��, ���ڿ�>: <���� ����: ����, stop>, <�̵� ����: �ʷ�, "moving">, <�߰� ����: �Ķ�, "following">, <���� ����: ����, "attacking">
�� �� �߰����� ���� ������ ���ݰ��� ������ ������ ǥ������.
- ���׶�� ��: ���� �þ�: �Ķ�, ���� ���� �Ÿ�: ����. (�÷��̾ ���� �þ߿� ���� ���� �Ѿƿ�. ���� ���� �Ÿ��� ���� ���� ������.)
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
	// �� ��ġ�� ������.
	myCharacter->update(myCharacter, timeDelta);

	return true;
}

void initFSM()
{
	// �ʱ� ��ġ���� �ߺ����� �ʰ� ���Ѵ�.
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

	// ���� ĳ���͵��� ������.
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

	// ���ΰ� ĳ���͸� ������.
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

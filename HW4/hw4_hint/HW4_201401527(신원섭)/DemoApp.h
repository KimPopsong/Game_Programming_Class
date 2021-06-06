#pragma once

// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
// DX Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <Dwmapi.h>
#include "InputKeyboard.h"
#include <time.h>
#include "Animation.h"
#include "SoundManager.h"
#include <queue>

// 자원 안전 반환 매크로.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// 현재 모듈의 시작주소 얻기.
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif
#define STARCOUNT 1
class DemoApp
{
public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize();
	void RunMessageLoop();
	void CharacterJump(float jump_v);				//주인공 점프함수, jump_v는 초기속도 설정
	bool keyrender();								//비동기식 처리 Dirext2d input 
	void StarFallMove(float star_v, int i);


private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//HRESULT CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, __deref_out ID2D1BitmapBrush** ppBitmapBrush);

	HRESULT LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, __deref_out ID2D1Bitmap** ppBitmap);
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, __deref_out ID2D1Bitmap** ppBitmap);
	BOOL InitSound(HWND hWnd);
	void funcCollisionCheck();

	void funcMovingStar(float _t);
	void funcStarCollisionCheck();

	struct Bullet
	{
		int x;			//총알 x좌표
		int y;			//총알 y좌표
		int speed;		//총알 스피드
		int era;		//총알 삭제 여부(윈도우 크기에서 벗어나거나,충돌)
		int mass;
	};

private:
	HWND m_hwnd;
	ID2D1Factory* m_pD2DFactory;
	IWICImagingFactory* m_pWICFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1Bitmap* m_pBitmap;
	ID2D1Bitmap* m_pCrowBitmap;
	ID2D1Bitmap* m_pCrowBitmap2;
	ID2D1Bitmap* m_pTreeBitmap;
	ID2D1Bitmap* m_pEnemyBitmap;
	ID2D1Bitmap* m_pHeroBitmap[10];
	ID2D1Bitmap* m_pCrowMotionBitmap[6];
	ID2D1Bitmap* m_pBitmapEnd;
	ID2D1Bitmap* m_pBitmapWin;
	ID2D1Bitmap* m_pLFireGun;
	ID2D1Bitmap* m_pRFireGun;
	ID2D1Bitmap* m_pLBullet;
	ID2D1Bitmap* m_pRBullet;
	ID2D1Bitmap* m_pTallSton;
	ID2D1Bitmap* m_pSton;


	void CreateBullet();							//총알 생성
	void RightDrawBullet(struct Bullet *temp);		//오른쪽 총알 드로잉
	void LeftDrawBullet(struct Bullet *temp);		//왼쪽 총알 드로잉
	void BulletCollision(struct Bullet *temp);
	std::queue <Bullet> bullet_R;					//오른쪽 총알 큐
	std::queue <Bullet> bullet_L;					//왼쪽 총알 큐

	ID2D1StrokeStyle *m_pStrokeStyleDash;
	//브러쉬
	ID2D1SolidColorBrush* m_pSceneBrush;
	ID2D1SolidColorBrush* m_pStarBrush;
	ID2D1SolidColorBrush* m_pBossHpBrush;

	ID2D1RadialGradientBrush* m_pRadialGradientBrush;
	ID2D1LinearGradientBrush* m_pLinearStarGradientBrush;

	//비트맵
	ID2D1BitmapBrush *m_pCrow_Nomal_BitmapBrush;
	ID2D1BitmapBrush *m_pCrow_Nomal_Bitmap2Brush;
	ID2D1BitmapBrush *m_pTree_Nomal_BitmapBrush;
	ID2D1BitmapBrush *m_pEnemy_Nomal_BitmapBrush;
	ID2D1BitmapBrush *m_pHero_Nomal_BitmapBrush[10];
	ID2D1BitmapBrush *m_pCrowMotion_Nomal_BitmapBrush[6];
	ID2D1BitmapBrush *m_pBitmapEnd_BitmapBrush;
	ID2D1BitmapBrush *m_pBitmapWin_BitmapBrush;
	ID2D1BitmapBrush *m_pLFireGun_BitmapBrush;
	ID2D1BitmapBrush *m_pRFireGun_BitmapBrush;
	ID2D1BitmapBrush *m_pLBullet_BitmapBrush;
	ID2D1BitmapBrush *m_pRBullet_BitmapBrush;
	ID2D1BitmapBrush *m_pTallSton_BitmapBrush;
	ID2D1BitmapBrush *m_pSton_BitmapBrush;



	//경로기하
	ID2D1PathGeometry *m_pCrow_PathGeometry; //까마귀
	ID2D1PathGeometry *m_pMoonGeometry; //달
	ID2D1PathGeometry *m_pStarGeometry[STARCOUNT];
	ID2D1PathGeometry *m_pStarFallGeometry;
	ID2D1PathGeometry *m_pStarFallFireGeometry[STARCOUNT];



	// Animation
	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;
	CrowAnimation<float> m_Animation;



	//DWrite
	IDWriteFactory *m_pDWriteFactory;
	IDWriteTextFormat *m_pTextFormat;

};

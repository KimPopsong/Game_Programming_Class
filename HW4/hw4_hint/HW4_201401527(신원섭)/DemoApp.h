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

// �ڿ� ���� ��ȯ ��ũ��.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// ���� ����� �����ּ� ���.
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
	void CharacterJump(float jump_v);				//���ΰ� �����Լ�, jump_v�� �ʱ�ӵ� ����
	bool keyrender();								//�񵿱�� ó�� Dirext2d input 
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
		int x;			//�Ѿ� x��ǥ
		int y;			//�Ѿ� y��ǥ
		int speed;		//�Ѿ� ���ǵ�
		int era;		//�Ѿ� ���� ����(������ ũ�⿡�� ����ų�,�浹)
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


	void CreateBullet();							//�Ѿ� ����
	void RightDrawBullet(struct Bullet *temp);		//������ �Ѿ� �����
	void LeftDrawBullet(struct Bullet *temp);		//���� �Ѿ� �����
	void BulletCollision(struct Bullet *temp);
	std::queue <Bullet> bullet_R;					//������ �Ѿ� ť
	std::queue <Bullet> bullet_L;					//���� �Ѿ� ť

	ID2D1StrokeStyle *m_pStrokeStyleDash;
	//�귯��
	ID2D1SolidColorBrush* m_pSceneBrush;
	ID2D1SolidColorBrush* m_pStarBrush;
	ID2D1SolidColorBrush* m_pBossHpBrush;

	ID2D1RadialGradientBrush* m_pRadialGradientBrush;
	ID2D1LinearGradientBrush* m_pLinearStarGradientBrush;

	//��Ʈ��
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



	//��α���
	ID2D1PathGeometry *m_pCrow_PathGeometry; //���
	ID2D1PathGeometry *m_pMoonGeometry; //��
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

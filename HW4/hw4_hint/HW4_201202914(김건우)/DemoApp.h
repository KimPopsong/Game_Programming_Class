#pragma once

#include "pch.h"
#include "Animation.h"

class DemoApp {
  enum class MissileState {
    READY, FLYING, HIT
  };

  enum class SeagullState {
    IN_PEACE, AWARE, EVACUATING, LEAVING, GONE
  };

  enum class SeagullEvent {
    MISSILE_LAUNCHED, MISSILE_NEAR, MISSILE_MISSED, ROCK_DESTROYED, FAR_AWAY
  };

public:
  DemoApp();
  ~DemoApp();
  HRESULT Initialize();
  void RunMessageLoop();

  enum class GameState {
    PLAYING, SUCCEESS
  };

private:
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  HRESULT CreateDeviceIndependentResources();
  HRESULT CreateDeviceResources();
  void DiscardDeviceResources();
  HRESULT OnRender();
  void OnResize(UINT width, UINT height);
  HRESULT LoadWave(LPWSTR filename, IXAudio2SourceVoice** ppsv, XAUDIO2_BUFFER* buf);

  void IssueSeagullEvent(SeagullEvent ev);

  float GetVolumeLinear() const;

private:
  HWND m_hwnd;

  ID2D1Factory* m_pD2DFactory;
  IWICImagingFactory* m_pWICFactory;
  IDWriteFactory *m_pDWriteFactory;
  ID2D1HwndRenderTarget* m_pRenderTarget;

  IDWriteTextFormat *m_pTextFormat;

  ID2D1PathGeometry* m_pRockAnimationGeometry;
  ID2D1PathGeometry* m_pMissileDrawingGeometry;
  ID2D1PathGeometry* m_pMissileAnimationGeometry;
  ID2D1RectangleGeometry* m_pCometClipGeometry;

  ID2D1Bitmap* m_pSpaceBitmap;
  ID2D1Bitmap* m_pRockBitmap;
  ID2D1Bitmap* m_pShooterBitmap;
  ID2D1Bitmap* m_pCometBitmap;
  ID2D1Bitmap* m_pSeagullInPeaceBitmap;
  ID2D1Bitmap* m_pSeagullAwareBitmap;
  ID2D1Bitmap* m_pSeagullFlyingBitmap;

  SeagullState m_SeagullState;
  ID2D1Bitmap*const* m_ppSeagullBitmap;
  LPCWSTR m_pSeagullStateWStr;
  D2D1_POINT_2F m_SeagullStartCoord;

  ID2D1SolidColorBrush* m_pRedBrush;
  ID2D1SolidColorBrush* m_pYellowBrush;
  ID2D1SolidColorBrush* m_pDarkBlueBrush;
  ID2D1BitmapBrush* m_pCometBitmapBrush;
  ID2D1RadialGradientBrush* m_pRadialOpacityBrush;

  AnimationLinear<float> m_RockAnimation;
  AnimationAccel<float> m_MissileAnimation;

  IXAudio2* m_pXAudio2;
  IXAudio2MasteringVoice* m_pMasterVoice;

  IXAudio2SourceVoice* m_pHitVoice;
  XAUDIO2_BUFFER m_HitBuffer;
  IXAudio2SourceVoice* m_pMissileReadyVoice;
  XAUDIO2_BUFFER m_MissileReadyBuffer;
  IXAudio2SourceVoice* m_pMissileNotReadyVoice;
  XAUDIO2_BUFFER m_MissileNotReadyBuffer;
  IXAudio2SourceVoice* m_pMissileLaunchVoice;
  XAUDIO2_BUFFER m_MissileLaunchBuffer;

  LARGE_INTEGER m_PrevTime;
  LARGE_INTEGER m_Frequency;

  D2D1_POINT_2F m_MouseMove;
  D2D1_POINT_2F m_LButtonUp;

  MissileState m_MissileState;

  GameState m_GameState;

  int m_VolumeLog;
  bool m_IsMuted;
};

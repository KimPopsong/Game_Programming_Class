#pragma once

#include <d2d1.h>
#include <wincodec.h>

HRESULT LoadBitmapFromFile(
  ID2D1RenderTarget* pRenderTarget,
  IWICImagingFactory* pIWICFactory,
  PCWSTR uri,
  UINT destinationWidth,
  UINT destinationHeight,
  ID2D1Bitmap** ppBitmap
);

HRESULT LoadBitmapFromResource(
  ID2D1RenderTarget* pRenderTarget,
  IWICImagingFactory* pIWICFactory,
  PCWSTR resourceName,
  PCWSTR resourceType,
  UINT destinationWidth,
  UINT destinationHeight,
  ID2D1Bitmap** ppBitmap
);
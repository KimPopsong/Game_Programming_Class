#include "hw3.h"

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// Create a Direct2D render target.
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
		);

		if (SUCCEEDED(hr))
		{
			// Create a solid color brush for writing text.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pTextBrush);
		}

		if (SUCCEEDED(hr))
		{
			// Create a scene brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black, 1.f),
				&m_pSceneBrush);
		}
		if (SUCCEEDED(hr))
		{
			// Create a gray brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Red),
				&fly_path_red_Brush);
		}
		if (SUCCEEDED(hr))
		{
			// Create a Red brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Red),
				&m_pRedBrush);
		}

		/* 배경 및 물체의 비트맵생성 */
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"BACKGROUND",
				L"IMAGE",
				size.width,
				size.height,
				&m_pBackgroundBitmap
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"SINRAMEN",
				L"IMAGE",
				100,
				100,
				&ramenTypeBitmap[0]
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"YULRAMEN",
				L"IMAGE",
				100,
				100,
				&ramenTypeBitmap[1]
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"CHAMGGAERAMEN",
				L"IMAGE",
				100,
				100,
				&ramenTypeBitmap[2]
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"NUGURI",
				L"IMAGE",
				100,
				100,
				&ramenTypeBitmap[3]
			);
		}

    if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"NUGURI",
				L"IMAGE",
				100,
				100,
				&ramenTypeBitmap[3]
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"FLY",
				L"IMAGE",
				0,
				0,
				&fly_Bitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"NAMBI",
				L"IMAGE",
				130,
				100,
				&nambi_Bitmap
			);
		}
		if (SUCCEEDED(hr))    
		{
			hr = LoadResourceBitmap(
			m_pRenderTarget,
			m_pWICFactory,
			L"NOTCOVERNAMBI",
			L"IMAGE",
			150,
			100,
			&notcover_nambi_Bitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"SOUPENAMBI",
				L"IMAGE",
				150,
				100,
				&soupe_nambi_Bitmap
			);
		}
		
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"WATERNAMBI",
				L"IMAGE",
				150,
				100,
				&water_nambi_Bitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"RAMENNAMBI",
				L"IMAGE",
				150,
				100,
				&ramen_nambi_Bitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"BURNEDNAMBI",
				L"IMAGE",
				130,
				100,
				&burned_nambi_Bitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"RIPEDRAMENNAMBI",
				L"IMAGE",
				150,
				100,
				&riped_ramen_nambi_Bitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"SONNIM",
				L"IMAGE",
				200,
				200,
				&sonnim_Bitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"MALPUNGSUN",
				L"IMAGE",
				100,
				100,
				&malpungsun_Bitmap
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"GASRANGE",
				L"IMAGE",
				100,
				100,
				&gasrangeBitmap
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"FIREGASRANGE",
				L"IMAGE",
				100,
				100,
				&firegasrangeBitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"SKULL",
				L"IMAGE",
				20,
				20,
				&skullBitmap
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(
				m_pRenderTarget,
				m_pWICFactory,
				L"MOUSEJAIL",
				L"IMAGE",
				200,
				200,
				&mouseJailBitmap
			);
		}
		/* 배경 및 물체의 Brush 생성 */
		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_EXTEND_MODE_CLAMP,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
			);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					m_pBackgroundBitmap,
					propertiesXClampYClamp,
					&m_pBackgroundBitmapBrush
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					ramenTypeBitmap[0],
					&ramenTypeBrush[0]
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					ramenTypeBitmap[1],
					&ramenTypeBrush[1]
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					ramenTypeBitmap[2],
					&ramenTypeBrush[2]
				);
			}
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					ramenTypeBitmap[3],
					&ramenTypeBrush[3]
				);
			}
		  if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateBitmapBrush(
				nambi_Bitmap, propertiesXClampYClamp,
				&nambi_BitmapBrush);
		  }
		  if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateBitmapBrush(
			  notcover_nambi_Bitmap, propertiesXClampYClamp,
			  &notcover_nambi_BitmapBrush);
		  }
		  if (SUCCEEDED(hr)) {
			  hr = m_pRenderTarget->CreateBitmapBrush(
				  soupe_nambi_Bitmap, propertiesXClampYClamp,
				  &soupe_nambi_BitmapBrush);
		  }
		  if (SUCCEEDED(hr)) {
			  hr = m_pRenderTarget->CreateBitmapBrush(
				  water_nambi_Bitmap, propertiesXClampYClamp,
				  &water_nambi_BitmapBrush);
		  }
		  if (SUCCEEDED(hr)) {
			  hr = m_pRenderTarget->CreateBitmapBrush(
				  ramen_nambi_Bitmap, propertiesXClampYClamp,
				  &ramen_nambi_BitmapBrush);
		  }
		  if (SUCCEEDED(hr)) {
			  hr = m_pRenderTarget->CreateBitmapBrush(
				  burned_nambi_Bitmap, propertiesXClampYClamp,
				  &burned_nambi_BitmapBrush);
		  }
		  if (SUCCEEDED(hr)) {
			  hr = m_pRenderTarget->CreateBitmapBrush(
				  riped_ramen_nambi_Bitmap, propertiesXClampYClamp,
				  &riped_ramen_nambi_BitmapBrush);
		  }
		  if (SUCCEEDED(hr)) {
			  hr = m_pRenderTarget->CreateBitmapBrush(
				  skullBitmap, propertiesXClampYClamp,
				  &skullBitmapBrush);
		  }
		  if (SUCCEEDED(hr)) {
			  hr = m_pRenderTarget->CreateBitmapBrush(
				  mouseJailBitmap, propertiesXClampYClamp,
				  &mouseJailBitmapBrush);
		  }
			ID2D1GradientStopCollection* pGradientStops = NULL;

			static const D2D1_GRADIENT_STOP gradientStops[] =
			{
				{   0.f,  D2D1::ColorF(D2D1::ColorF::Black, 1.0f)  },
				{   1.f,  D2D1::ColorF(D2D1::ColorF::White, 0.0f)  },
			};

			hr = m_pRenderTarget->CreateGradientStopCollection(gradientStops, 2, &pGradientStops);
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateRadialGradientBrush(
					D2D1::RadialGradientBrushProperties(D2D1::Point2F(50, 50), D2D1::Point2F(0, 0), 50, 50),
					pGradientStops,
					&ramenTypeOpacityBrush[0]
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateRadialGradientBrush(
					D2D1::RadialGradientBrushProperties(D2D1::Point2F(50, 50), D2D1::Point2F(0, 0), 50, 50),
					pGradientStops,
					&ramenTypeOpacityBrush[1]
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateRadialGradientBrush(
					D2D1::RadialGradientBrushProperties(D2D1::Point2F(50, 50), D2D1::Point2F(0, 0), 50, 50),
					pGradientStops,
					&ramenTypeOpacityBrush[2]
				);
			}
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					sonnim_Bitmap,
					propertiesXClampYClamp,
					&sonnim_BitmapBrush
				);
			}
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					malpungsun_Bitmap,
					propertiesXClampYClamp,
					&malpungsun_BitmapBrush
				);
			}
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					fly_Bitmap,
					propertiesXClampYClamp,
					&fly_BitmapBrush
				);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					gasrangeBitmap,
					propertiesXClampYClamp,
					&gasrangeBitmapBrush
				);
			}
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(
					firegasrangeBitmap,
					propertiesXClampYClamp,
					&firegasrangeBitmapBrush
				);
			}
		}

	}

	return hr;
}
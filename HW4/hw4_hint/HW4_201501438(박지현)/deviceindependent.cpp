#include "hw3.h"

using namespace D2D1;
HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;

	// Create a WIC factory.
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	if (SUCCEEDED(hr))
	{
		// Create a shared DirectWrite factory
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
		);
	}

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",     // The font family name.
			NULL,           // The font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			13.0f,
			L"en-us",
			&m_pTextFormat
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",     // The font family name.
			NULL,           // The font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			25.0f,
			L"en-us",
			&m_score_TextFormat
		);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pDirect2dFactory->CreateRectangleGeometry(D2D1::RectF(0, 0, 100, 100), &m_pRectGeo);
	}

	/* 경로를 그리는 기하 */
	if (SUCCEEDED(hr))
	{
		hr = m_pDirect2dFactory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_DASH,
				0.0f),
			NULL,
			0,
			&m_pStrokeStyleDash
		);
	}

	// 파리 경로 그리기
	if (SUCCEEDED(hr))
	{
		hr = m_pDirect2dFactory->CreatePathGeometry(&fly_PathGeometry);

		if (SUCCEEDED(hr))
		{
			hr = fly_PathGeometry->Open(&pSink);

			if (SUCCEEDED(hr))
			{
				D2D1_POINT_2F currentLocation = { 1200.0f, 130.0f };

				pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_HOLLOW);

				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(1100, 100), D2D1::Point2F(750, 100), D2D1::Point2F(400, 135)));

				pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(750, 170), D2D1::Point2F(1100, 170), D2D1::Point2F(1200, 130)));

				/*	pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(830, 210), D2D1::Point2F(700, 225), D2D1::Point2F(600, 250)));
					pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(570, 260), D2D1::Point2F(680, 325), D2D1::Point2F(700, 350)));
					pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(690, 265), D2D1::Point2F(500, 635), D2D1::Point2F(725, 330)));
					pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(740, 440), D2D1::Point2F(765, 450), D2D1::Point2F(940, 420)));
					pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(950, 400), D2D1::Point2F(1000, 410), D2D1::Point2F(1000, 390)));
	*/
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				hr = pSink->Close();
			}
			SAFE_RELEASE(pSink);
		}
	}
	// 손님 경로 기하 그리기
	if (SUCCEEDED(hr))
	{
		hr = m_pDirect2dFactory->CreatePathGeometry(&sonnim_PathGeometry);

		if (SUCCEEDED(hr))
		{
			hr = sonnim_PathGeometry->Open(&pSink);

			if (SUCCEEDED(hr))
			{
				D2D1_POINT_2F currentLocation = { 0.0f, 500.0f };

				pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_HOLLOW);
				pSink->AddLine(D2D1::Point2F(30, 500));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				hr = pSink->Close();
			}
			SAFE_RELEASE(pSink);
		}
	}
	// 주전자 그리기
	if (SUCCEEDED(hr))
	{
		hr = m_pDirect2dFactory->CreatePathGeometry(&kettleGeometry);

		if (SUCCEEDED(hr))
		{
			hr = kettleGeometry->Open(&pSink);

			if (SUCCEEDED(hr))
			{
				D2D1_POINT_2F currentLocation = { 200.0f , 200.0f };
				// 주전자 몸통 부분
				pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_FILLED);
				pSink->AddBezier(BezierSegment(Point2F(220, 190), Point2F(230, 190), Point2F(250, 200)));
				pSink->AddBezier(BezierSegment(Point2F(260, 230), Point2F(260, 250), Point2F(250, 270)));
				pSink->AddLine(Point2F(200, 270));
				pSink->AddBezier(BezierSegment(Point2F(190, 250), Point2F(190, 230), Point2F(200, 200)));
				pSink->AddBezier(BezierSegment(Point2F(220, 210), Point2F(230, 210), Point2F(250, 200)));
				pSink->AddBezier(BezierSegment(Point2F(230, 210), Point2F(220, 210), Point2F(200, 200)));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
				// 주전자 입구 부분
				pSink->BeginFigure(Point2F(192, 230), D2D1_FIGURE_BEGIN_FILLED);
				pSink->AddBezier(BezierSegment(Point2F(170, 220), Point2F(160, 210), Point2F(175, 200)));
				pSink->AddLine(Point2F(165, 200));
				pSink->AddBezier(BezierSegment(Point2F(150, 220), Point2F(180, 240), Point2F(192, 240)));
				pSink->AddLine(Point2F(192, 230));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
				// 주전자 뚜껑 손잡이 부분
				pSink->BeginFigure(Point2F(220, 200), D2D1_FIGURE_BEGIN_FILLED);
				pSink->AddLine(Point2F(230, 200));
				pSink->AddLine(Point2F(230, 190));
				pSink->AddLine(Point2F(220, 190));
				pSink->AddLine(Point2F(220, 200));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
				// 주전자 손잡이 부분
				pSink->BeginFigure(Point2F(255, 220), D2D1_FIGURE_BEGIN_FILLED);
				pSink->AddBezier(BezierSegment(Point2F(280, 230), Point2F(280, 235), Point2F(259, 240)));
				pSink->AddLine(Point2F(258, 235));
				pSink->AddBezier(BezierSegment(Point2F(270, 230), Point2F(270, 235), Point2F(256, 225)));
				pSink->AddLine(Point2F(255, 220));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSink->Close();
			}
			SAFE_RELEASE(pSink);
		}
	}
	if (SUCCEEDED(hr))
	{
		for (int i = 0; i < 5; i++)
		{
			//hr = m_pDirect2dFactory->CreatePathGeometry(&pEnemy[i]);
			hr = m_pDirect2dFactory->CreatePathGeometry(&pViewRange[i]);
			hr = m_pDirect2dFactory->CreatePathGeometry(&pAttackRange[i]);
			//Enemy
		/*	if (SUCCEEDED(hr))
			{
				hr = pEnemy[i]->Open(&pSink);
			}
			if (SUCCEEDED(hr))
			{
				D2D1_POINT_2F StartPos = D2D1::Point2F(-10, 0);
				pSink->BeginFigure(StartPos, D2D1_FIGURE_BEGIN_FILLED);

				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(10, 0), D2D1::SizeF(10, 10), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(-10, 0), D2D1::SizeF(10, 10), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSink->Close();
			}*/
			SAFE_RELEASE(pSink);
			//ViewRange
			if (SUCCEEDED(hr))
			{
				hr = pViewRange[i]->Open(&pSink);
			}
			if (SUCCEEDED(hr))
			{
				D2D1_POINT_2F StartPos = D2D1::Point2F(-90, 0);
				pSink->BeginFigure(StartPos, D2D1_FIGURE_BEGIN_FILLED);

				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(90, 0), D2D1::SizeF(90, 90), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(-90, 0), D2D1::SizeF(90, 90), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSink->Close();
			}
			SAFE_RELEASE(pSink);
			//AttackRange
			if (SUCCEEDED(hr))
			{
				hr = pAttackRange[i]->Open(&pSink);
			}
			if (SUCCEEDED(hr))
			{
				D2D1_POINT_2F StartPos = D2D1::Point2F(-30, 0);
				pSink->BeginFigure(StartPos, D2D1_FIGURE_BEGIN_FILLED);

				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(30, 0), D2D1::SizeF(30, 30), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(-30, 0), D2D1::SizeF(30, 30), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = pSink->Close();
			}
			SAFE_RELEASE(pSink);
		}
	}

	return hr;
}
#pragma once
#include <d2d1.h>
#include <d2d1helper.h>
class Bullets{
	public:
		BOOL Visible;
		D2D1_RECT_F Source;
		D2D_POINT_2F Position;
		Bullets() {
			Visible = false;
		}
};
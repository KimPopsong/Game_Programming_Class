// HelloMath.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <math.h>  


int main()
{
	double pi = 3.1415926535;
	double x, y;

	x = pi / 4;
	y = sin(x);
	printf("sin( %f ) = %f\n", x, y);

	return 0;
}


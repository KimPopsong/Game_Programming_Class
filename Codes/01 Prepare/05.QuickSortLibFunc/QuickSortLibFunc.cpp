// QuickSortLibFunc.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdlib.h>
#include <time.h>


void print_array(const char* str, const int* list, int n);
int compare(const void* p, const void* q);


int main()
{
	srand((unsigned)time(NULL));

	int n = 5;
	int* list = (int*)malloc(sizeof(int)*n);
	for (int i = 0; i<n; i++)
		list[i] = rand();
	print_array("정렬 전", list, n);
	qsort(list, n, sizeof(list[0]), compare);
	print_array("정렬 후", list, n);
	free(list);

	return 0;
}

void print_array(const char* str, const int* list, int n)
{
	printf("%s = (", str);
	for (int i = 0; i<n; i++)
		printf(" %6d%s", list[i], (i<n - 1) ? "," : ")\n");
}

int compare(const void* p, const void* q) {
	const int* ip = (const int*)p;
	const int* iq = (const int*)q;
	return *ip - *iq;
}



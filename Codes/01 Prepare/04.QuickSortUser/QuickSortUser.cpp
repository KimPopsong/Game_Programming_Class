// QuickSortUser.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdlib.h>
#include <time.h>

void print_array(const char* str, const int* list, int n);
void qksort(int* list, int ilo, int ihi);

int main()
{
	srand((unsigned)time(NULL));

	int n = 5;
	int* list = (int*)malloc(sizeof(int)*n);
	for (int i = 0; i<n; i++)
		list[i] = rand();
	print_array("정렬 전", list, n);
	qksort(list, 0, n - 1);
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

void qksort(int* list, int ilo, int ihi)
{
	if (ilo >= ihi) return;
	int pivot = list[(ilo + ihi) / 2];
	int ieq = ilo;
	int ulo = ilo;
	int uhi = ihi;
	while (ulo <= uhi) {
		if (list[uhi] > pivot) {
			uhi--;
		}
		else {
			int tempEntry = list[ulo];
			list[ulo] = list[uhi];
			list[uhi] = tempEntry;
			if (list[ulo] < pivot) {
				tempEntry = list[ieq];
				list[ieq] = list[ulo];
				list[ulo] = tempEntry;
				ieq++;
			}
			ulo++;
		}
	}
	qksort(list, ilo, ieq - 1);
	qksort(list, uhi + 1, ihi);
}

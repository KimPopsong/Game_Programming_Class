// QuickSortStruct.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdlib.h>
#include <time.h>


typedef struct {
	int id;
	float score;
} student;

void print_array(const char* str, const student* list, int n);
int compare_student(const void* p, const void* q);


int main()
{
	srand((unsigned)time(NULL));

	int n = 5;
	student* list = (student*)malloc(sizeof(student)*n);
	for (int i = 0; i<n; i++) {
		list[i].id = i+1;
		list[i].score = (float)(rand() % 100);
	}
	print_array("정렬 전", list, n);
	qsort(list, n, sizeof(student), compare_student);
	print_array("정렬 후", list, n);
	free(list);

	return 0;
}

void print_array(const char* str, const student* list, int n)
{
	printf("%s = (", str);
	for (int i = 0; i<n; i++) {
		printf(" [%d,%d]%s", list->id, (int)list->score, (i<n - 1) ? "," : ")\n");
		list++;
	}
}

int compare_student(const void* p, const void* q)
{
	const student* ip = (const student*)p;
	const student* iq = (const student*)q;
	if (ip->score > iq->score) return 1;
	else if (ip->score < iq->score) return -1;
	else return 0;
}

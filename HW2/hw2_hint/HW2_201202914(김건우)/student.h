#pragma once

#include <string>

#define MAX_STUDENT 10
#define MAX_LAST_NAME 18
#define MAX_FIRST_NAME 21

typedef struct {
  int grade;				// ����
  std::string name;		// �̸�
} Student;

Student MakeStudent() {
  Student s;
  std::string name = "";

  char* lastnames[MAX_LAST_NAME] = {
    "��", "��", "��",
    "��", "��", "��",
    "��", "��", "��",
    "��", "��", "��",
    "ȫ", "��", "��",
    "��", "��", "��" };		// ���� ����
  char* firstnames[MAX_FIRST_NAME] = {
    "��", "��", "��",
    "��", "��", "ö",
    "��", "��", "��",
    "��", "��", "��",
    "��", "��", "��",
    "��", "��", "ȣ",
    "��", "��", "��" };		// ���� �̸�

  int tmp = rand() % 100;
  s.grade = tmp;													// ������ 0~100 ����

  name = lastnames[rand() % MAX_LAST_NAME];	// ���� ����

  for (int j = 0; j < (rand() % 3) + 1; j++) 			// ���� ������ �̸��� 1~3��
    name += firstnames[rand() % MAX_FIRST_NAME];

  s.name = name;

  return s;
}
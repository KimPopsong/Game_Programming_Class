#pragma once

#include <string>

#define MAX_STUDENT 10
#define MAX_LAST_NAME 18
#define MAX_FIRST_NAME 21

typedef struct {
  int grade;				// 성적
  std::string name;		// 이름
} Student;

Student MakeStudent() {
  Student s;
  std::string name = "";

  char* lastnames[MAX_LAST_NAME] = {
    "김", "이", "박",
    "최", "강", "유",
    "노", "정", "오",
    "송", "서", "한",
    "홍", "고", "신",
    "조", "장", "임" };		// 랜덤 성씨
  char* firstnames[MAX_FIRST_NAME] = {
    "주", "지", "정",
    "수", "인", "철",
    "현", "석", "준",
    "은", "연", "범",
    "동", "훈", "영",
    "소", "하", "호",
    "진", "명", "윤" };		// 랜덤 이름

  int tmp = rand() % 100;
  s.grade = tmp;													// 성적은 0~100 정수

  name = lastnames[rand() % MAX_LAST_NAME];	// 랜덤 성씨

  for (int j = 0; j < (rand() % 3) + 1; j++) 			// 성씨 제외한 이름은 1~3자
    name += firstnames[rand() % MAX_FIRST_NAME];

  s.name = name;

  return s;
}
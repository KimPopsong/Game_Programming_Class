#pragma once
#include "FiniteStateMachine.h"
#include "hw3.h"

#define FIRST_VELOCITY_HERO		50
#define FIRST_VELOCITY_CHASER	30
#define FIRST_VELOCITY_BALL		70
#define FIRST_VELOCITY_BALL_Z	70
#define ANGLE_BALL				45
#define GRAVITY					9.8


class Boss
{
public:
	enum StateID {
		STATE_UNDEFINED = 0,
		STATE_STAND,		//서있는 상태
		STATE_MOVE,			//이동하는 상태
		STATE_ATTACK,		//원거리 공격 상태
		STATE_RUNAWAY,		//도망가는 상태
		STATE_FOLLOW,		//추격 상태
	};

	enum EventID {
		EVENT_UNDEFINED = 0,
		EVENT_FINDTARGET,		//목표를 찾음 이벤트
		EVENT_LOSTTARGET,		//목표를 놓침 이벤트
		EVENT_BEATTACKED,		//원거리 공격 이벤트
		EVENT_HEALTHDRAINED,	//도망 이벤트
		EVENT_STOPWALK,			//걷기 중지 이벤트
		EVENT_DUBIOUS,			//탐색 이벤트
		EVENT_WITHINATTACK,		//공격, 추격 이벤트
		EVENT_OUTOFATTACK,		//공격 범위 벗어남 이벤트
	};

	enum Type {
		TYPE_UNDEFINED = 0,
		TYPE_HUMAN,
		TYPE_AI,
	};

	D2D_POINT_2F pos;		// 현재위치
	float velocity;			// 현재속력
	float force;			// 힘
	float mass;				// 질량
	float angle;			// 각도
	float a, b;				// 이동직선 Y=aX+b를 나타내는 변수
	float attack_time;		// 원거리 공격 시간 
	int energe;				// boss energe
	bool random;			// 랜덤좌표생성시 플래그
	bool chaser;			// 현재좌표생성시 플래그
	bool runaway;			// runaway상태 플래그
	bool attack;			// attack상태 플래그
	bool attack_cooltime;	// 원거리 공격 쿨타임
	D2D_POINT_2F random_pos;// 랜덤좌표
	float destPosition_x, destPosition_y; //목표좌표

private:
	Type type;
	FiniteStateMachine*	stateMachine;
	float HeroPosition_x, HeroPosition_y;	// 주인공의 좌표
	int length;								// 주인공과 추격자의 거리
	WCHAR state_str[50];					// 추격자의 상태 문자열


public:

	Boss(Type _type, DWORD stateTransitions[][3], int numTransitions, float x, float y, float ang);
	~Boss();

	void update();							// 추격자의 상태 갱신
	void setHeroPosition(float x, float y);	// 주인공의 좌표 지정
	void setDestPosition(float x, float y);	// 추격자의 목표좌표 지정
	WCHAR* get_state_str();					// 추격자의 상태 문자열 리턴
	float getAngle() { return angle; };		// 추격자의 이동각도

private:

	void issueEvent(DWORD event);		// 이벤트발생
	void updateAI();					// 추격자상태 갱신
	bool isVisible();					// 가시영역
	bool isAttackable();				// 공격영역
	bool isRunable();					// 도망영역

};
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
		STATE_STAND,		//���ִ� ����
		STATE_MOVE,			//�̵��ϴ� ����
		STATE_ATTACK,		//���Ÿ� ���� ����
		STATE_RUNAWAY,		//�������� ����
		STATE_FOLLOW,		//�߰� ����
	};

	enum EventID {
		EVENT_UNDEFINED = 0,
		EVENT_FINDTARGET,		//��ǥ�� ã�� �̺�Ʈ
		EVENT_LOSTTARGET,		//��ǥ�� ��ħ �̺�Ʈ
		EVENT_BEATTACKED,		//���Ÿ� ���� �̺�Ʈ
		EVENT_HEALTHDRAINED,	//���� �̺�Ʈ
		EVENT_STOPWALK,			//�ȱ� ���� �̺�Ʈ
		EVENT_DUBIOUS,			//Ž�� �̺�Ʈ
		EVENT_WITHINATTACK,		//����, �߰� �̺�Ʈ
		EVENT_OUTOFATTACK,		//���� ���� ��� �̺�Ʈ
	};

	enum Type {
		TYPE_UNDEFINED = 0,
		TYPE_HUMAN,
		TYPE_AI,
	};

	D2D_POINT_2F pos;		// ������ġ
	float velocity;			// ����ӷ�
	float force;			// ��
	float mass;				// ����
	float angle;			// ����
	float a, b;				// �̵����� Y=aX+b�� ��Ÿ���� ����
	float attack_time;		// ���Ÿ� ���� �ð� 
	int energe;				// boss energe
	bool random;			// ������ǥ������ �÷���
	bool chaser;			// ������ǥ������ �÷���
	bool runaway;			// runaway���� �÷���
	bool attack;			// attack���� �÷���
	bool attack_cooltime;	// ���Ÿ� ���� ��Ÿ��
	D2D_POINT_2F random_pos;// ������ǥ
	float destPosition_x, destPosition_y; //��ǥ��ǥ

private:
	Type type;
	FiniteStateMachine*	stateMachine;
	float HeroPosition_x, HeroPosition_y;	// ���ΰ��� ��ǥ
	int length;								// ���ΰ��� �߰����� �Ÿ�
	WCHAR state_str[50];					// �߰����� ���� ���ڿ�


public:

	Boss(Type _type, DWORD stateTransitions[][3], int numTransitions, float x, float y, float ang);
	~Boss();

	void update();							// �߰����� ���� ����
	void setHeroPosition(float x, float y);	// ���ΰ��� ��ǥ ����
	void setDestPosition(float x, float y);	// �߰����� ��ǥ��ǥ ����
	WCHAR* get_state_str();					// �߰����� ���� ���ڿ� ����
	float getAngle() { return angle; };		// �߰����� �̵�����

private:

	void issueEvent(DWORD event);		// �̺�Ʈ�߻�
	void updateAI();					// �߰��ڻ��� ����
	bool isVisible();					// ���ÿ���
	bool isAttackable();				// ���ݿ���
	bool isRunable();					// ��������

};
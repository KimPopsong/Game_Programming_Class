#include "Boss.h"
#include "hw3.h"


Boss::Boss(Type _type, DWORD stateTransitions[][3], int numTransitions, float x, float y, float ang)
{
	// ������ �߰��� �������� �ʱ�ȭ
	type = _type;
	pos.x = x;
	pos.y = y;
	velocity = FIRST_VELOCITY_CHASER;
	force = 0;
	mass = 10;
	random = false;
	chaser = false;
	runaway = true;
	attack = false;
	energe = 10;
	attack_cooltime = false;

	// ���ѻ��±�踦 �����.
	stateMachine = new FiniteStateMachine();
	for (int i = 0; i<numTransitions; i++) {
		stateMachine->addStateTransition(stateTransitions[i][0], stateTransitions[i][1], stateTransitions[i][2]);
	}
	stateMachine->setCurrentStateID(STATE_STAND);

}

Boss::~Boss()
{
	if (stateMachine != 0)
		delete stateMachine;
}


void Boss::update()
{
	updateAI();
}


void Boss::updateAI()
{
	switch (stateMachine->getCurrentStateID()) {
	case STATE_STAND:
		chaser = false;
		runaway = true;
		attack = false;
		attack_time = 0.0;
		wcscpy_s(state_str, L"STAND");

		// ���ü�� ����.
		// ��ǥ��ǥ�� ���ΰ��� ��ǥ�� ������
		if (isVisible()) {
			chaser = true;
			setDestPosition(HeroPosition_x, HeroPosition_y);
			issueEvent(Boss::EVENT_FINDTARGET);
			break;
		}

		if (isAttackable()) {
			issueEvent(Boss::EVENT_BEATTACKED);
			break;
		}

		// ������ MOVE�λ��¸� �ٲ�.
		// onrender���� ��������ǥ�� �ޱ����ؼ� true�� ������
		if ((GetTickCount() % 100 == 0) && (rand() % 10 < 7)) {
			random = true;
			issueEvent(Boss::EVENT_DUBIOUS);
			break;
		}
		break;

	case STATE_MOVE:
		attack = false;
		if (chaser == true)
			wcscpy_s(state_str, L"MOVE");
		else
			wcscpy_s(state_str, L"STAND");


		// ���ü�� ����.
		// ��ǥ��ǥ�� ���ΰ��� ��ǥ�� ������
		if (isVisible()) {
			chaser = true;
			setDestPosition(HeroPosition_x, HeroPosition_y);
			issueEvent(Boss::EVENT_FINDTARGET);
			break;
		}
		if (isAttackable()) {
			issueEvent(Boss::EVENT_BEATTACKED);
			break;
		}

		// ������ STAND�λ��¸� �ٲ�
		// chaser�� �ð������� ��ǥ�̵��� ǥ���ϱ����� flag
		if ((GetTickCount() % 100 == 0) && (rand() % 10 < 7)) {
			chaser = false;
			issueEvent(Boss::EVENT_STOPWALK);
			break;
		}
		break;

	case STATE_FOLLOW:
		wcscpy_s(state_str, L"FOLLOW");
		attack = false;

		if (!isVisible()) { // ���ü�� ������ ����. ��ǥ���� ������.
			issueEvent(Boss::EVENT_LOSTTARGET);
			break;
		}

		
		if (isRunable()) {
			issueEvent(Boss::EVENT_HEALTHDRAINED);
			break;
		}

		break;

	case STATE_ATTACK:
		attack_time += 0.1;
		attack_cooltime = true;
		wcscpy_s(state_str, L"ATTACK");
		attack = true;
		//���� ���� ���ü�� ����
		if (isVisible()) { 
			issueEvent(Boss::EVENT_FINDTARGET);
			break;
		}
		
		if (attack_time >= 30.0) {
			issueEvent(Boss::EVENT_HEALTHDRAINED);
			break;
		}
		
		break;

	case STATE_RUNAWAY:
		attack = false;
		attack_time = 0.0;
		wcscpy_s(state_str, L"RUNAWAY");
		
		//RUNAWAY���¿��� random��ǥ�� �ѹ��� �����ϱ����� ����
		if (runaway)
		{
			random = true;
			runaway = false;
		}
		

		//RUNAWAY���¿��� �̵��̳����� STAND���·� ������
		else if (chaser == false)
			issueEvent(Boss::EVENT_LOSTTARGET);

		break;
	}
}

void Boss::issueEvent(DWORD event)
{
	stateMachine->issueEvent(event);

	// ���º��� ������ �ִϸ��̼��� �ٲ�.
	// ONRENDER�� updateAI()���� �����Ϸ�
	switch (stateMachine->getCurrentStateID()) {
	case STATE_STAND:	 break;
	case STATE_MOVE:	 break;
	case STATE_ATTACK:	 break;
	case STATE_FOLLOW:	 break;
	case STATE_RUNAWAY:	 break;
	}
}

void Boss::setHeroPosition(float x, float y)
{
	// ���ΰ���ǥ�� ������
	HeroPosition_x = x;
	HeroPosition_y = y;
}

bool Boss::isVisible()
{
	// �Ÿ������� ���ΰ��� �� �� �ִ��� �˻���
	length = (int)sqrt((pos.x - HeroPosition_x) * (pos.x - HeroPosition_x) + (pos.y - HeroPosition_y)*(pos.y - HeroPosition_y));
	if (length <= 350)
		return true;
	else
		return false;
}
bool Boss::isRunable(){
	length = (int)sqrt((pos.x - HeroPosition_x) * (pos.x - HeroPosition_x) + (pos.y - HeroPosition_y)*(pos.y - HeroPosition_y));
	if (length <= 250)
		return true;
	else
		return false;
}


bool Boss::isAttackable()
{
	// �Ÿ������� ���ΰ��� ������ �� �ִ��� �˻���
	length = (int)sqrt((pos.x - HeroPosition_x) * (pos.x - HeroPosition_x) + (pos.y - HeroPosition_y)*(pos.y - HeroPosition_y));
	if (length >=600)
		return true;
	else
		return false;
}

void Boss::setDestPosition(float x, float y)
{
	//��ǥ��ǥ�� ������. ���ΰ���ǥ�ϼ��� ������ǥ�ϼ��� ����
	destPosition_x = x;
	destPosition_y = y;
}

// �߰����� ���¸� ��Ÿ���� ���ڿ� ��ȯ
WCHAR* Boss::get_state_str() { return state_str; }
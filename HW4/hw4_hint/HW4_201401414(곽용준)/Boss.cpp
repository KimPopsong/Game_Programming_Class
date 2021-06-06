#include "Boss.h"
#include "hw3.h"


Boss::Boss(Type _type, DWORD stateTransitions[][3], int numTransitions, float x, float y, float ang)
{
	// 기존의 추격자 변수들의 초기화
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

	// 유한상태기계를 만든다.
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

		// 대상물체가 보임.
		// 목표좌표를 주인공의 좌표로 설정함
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

		// 가끔씩 MOVE로상태를 바꿈.
		// onrender에서 랜덤한좌표를 받기위해서 true로 설정함
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


		// 대상물체가 보임.
		// 목표좌표를 주인공의 좌표로 설정함
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

		// 가끔씩 STAND로상태를 바꿈
		// chaser는 시간에따른 좌표이동을 표현하기위한 flag
		if ((GetTickCount() % 100 == 0) && (rand() % 10 < 7)) {
			chaser = false;
			issueEvent(Boss::EVENT_STOPWALK);
			break;
		}
		break;

	case STATE_FOLLOW:
		wcscpy_s(state_str, L"FOLLOW");
		attack = false;

		if (!isVisible()) { // 대상물체가 보이지 않음. 목표물을 놓쳤음.
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
		//공격 도중 대상물체가 보임
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
		
		//RUNAWAY상태에서 random좌표를 한번만 생성하기위한 설정
		if (runaway)
		{
			random = true;
			runaway = false;
		}
		

		//RUNAWAY상태에서 이동이끝날때 STAND상태로 변경함
		else if (chaser == false)
			issueEvent(Boss::EVENT_LOSTTARGET);

		break;
	}
}

void Boss::issueEvent(DWORD event)
{
	stateMachine->issueEvent(event);

	// 상태변이 시점에 애니메이션을 바꿈.
	// ONRENDER나 updateAI()에서 구현완료
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
	// 주인공좌표를 갱신함
	HeroPosition_x = x;
	HeroPosition_y = y;
}

bool Boss::isVisible()
{
	// 거리에따라 주인공을 볼 수 있는지 검사함
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
	// 거리에따라 주인공을 공격할 수 있는지 검사함
	length = (int)sqrt((pos.x - HeroPosition_x) * (pos.x - HeroPosition_x) + (pos.y - HeroPosition_y)*(pos.y - HeroPosition_y));
	if (length >=600)
		return true;
	else
		return false;
}

void Boss::setDestPosition(float x, float y)
{
	//목표좌표를 설정함. 주인공좌표일수도 랜덤좌표일수도 있음
	destPosition_x = x;
	destPosition_y = y;
}

// 추격자의 상태를 나타내는 문자열 반환
WCHAR* Boss::get_state_str() { return state_str; }